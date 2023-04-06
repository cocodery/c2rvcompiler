#include "AstVisitor.hh"

namespace {

template <typename _Type, typename _ListType>
std::vector<_Type *> getInitVal(_ListType *list) {
    if constexpr (std::is_same_v<_Type, SysYParser::InitValContext>) {
        return list->initVal();
    } else if constexpr (std::is_same_v<_Type, SysYParser::ConstInitValContext>) {
        return list->constInitVal();
    } else {
        assert(0);
    }
}

template <typename _Type, typename _ListType, typename _ScalarType>
BaseValuePtr parseListInit(_ListType *node, ArrDims &arr_dims, TypeID cur_type, AstVisitor *_this) {
    ListTypePtr list_type = ListType::CreatePtr(cur_type | ARRAY, arr_dims, false);

    ConstArr const_arr;
    const_arr.reserve(list_type->getArrDims());

    std::function<void(_ListType *, ArrDims &, ConstArr &)> 
        function = [&](_ListType *node, ArrDims &arr_dims, ConstArr &const_arr) {
        size_t total_size = 1;
        for (auto &&dim : arr_dims) {
            total_size *= dim;
        }
        if (total_size == 0) return;
        size_t cnt = 0;
        auto &&childs = getInitVal<_Type, _ListType>(node);
        for (auto &&child : childs) {
            if (auto &&scalar_node = dynamic_cast<_ScalarType *>(child)) {
                ConstantPtr value = std::dynamic_pointer_cast<Constant>(scalar_node->accept(_this).template as<BaseValuePtr>());
                const_arr.push_back(value);
                ++cnt;
            } else {
                ArrDims child_dims = arr_dims;
                child_dims.erase(child_dims.begin());

                auto &&list_node = dynamic_cast<_ListType *>(child);
                function(list_node, child_dims, const_arr);
                cnt += total_size / arr_dims[0];
            }
        }
        while (cnt < total_size) {
            if ((cur_type & INT) == INT) {
                const_arr.push_back(zero_int32);
            } else if ((cur_type & FLOAT) == FLOAT) {
                const_arr.push_back(zero_float);
            } else {
                assert(0);
            }
            ++cnt;
        }
        return;
    };

    function(node, arr_dims, const_arr);

    ConstArrayPtr value = ConstArray::CreatePtr(list_type, const_arr);

    return std::static_pointer_cast<BaseValue>(value);
}

void parseLocalListInit(SysYParser::ListInitvalContext *ctx, BaseValuePtr addr, BlockPtr cur_block) {
    // TODO
    assert(0);
}

}

AstVisitor::AstVisitor(CompilationUnit &comp_unit) : comp_unit(comp_unit) {
    have_main_func = false;

    ret_addr = nullptr;
    ret_block = nullptr;

    in_function = false;
    in_loop = false;

    cur_type = NONE;
    cur_table = &comp_unit.getGlbTable();
}

antlrcpp::Any AstVisitor::visitChildren(antlr4::tree::ParseTree *ctx) {
    for (auto &&child : ctx->children) {
        child->accept(this);
    }
    return nullptr;
}

antlrcpp::Any AstVisitor::visitCompilationUnit(SysYParser::CompilationUnitContext *ctx) {
    auto &&trans_unit = ctx->translationUnit();
    if (trans_unit != nullptr) {
        trans_unit->accept(this);
    }
    return have_main_func;
}

antlrcpp::Any AstVisitor::visitTranslationUnit(SysYParser::TranslationUnitContext *ctx) {
    visitChildren(ctx);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitDecl(SysYParser::DeclContext *ctx) {
    visitChildren(ctx);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitBType(SysYParser::BTypeContext *ctx) {
    return getTypeID(ctx->getText());
}

antlrcpp::Any AstVisitor::visitConstDecl(SysYParser::ConstDeclContext *ctx) {
    // Whatever the ConstDecl is 
    // local or global, Constant or ConstArray
    // don't generate any calculation IR
    cur_type = ctx->bType()->accept(this).as<TypeID>() | (CONST | (in_function ? NONE : GLOBAL));

    auto &&const_def = ctx->constDef();

    for (auto &&def_node : const_def) {
        auto [name, value] = def_node->accept(this).as<NameValue>();
        value->fixValue(cur_type);

        if (!in_function) {
            // global Constant or ConstArray insert inyo global-table directly
            comp_unit.insertSymbol(name, value);
        } else {
            //  local Constant or ConstArray insert inyo local -talbe for `resolveTable`
            cur_table->insertSymbol(name, value);
            //  local ConstArray insert twice into global-table for init in `.data-section`
            if (value->getBaseType()->ArrayType()) {
                comp_unit.insertSymbol(name, value);
            }
        }
    }

    return nullptr;
}

antlrcpp::Any AstVisitor::visitConstDef(SysYParser::ConstDefContext *ctx) {
    std::string name = ctx->Identifier()->getText();

    auto &&dims_vec = ctx->constExp();
    auto &&init_val = ctx->constInitVal();

    auto &&arr_dims = getArrDims(dims_vec);

    BaseValuePtr value = nullptr;

    if (arr_dims.size() == 0) {
        // for whether global-constant or local-constant
        // don't need to output to llvm-IR
        // just need to ensure get value via name from map
        value = init_val->accept(this).as<BaseValuePtr>();
    } else {
        // no matter the constant-array is global or not
        // set constant-array GLOBAL-bit put them into .data section
        BaseTypePtr  base_type  = ListType::CreatePtr(cur_type | ARRAY | GLOBAL | POINTER, arr_dims, false);
        BaseValuePtr init_value = parseListInit<
                            SysYParser::ConstInitValContext, 
                            SysYParser::ListConstInitValContext, 
                            SysYParser::ScalarConstInitValContext
                        > (dynamic_cast<SysYParser::ListConstInitValContext *>(init_val), arr_dims, cur_type, this)
                        ;
        value = GlobalValue::CreatePtr(base_type, init_value);
    }
    
    return std::make_pair(name, value);
}

antlrcpp::Any AstVisitor::visitScalarConstInitVal(SysYParser::ScalarConstInitValContext *ctx) {
    BaseValuePtr value = ctx->constExp()->accept(this).as<BaseValuePtr>();
    return value;
}

antlrcpp::Any AstVisitor::visitListConstInitVal(SysYParser::ListConstInitValContext *ctx) {
    assert(0);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitVarDecl(SysYParser::VarDeclContext *ctx) {
    cur_type = ctx->bType()->accept(this).as<TypeID>() | (VARIABLE | (in_function ? NONE : GLOBAL));

    auto &&var_def = ctx->varDef();

    for (auto &&def_node : var_def) {
        auto [name, value] = def_node->accept(this).as<NameValue>();
        value->fixValue(cur_type);

        if (!in_function) {
            comp_unit.insertSymbol(name, value);
        } else {
            cur_table->insertSymbol(name, value);
        }
    }

    return nullptr;
}

antlrcpp::Any AstVisitor::visitUninitVarDef(SysYParser::UninitVarDefContext *ctx) {
    std::string name = ctx->Identifier()->getText();

    auto &&dims_vec = ctx->constExp();

    auto &&arr_dims = getArrDims(dims_vec);

    BaseValuePtr value = nullptr;

    if (!in_function) { // insert into global-table
        if (arr_dims.size() == 0) { // global scalar un-init variable
            BaseTypePtr  _type  = ScalarType::CreatePtr(cur_type | POINTER);
            BaseValuePtr _value = UnInitVar ::CreatePtr(ScalarType::CreatePtr(cur_type));
            value = GlobalValue::CreatePtr(_type, _value);
        } else { // global list un-init variable
            BaseTypePtr  _type  = ListType ::CreatePtr(cur_type | ARRAY | POINTER, arr_dims, false);
            BaseValuePtr _value = UnInitVar::CreatePtr(ListType::CreatePtr(cur_type | ARRAY, arr_dims, false));
            value = GlobalValue::CreatePtr(_type, _value);
        }
    } else { // insert into current-table / local-table
        if (arr_dims.size() == 0) { // local scalar un-init variable
            value = Variable::CreatePtr(ScalarType::CreatePtr(cur_type | POINTER));
            InstPtr alloca_inst = AllocaInst::CreatePtr(ScalarType::CreatePtr(cur_type), value);
            cur_block->insertInst(alloca_inst);
        } else { // local list un-init variable
            value = Variable::CreatePtr(ListType::CreatePtr(cur_type | ARRAY | POINTER, arr_dims, false));
            InstPtr alloca_inst = AllocaInst::CreatePtr(ListType::CreatePtr(cur_type | ARRAY, arr_dims, false), value);
            cur_block->insertInst(alloca_inst);
        }
    }
    assert(value != nullptr);
    
    return std::make_pair(name, value);
}

antlrcpp::Any AstVisitor::visitInitVarDef(SysYParser::InitVarDefContext *ctx) {
    std::string name = ctx->Identifier()->getText();

    auto &&dims_vec = ctx->constExp();
    auto &&init_val = ctx->initVal();

    auto &&arr_dims = getArrDims(dims_vec);

    BaseValuePtr value = nullptr;

    if (!in_function) {
        if (arr_dims.size() == 0) {
            BaseTypePtr _type   = ScalarType::CreatePtr(cur_type | POINTER);
            BaseValuePtr _value = init_val->accept(this).as<BaseValuePtr>();
            value = GlobalValue::CreatePtr(_type, _value);
        } else {
            BaseTypePtr  _type  = ListType::CreatePtr(cur_type | ARRAY | POINTER, arr_dims, false);
            BaseValuePtr _value = parseListInit<
                        SysYParser::InitValContext, 
                        SysYParser::ListInitvalContext, 
                        SysYParser::ScalarInitValContext
                    > (dynamic_cast<SysYParser::ListInitvalContext *>(init_val), arr_dims, cur_type, this);
            value = GlobalValue::CreatePtr(_type, _value);
        }
    } else {
        if (arr_dims.size() == 0) { // local scalar un-init variable
            value = Variable::CreatePtr(ScalarType::CreatePtr(cur_type | POINTER));
            InstPtr alloca_inst = AllocaInst::CreatePtr(ScalarType::CreatePtr(cur_type), value);
            cur_block->insertInst(alloca_inst);
            BaseValuePtr store_value = ctx->initVal()->accept(this).as<BaseValuePtr>();
            InstPtr store_inst  = StoreInst::StoreValue2Mem(value, store_value, cur_block);
            cur_block->insertInst(store_inst);
        } else { // local list un-init variable
            value = Variable::CreatePtr(ListType::CreatePtr(cur_type | ARRAY | POINTER, arr_dims, false));
            InstPtr alloca_inst = AllocaInst::CreatePtr(ListType::CreatePtr(cur_type | ARRAY, arr_dims, false), value);
            cur_block->insertInst(alloca_inst);
            parseLocalListInit(dynamic_cast<SysYParser::ListInitvalContext *>(init_val), value, cur_block);
        }
    }
    assert(value != nullptr);
    
    return std::make_pair(name, value);
}

antlrcpp::Any AstVisitor::visitScalarInitVal(SysYParser::ScalarInitValContext *ctx) {
    BaseValuePtr value = ctx->exp()->accept(this).as<BaseValuePtr>();
    return value;
}

antlrcpp::Any AstVisitor::visitListInitval(SysYParser::ListInitvalContext *ctx) {
    assert(0);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitFuncDef(SysYParser::FuncDefContext *ctx) {
    this->in_function = true;
    BasicBlock::resetBlkIdx();
    Variable::resetVarIdx();

    TypeID type_id = ctx->funcType()->accept(this).as<TypeID>();
    ScalarTypePtr ret_type = ScalarType::CreatePtr(type_id);

    std::string func_name = ctx->Identifier()->getText();

    auto &&param_node = ctx->funcFParams();
    auto &&[param_name, param_list] = (param_node == nullptr) ?
                                        std::make_pair(std::vector<std::string>(), ParamList()) :
                                        param_node->accept(this).as<std::pair<std::vector<std::string>, ParamList>>()
                                    ;

    NormalFuncPtr function = NormalFunction::CreatePtr(ret_type, func_name, param_list);
    cur_func = function;

    cur_block = cur_func->createBB();
    ret_block = cur_func->createBB();

    // create a local-table layer for func-parameter to convenient resolveTable
    SymbolTable *last_table = this->cur_table;
    this->cur_table = initParamList(cur_block, last_table, param_name); 

    if (type_id != VOID) {
        ret_addr = Variable::CreatePtr(ScalarType::CreatePtr(type_id | VARIABLE | POINTER));
        cur_block->insertInst(AllocaInst::CreatePtr(ret_type, ret_addr));
    }

    ctx->block()->accept(this);
    cur_block->insertInst(JumpInst::CreatePtr(ret_block));
    if (type_id == VOID) {
        ret_block->insertInst(ReturnInst::CreatePtr(ret_type, nullptr));
    } else {
        BaseValuePtr func_ret_value = LoadInst::LoadValuefromMem(ret_addr, ret_block);
        ret_block->insertInst(ReturnInst::CreatePtr(ret_type, func_ret_value));
    }

    comp_unit.insertFunction(function);

    this->in_function = false;
    this->cur_table = last_table;

    return nullptr;
}
    
antlrcpp::Any AstVisitor::visitFuncType(SysYParser::FuncTypeContext *ctx) {
    return getTypeID(ctx->getText());
}
    
antlrcpp::Any AstVisitor::visitFuncFParams(SysYParser::FuncFParamsContext *ctx) {
    std::vector<std::string> param_name;
    ParamList param_list;
    for (auto &&param_node : ctx->funcFParam()) {
        auto [name, param] = param_node->accept(this).as<std::pair<std::string, BaseValuePtr>>();
        param_name.push_back(name);
        param_list.push_back(param);
    }
    return std::make_pair(param_name, param_list);
}
    
antlrcpp::Any AstVisitor::visitFuncFParam(SysYParser::FuncFParamContext *ctx) {
    TypeID param_tid = ctx->bType()->accept(this).as<TypeID>() | VARIABLE;

    std::string param_name = ctx->Identifier()->getText();

    if (ctx->getText().find("[") != std::string::npos) {
        param_tid |= POINTER;
    }
    
    BaseValuePtr value = Variable::CreatePtr(ScalarType::CreatePtr(param_tid));

    return std::make_pair(param_name, value);
}

antlrcpp::Any AstVisitor::visitBlock(SysYParser::BlockContext *ctx) {
    visitChildren(ctx);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitBlockItemList(SysYParser::BlockItemListContext *ctx) {
    visitChildren(ctx);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitBlockItem(SysYParser::BlockItemContext *ctx) {
    visitChildren(ctx);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitStmt(SysYParser::StmtContext *ctx) {
    visitChildren(ctx);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitAssignStmt(SysYParser::AssignStmtContext *ctx) {
    BaseValuePtr store_addr  = ctx->lVal()->accept(this);
    BaseValuePtr store_value = ctx->exp()->accept(this);
    // in SysY, only care about '='
    StoreInstPtr store_inst = StoreInst::StoreValue2Mem(store_addr, store_value, cur_block);
    cur_block->insertInst(store_inst);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitAssignOp(SysYParser::AssignOpContext *ctx) {
    assert(0);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitExpStmt(SysYParser::ExpStmtContext *ctx) {
    if (ctx->exp() != nullptr) {
        ctx->exp()->accept(this).as<BaseValuePtr>();
    }
    return nullptr;
}

antlrcpp::Any AstVisitor::visitBlockStmt(SysYParser::BlockStmtContext *ctx) {
    SymbolTable *last_table = this->cur_table;
    SymbolTable *new_table  = this->newLocalTable(last_table);
    this->cur_table = new_table;
    ctx->block()->accept(this);
    this->cur_table = last_table;
    return nullptr;
}

antlrcpp::Any AstVisitor::visitIfStmt(SysYParser::IfStmtContext *ctx) {
    BaseValuePtr cond = ctx->condExp()->accept(this).as<BaseValuePtr>();
    cond = scalarTypeConvert(BOOL, cond, cur_block);
    BlockPtr last_block = this->cur_block;

    SymbolTable *last_table  = this->cur_table;
    SymbolTable *table_true  = newLocalTable(last_table);
    SymbolTable *table_false = newLocalTable(last_table);

    BlockPtr bb_true = cur_func->createBB();
    this->cur_table = table_true;
    this->cur_block = bb_true;
    ctx->stmt(0)->accept(this);
    BlockPtr true_end = this->cur_block;

    BlockPtr bb_false = cur_func->createBB();
    this->cur_table = table_false;
    this->cur_block = bb_false;
    if (ctx->Else() != nullptr) {
        ctx->stmt(1)->accept(this);
    }
    BlockPtr false_end = this->cur_block;

    BranchInstPtr branch_inst = BranchInst::CreatePtr(cond, bb_true, bb_false);
    last_block->insertInst(branch_inst);

    BlockPtr branch_out = cur_func->createBB();
    this->cur_table = last_table;
    this->cur_block = branch_out;

    JumpInstPtr jump_inst = JumpInst::CreatePtr(branch_out);
    true_end ->insertInst(jump_inst);
    false_end->insertInst(jump_inst);

    return nullptr;
}

antlrcpp::Any AstVisitor::visitWhileLoop(SysYParser::WhileLoopContext *ctx) {
    assert(0);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitContinueStmt(SysYParser::ContinueStmtContext *ctx) {
    assert(0);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitBreakStmt(SysYParser::BreakStmtContext *ctx) {
    assert(0);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitReturnStmt(SysYParser::ReturnStmtContext *ctx) {
    ScalarTypePtr ret_type  = cur_func->getReturnType();
    if (ret_type->VoidType()) {
        assert(ctx->exp() == nullptr);
        cur_block->insertInst(JumpInst::CreatePtr(ret_block));
    } else {
        assert(ctx->exp() != nullptr);
        BaseValuePtr ret_value = scalarTypeConvert(ret_type->getMaskedType(VOID | INT | FLOAT), ctx->exp()->accept(this).as<BaseValuePtr>(), cur_block);
        cur_block->insertInst(StoreInst::StoreValue2Mem(ret_addr, ret_value, cur_block));
        cur_block->insertInst(JumpInst::CreatePtr(ret_block));
    }

    cur_block = cur_func->createBB(false);

    return nullptr;
}

antlrcpp::Any AstVisitor::visitExp(SysYParser::ExpContext *ctx) {
    BaseValuePtr value = ctx->addExp()->accept(this).as<BaseValuePtr>();
    return value;
}

antlrcpp::Any AstVisitor::visitLVal(SysYParser::LValContext *ctx) {
    std::string name = ctx->Identifier()->getText();
    BaseValuePtr value = resolveTable(name);
    return value;
}

antlrcpp::Any AstVisitor::visitPrimaryExp1(SysYParser::PrimaryExp1Context *ctx) {
    BaseValuePtr value = ctx->exp()->accept(this).as<BaseValuePtr>();
    return value;
}

antlrcpp::Any AstVisitor::visitPrimaryExp2(SysYParser::PrimaryExp2Context *ctx) {
    BaseValuePtr value = ctx->lVal()->accept(this).as<BaseValuePtr>();
    return value;
}

antlrcpp::Any AstVisitor::visitPrimaryExp3(SysYParser::PrimaryExp3Context *ctx) {
    ConstantPtr constant = ctx->number()->accept(this).as<ConstantPtr>();
    return std::static_pointer_cast<BaseValue>(constant);
}

antlrcpp::Any AstVisitor::visitNumber1(SysYParser::Number1Context *ctx) {
    ConstantPtr constant1 = Constant::CreatePtr(ScalarType::CreatePtr(INT | CONSTANT), ConstType(std::stoi(ctx->getText(), nullptr, 0)));
    return constant1;
}

antlrcpp::Any AstVisitor::visitNumber2(SysYParser::Number2Context *ctx) {
    ConstantPtr constant2 = Constant::CreatePtr(ScalarType::CreatePtr(FLOAT | CONSTANT), ConstType(std::stof(ctx->getText())));
    return constant2;
}

antlrcpp::Any AstVisitor::visitFuncRParams(SysYParser::FuncRParamsContext *ctx) {
    RParamList rparam_list;

    auto &&rparam_node = ctx->funcRParam();
    auto &&fparam_list = callee_func->getParamList();

    size_t rparam_size = rparam_node.size();
    assert(rparam_size == fparam_list.size());
    rparam_list.reserve(rparam_size);

    for (size_t idx = 0; idx < rparam_size; ++idx) {
        BaseValuePtr fparam = fparam_list[idx];
        BaseValuePtr rparam = rparam_node[idx]->accept(this).as<BaseValuePtr>();

        TypeID tid_rparam = rparam->getBaseType()->getMaskedType(BOOL | INT | FLOAT, POINTER);
        TypeID tid_fparam = fparam->getBaseType()->getMaskedType(       INT | FLOAT, POINTER);

        if (tid_rparam & POINTER) { // if param from parser is Pointer
            if (tid_fparam & POINTER) { // if fparam is Pointer
                assert(tid_rparam == tid_fparam); // check they have same TypeID
            } else {
                // fparam is Scalar, load from addr(rparam), and then do scalarTypeConvert
                rparam = scalarTypeConvert(tid_fparam, LoadInst::LoadValuefromMem(rparam, cur_block), cur_block);
            }
        } else {
            // do scalarTypeConvert
            rparam = scalarTypeConvert(tid_fparam, rparam, cur_block);
        }

        rparam_list.push_back(rparam);
    }

    return rparam_list;
}

antlrcpp::Any AstVisitor::visitFuncRParam(SysYParser::FuncRParamContext *ctx) {
    return ctx->exp()->accept(this).as<BaseValuePtr>();
}

antlrcpp::Any AstVisitor::visitUnary1(SysYParser::Unary1Context *ctx) {
    BaseValuePtr value = ctx->primaryExp()->accept(this).as<BaseValuePtr>();
    return value;
}

antlrcpp::Any AstVisitor::visitUnary2(SysYParser::Unary2Context *ctx) {
    std::string callee_name = ctx->Identifier()->getText();
    RParamList rparam_list;

    this->callee_func = comp_unit.getFunction(callee_name);

    if (callee_name == "starttime" || callee_name == "stoptime") {
        callee_name = (callee_name == "starttime") ? "_sysy_starttime" : "_sysy_stoptime";
        rparam_list.push_back(Constant::CreatePtr(ScalarType::CreatePtr(INT | CONSTANT), static_cast<int32_t>(ctx->start->getLine())));
    } else if (ctx->funcRParams() != nullptr) {
        rparam_list = ctx->funcRParams()->accept(this).as<RParamList>();
    }

    ScalarTypePtr ret_type = this->callee_func->getReturnType();

    BaseValuePtr ret_value = ret_type->VoidType() ? 
                                (BaseValuePtr)(nullptr) :
                                Variable::CreatePtr(ScalarType::CreatePtr(ret_type->getMaskedType(INT | FLOAT) | VARIABLE))
                                ;
    
    CallInstPtr call_inst = CallInst::CreatePtr(ret_type, ret_value, callee_name, rparam_list);
    cur_block->insertInst(call_inst);

    return ret_value;
}

antlrcpp::Any AstVisitor::visitUnary3(SysYParser::Unary3Context *ctx) {
    OpCode unary_op = ctx->unaryOp()->accept(this).as<OpCode>();
    BaseValuePtr value   = ctx->unaryExp()->accept(this).as<BaseValuePtr>();
    return Value::unaryOperate(unary_op, value, cur_block);
}

antlrcpp::Any AstVisitor::visitUnaryOp(SysYParser::UnaryOpContext *ctx) {
    std::string op = ctx->getText();
    if (op == "+") {
        return OP_ADD;
    } else if (op == "-") {
        return OP_MINUS;
    } else if (op == "!") {
        return OP_NOT;
    } else {
        assert(0);
    }
}

antlrcpp::Any AstVisitor::visitMulExp(SysYParser::MulExpContext *ctx) {
    auto &&unary_exp = ctx->unaryExp();
    auto &&mul_op    = ctx->mulOp();
    BaseValuePtr lhs = unary_exp[0]->accept(this).as<BaseValuePtr>(), rhs = nullptr;

    size_t size = unary_exp.size();
    for (size_t idx = 1; idx < size; ++idx) {
        OpCode op = mul_op[idx-1]->accept(this).as<OpCode>();
        rhs = unary_exp[idx]->accept(this).as<BaseValuePtr>();
        lhs = Value::binaryOperate(op, lhs, rhs, cur_block);
    }

    return lhs;
}

antlrcpp::Any AstVisitor::visitMulOp(SysYParser::MulOpContext *ctx) {
    std::string op = ctx->getText();
    if (op == "*") {
        return OP_MUL;
    } else if (op == "/") {
        return OP_DIV;
    } else if (op == "%") {
        return OP_REM;
    } else {
        assert(0);
    }
}

antlrcpp::Any AstVisitor::visitAddExp(SysYParser::AddExpContext *ctx) {
    auto &&mul_exp = ctx->mulExp();
    auto &&add_op  = ctx->addOp();

    BaseValuePtr lhs = mul_exp[0]->accept(this).as<BaseValuePtr>(), rhs = nullptr;
    
    size_t size = mul_exp.size();
    for (size_t idx = 1; idx < size; ++idx) {
        OpCode op = add_op[idx-1]->accept(this).as<OpCode>();
        rhs = mul_exp[idx]->accept(this).as<BaseValuePtr>();
        lhs = Value::binaryOperate(op, lhs, rhs, cur_block);
    }
    // assert(lhs != nullptr);
    return lhs;
}

antlrcpp::Any AstVisitor::visitAddOp(SysYParser::AddOpContext *ctx) {
    std::string op = ctx->getText();
    if (op == "+") {
        return OP_ADD;
    } else if (op == "-") {
        return OP_SUB;
    } else {
        assert(0);
    }
}

antlrcpp::Any AstVisitor::visitRelExp(SysYParser::RelExpContext *ctx) {
    auto &&add_exp = ctx->addExp();
    auto &&rel_op  = ctx->relOp();

    BaseValuePtr lhs = add_exp[0]->accept(this).as<BaseValuePtr>(), rhs = nullptr;

    size_t size = add_exp.size();
    for (size_t idx = 1; idx < size; ++idx) {
        OpCode op = rel_op[idx-1]->accept(this).as<OpCode>();
        rhs = add_exp[idx]->accept(this).as<BaseValuePtr>();
        lhs = Value::binaryOperate(op, lhs, rhs, cur_block);
    }
    // assert(lhs != nullptr);
    return lhs;
}

antlrcpp::Any AstVisitor::visitRelOp(SysYParser::RelOpContext *ctx) {
    std::string op = ctx->getText();
    if (op == "<") {
        return OP_LTH;
    } else if (op == ">") {
        return OP_GTH;
    } else if (op == "<=") {
        return OP_LEQ;
    } else if (op == ">=") {
        return OP_GEQ;
    } else  {
        assert(0);
    }
}

antlrcpp::Any AstVisitor::visitEqExp(SysYParser::EqExpContext *ctx) {
    auto &&rel_exp = ctx->relExp();
    auto &&eq_op   = ctx->eqOp();

    BaseValuePtr lhs = rel_exp[0]->accept(this).as<BaseValuePtr>(), rhs = nullptr;

    size_t size = rel_exp.size();
    for (size_t idx = 1; idx < size; ++idx) {
        OpCode op = eq_op[idx-1]->accept(this).as<OpCode>();
        rhs = rel_exp[idx]->accept(this).as<BaseValuePtr>();
        lhs = Value::binaryOperate(op, lhs, rhs, cur_block);
    }
    // assert(lhs != nullptr);
    return lhs;
}

antlrcpp::Any AstVisitor::visitEqOp(SysYParser::EqOpContext *ctx) {
    std::string op = ctx->getText();
    if (op == "==") {
        return OP_EQU;
    } else if (op == "!=") {
        return OP_NEQ;
    } else {
        assert(0);
    }
}

antlrcpp::Any AstVisitor::visitLAndExp(SysYParser::LAndExpContext *ctx) {
    auto &&eq_exp = ctx->eqExp();
    if (eq_exp.size() == 1) {
        return eq_exp[0]->accept(this).as<BaseValuePtr>();
    }
    return visitChildren(ctx);
}

antlrcpp::Any AstVisitor::visitLOrExp(SysYParser::LOrExpContext *ctx) {
    auto &&land_exp = ctx->lAndExp();
    if (land_exp.size() == 1) {
        return land_exp[0]->accept(this).as<BaseValuePtr>();
    }
    return visitChildren(ctx);
}

antlrcpp::Any AstVisitor::visitCondExp(SysYParser::CondExpContext *ctx) {
    return ctx->lOrExp()->accept(this).as<BaseValuePtr>();
}

antlrcpp::Any AstVisitor::visitConstExp(SysYParser::ConstExpContext *ctx) {
    BaseValuePtr value = ctx->addExp()->accept(this).as<BaseValuePtr>();
    return value;
}

ArrDims AstVisitor::getArrDims(std::vector<SysYParser::ConstExpContext *> &constExpVec) {
    ArrDims arr_dims;
    for (auto &&const_exp : constExpVec) {
        BaseValuePtr base_value = const_exp->accept(this).as<BaseValuePtr>();
        ConstantPtr constant = std::dynamic_pointer_cast<Constant>(base_value);
        constant->fixValue(INT | CONST);
        arr_dims.push_back(std::get<int32_t>(constant->getValue()));
    }
    return arr_dims;
}

BaseValuePtr AstVisitor::parseConstListInit(SysYParser::ListConstInitValContext *node, ArrDims &arr_dims) {
    ListTypePtr list_type = ListType::CreatePtr(cur_type | ARRAY, arr_dims, false);

    ConstArr const_arr;
    const_arr.reserve(list_type->getArrDims());

    std::function<void(SysYParser::ListConstInitValContext *, ArrDims &, ConstArr &)> 
        function = [&](SysYParser::ListConstInitValContext *node, ArrDims &arr_dims, ConstArr &const_arr) {
        size_t total_size = 1;
        for (auto &&dim : arr_dims) {
            total_size *= dim;
        }
        if (total_size == 0) return;
        size_t cnt = 0;
        for (auto &&child : node->constInitVal()) {
            if (auto &&scalar_node = dynamic_cast<SysYParser::ScalarConstInitValContext *>(child)) {
                ConstantPtr value = std::dynamic_pointer_cast<Constant>(scalar_node->accept(this).as<BaseValuePtr>());
                const_arr.push_back(value);
                ++cnt;
            } else {
                ArrDims child_dims = arr_dims;
                child_dims.erase(child_dims.begin());

                auto &&list_node = dynamic_cast<SysYParser::ListConstInitValContext *>(child);
                function(list_node, child_dims, const_arr);
                cnt += total_size / arr_dims[0];
            }
        }
        while (cnt < total_size) {
            const_arr.push_back(Constant::CreatePtr(ScalarType::CreatePtr(INT | CONSTANT), std::variant<bool, int32_t, float>(0)));
            ++cnt;
        }
        return;
    };

    function(node, arr_dims, const_arr);

    ConstArrayPtr value = ConstArray::CreatePtr(list_type, const_arr);

    return std::static_pointer_cast<BaseValue>(value);
}

BaseValuePtr AstVisitor::parseGlbVarListInit(SysYParser::ListInitvalContext *node, ArrDims &arr_dims) {
    ListTypePtr list_type = ListType::CreatePtr(cur_type | ARRAY | CONST, arr_dims, false);

    ConstArr const_arr;
    const_arr.reserve(list_type->getArrDims());

    std::function<void(SysYParser::ListInitvalContext *, ArrDims &, ConstArr &)> 
        function = [&](SysYParser::ListInitvalContext *node, ArrDims &arr_dims, ConstArr &const_arr) {
        size_t total_size = 1;
        for (auto &&dim : arr_dims) {
            total_size *= dim;
        }
        if (total_size == 0) return;
        size_t cnt = 0;
        for (auto &&child : node->initVal()) {
            if (auto &&scalar_node = dynamic_cast<SysYParser::ScalarInitValContext *>(child)) {
                ConstantPtr value = std::dynamic_pointer_cast<Constant>(scalar_node->accept(this).as<BaseValuePtr>());
                const_arr.push_back(value);
                ++cnt;
            } else {
                ArrDims child_dims = arr_dims;
                child_dims.erase(child_dims.begin());

                auto &&list_node = dynamic_cast<SysYParser::ListInitvalContext *>(child);
                function(list_node, child_dims, const_arr);
                cnt += total_size / arr_dims[0];
            }
        }
        while (cnt < total_size) {
            const_arr.push_back(Constant::CreatePtr(ScalarType::CreatePtr(INT | CONSTANT), std::variant<bool, int32_t, float>(0)));
            ++cnt;
        }
        return;
    };

    function(node, arr_dims, const_arr);

    ConstArrayPtr value = ConstArray::CreatePtr(list_type, const_arr);

    return std::static_pointer_cast<BaseValue>(value);
}

SymbolTable *AstVisitor::newLocalTable(SymbolTable *parent) {
    SymbolTable *table = new SymbolTable(parent);
    this->table_list.push_back(table);
    return table;
}

void AstVisitor::clearTableList() {
    for (auto &&table : this->table_list) {
        delete table;
    }
    this->table_list.clear();
}

BaseValuePtr AstVisitor::resolveTable(std::string &name) {
    SymbolTable *itre_table = this->cur_table;
    while (itre_table != nullptr) {
        auto &&sym_table = itre_table->getNameValueMap();
        if (sym_table.find(name) != sym_table.end()) {
            return sym_table[name];
        }
        itre_table = itre_table->getParentTable();
    }
    assert(0);
}

SymbolTable *AstVisitor::initParamList(BlockPtr first_block, SymbolTable *parent, std::vector<std::string> param_name) {
    SymbolTable *new_table = newLocalTable(parent);

    size_t size = param_name.size();
    auto &&param_list = this->cur_func->getParamList();
    for (size_t idx = 0; idx < size; ++idx) {
        auto &&name  = param_name[idx];
        auto &&param = param_list[idx];
        if (param->getBaseType()->PoniterType()) {
            new_table->insertSymbol(name, param);
        } else {
            TypeID tid = param->getBaseType()->getMaskedType(INT | FLOAT | VARIABLE);
            BaseValuePtr  alloca_addr = Variable::CreatePtr(ScalarType::CreatePtr(tid | POINTER));
            AllocaInstPtr alloca_inst = AllocaInst::CreatePtr(ScalarType::CreatePtr(tid), alloca_addr);
            first_block->insertInst(alloca_inst);
            StoreInstPtr store_inst = StoreInst::StoreValue2Mem(alloca_addr, param, first_block);
            first_block->insertInst(store_inst);
            new_table->insertSymbol(name, alloca_addr);
        }
    }

    return new_table;
}
