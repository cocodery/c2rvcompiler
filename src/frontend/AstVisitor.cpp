#include "AstVisitor.hh"

namespace {

template <typename _Type, typename _ListType>
std::vector<_Type *> getInitVal(_ListType *list) {
    if constexpr (std::is_same_v<_Type, SysYParser::InitValContext>) {
        return list->initVal();
    } else if constexpr (std::is_same_v<_Type, SysYParser::ConstInitValContext>) {
        return list->constInitVal();
    } 
    assert(false);
}

template <typename _Type, typename _ListType, typename _ScalarType>
BaseValuePtr parseListInit(_ListType *node, ListTypePtr list_type, AstVisitor *_this) {
    ConstArr const_arr;
    const_arr.reserve(list_type->getArrSize());

    const ArrDims arr_dims = list_type->getArrDims();
    const ArrDims dim_size = list_type->getDimSize();
    ConstantPtr zero = (list_type->intType()) ? zero_int32 : zero_float;

    std::function<void(_ListType *, ConstArr &, size_t)> 
        function = [&](_ListType *node, ConstArr &const_arr, size_t level) {
        size_t total_size = 1;
        for (size_t idx = level; idx < arr_dims.size(); ++idx) {
            total_size *= arr_dims[idx];
        }
        if (total_size == 0) return;
        size_t cnt = 0;
        for (auto &&child : getInitVal<_Type, _ListType>(node)) {
            if (auto &&scalar_node = dynamic_cast<_ScalarType *>(child)) {
                ConstantPtr value = std::dynamic_pointer_cast<Constant>(scalar_node->accept(_this).template as<BaseValuePtr>());
                const_arr.push_back(value);
                ++cnt;
            } else {
                if (cnt > 0) {
                    for (level = 0; level < dim_size.size(); ++level) {
                        if (level == dim_size.size() - 1) assert(0);
                        if (cnt % dim_size[level] == 0) break;
                    }
                }
                level += 1;
                auto &&list_node = dynamic_cast<_ListType *>(child);
                function(list_node, const_arr, level);
                cnt += dim_size[level - 1];
            }
        }
        while (cnt < total_size) {
            const_arr.push_back(zero);
            ++cnt;
        }
        return;
    };
    function(node, const_arr, 0);

    return ConstArray::CreatePtr(list_type, const_arr);
}

}

AstVisitor::AstVisitor(CompilationUnit &_comp_unit) : comp_unit(_comp_unit) {
    have_main_func = false;

    in_loop = false;
    out_loop_block = nullptr;

    ret_addr  = nullptr;
    ret_block = nullptr;

    cur_type     = VOID;
    cur_position = GLOBAL;
    ptr_or_not   = NONE2;

    cur_block = nullptr;

    cur_func = nullptr;
    callee_func = nullptr;

    cur_table = &comp_unit.getGlbTable();
    table_list.clear();

    return_list.clear();

    target_continue = nullptr;

    break_list.clear();

    lOr_list.clear();
    lAnd_list.clear();

    addrTypeTable.clear();
}

antlrcpp::Any AstVisitor::visitChildren(antlr4::tree::ParseTree *ctx) {
    for (auto &&child : ctx->children) {
        child->accept(this);
    }
    return nullptr;
}

antlrcpp::Any AstVisitor::visitCompilationUnit(SysYParser::CompilationUnitContext *ctx) {
    visitChildren(ctx);
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
    std::string type_name = ctx->getText();
    if (type_name == "int") {
        return INT;
    } else if (type_name == "float") {
        return FLOAT;
    } 
    assert(false);
}

antlrcpp::Any AstVisitor::visitConstDecl(SysYParser::ConstDeclContext *ctx) {
    // ConstDecl don't generate any calculation llvmIR
    cur_type = ctx->bType()->accept(this).as<ATTR_TYPE>();

    for (auto &&def_node : ctx->constDef()) {
        auto [name, value] = def_node->accept(this).as<NameValue>();
        value->fixValue(cur_type);

        cur_table->insertSymbol(name, value);
        // for Local Constant-Array
        // store one copy at Global-Table to generate in .data section 
        if (cur_position == LOCAL && value->getBaseType()->IsArray()) {
            comp_unit.insertSymbol(name, value);
        }
    }
    return nullptr;
}

antlrcpp::Any AstVisitor::visitConstDef(SysYParser::ConstDefContext *ctx) {
    std::string name = ctx->Identifier()->getText();

    auto &&dims_vec = ctx->constExp();
    auto &&init_val = ctx->constInitVal();

    BaseValuePtr value = nullptr;

    if (dims_vec.size() == 0) {
        value = init_val->accept(this).as<BaseValuePtr>();
    } else {
        // Const-Array is always GLOBAL no matter GLOBAL or LOCAL
        auto &&arr_dims = getArrayDims(dims_vec);
        ListTypePtr ty_stored = ListType::CreatePtr(cur_type, IMMUTABLE, NOTPTR , ARRAY, GLOBAL, arr_dims);
        ListTypePtr ty_alloca = ListType::CreatePtr(cur_type, IMMUTABLE, POINTER, ARRAY, GLOBAL, arr_dims);
        BaseValuePtr init_value = parseListInit<
                            SysYParser::ConstInitValContext, 
                            SysYParser::ListConstInitValContext, 
                            SysYParser::ScalarConstInitValContext
                        > (dynamic_cast<SysYParser::ListConstInitValContext *>(init_val), ty_stored, this)
                        ;
        value = GlobalValue::CreatePtr(ty_alloca, init_value);
        addrTypeTable[value] = ty_stored;
    }
    return std::make_pair(name, value);
}

antlrcpp::Any AstVisitor::visitScalarConstInitVal(SysYParser::ScalarConstInitValContext *ctx) {
    return ctx->constExp()->accept(this).as<BaseValuePtr>();
}

antlrcpp::Any AstVisitor::visitListConstInitVal(SysYParser::ListConstInitValContext *ctx) {
    assert(0);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitVarDecl(SysYParser::VarDeclContext *ctx) {
    cur_type = ctx->bType()->accept(this).as<ATTR_TYPE>();

    auto &&var_def = ctx->varDef();

    for (auto &&def_node : var_def) {
        auto [name, value] = def_node->accept(this).as<NameValue>();
        value->fixValue(cur_type);

        cur_table->insertSymbol(name, value);
    }
    return nullptr;
}

antlrcpp::Any AstVisitor::visitUninitVarDef(SysYParser::UninitVarDefContext *ctx) {
    std::string name = ctx->Identifier()->getText();

    auto &&dims_vec = ctx->constExp();

    BaseValuePtr address = nullptr;
    // ty_stored and ty_alloca are created at the same time
    // and ty_alloca is used to create addr_alloca
    // so no need to check type
    if (dims_vec.size() == 0) {
        ScalarTypePtr ty_stored = (cur_position == GLOBAL) ?
                                    (cur_type == INT ? type_int_G : type_float_G) :
                                    (cur_type == INT ? type_int_L : type_float_L)
                                    ;
        ScalarTypePtr ty_alloca = (cur_position == GLOBAL) ?
                                    (cur_type == INT ? type_int_ptr_G : type_float_ptr_G) :
                                    (cur_type == INT ? type_int_ptr_L : type_float_ptr_L)
                                    ;
        if (cur_position == GLOBAL) {
            address = GlobalValue::CreatePtr(ty_alloca, UnInitVar::CreatePtr(ty_stored));
        } else {
            address = AllocaInst::DoAllocaAddr(ty_stored, ty_alloca, (in_loop ? out_loop_block : cur_block));
        }
    } else {
        ArrDims &&arr_dims = getArrayDims(dims_vec);
        ListTypePtr ty_stored = ListType::CreatePtr(cur_type, MUTABLE, NOTPTR , ARRAY, cur_position, arr_dims);
        ListTypePtr ty_alloca = ListType::CreatePtr(cur_type, MUTABLE, POINTER, ARRAY, cur_position, arr_dims);
        if (cur_position == GLOBAL) {
            address = GlobalValue::CreatePtr(ty_alloca, UnInitVar::CreatePtr(ty_stored));
        } else {
            address = AllocaInst::DoAllocaAddr(ty_stored, ty_alloca, (in_loop ? out_loop_block : cur_block));
        }
        addrTypeTable[address] = ty_stored;
    }
    return std::make_pair(name, address);
}

antlrcpp::Any AstVisitor::visitInitVarDef(SysYParser::InitVarDefContext *ctx) {
    std::string name = ctx->Identifier()->getText();

    auto &&dims_vec = ctx->constExp();
    auto &&init_val = ctx->initVal();

    BaseValuePtr address = nullptr;
    // ty_stored and ty_alloca are created at the same time
    // and ty_alloca is used to create addr_alloca
    // so no need to check type
    if (dims_vec.size() == 0) {
        ScalarTypePtr ty_stored = (cur_position == GLOBAL) ?
                                    (cur_type == INT ? type_int_G : type_float_G) :
                                    (cur_type == INT ? type_int_L : type_float_L)
                                    ;
        ScalarTypePtr ty_alloca = (cur_position == GLOBAL) ?
                                    (cur_type == INT ? type_int_ptr_G : type_float_ptr_G) :
                                    (cur_type == INT ? type_int_ptr_L : type_float_ptr_L)
                                    ;
        if (cur_position == GLOBAL) {
            address = GlobalValue::CreatePtr(ty_alloca, init_val->accept(this).as<BaseValuePtr>());
        } else {
            address = AllocaInst::DoAllocaAddr(ty_stored, ty_alloca, (in_loop ? out_loop_block : cur_block));
            StoreInst::DoStoreValue(address, init_val->accept(this).as<BaseValuePtr>(), cur_block);
        }
    } else {
        auto &&arr_dims = getArrayDims(dims_vec);
        ListTypePtr ty_stored = ListType::CreatePtr(cur_type, MUTABLE, NOTPTR , ARRAY, cur_position, arr_dims);
        ListTypePtr ty_alloca = ListType::CreatePtr(cur_type, MUTABLE, POINTER, ARRAY, cur_position, arr_dims);
        if (cur_position == GLOBAL) {
            BaseValuePtr init_value = parseListInit<
                        SysYParser::InitValContext, 
                        SysYParser::ListInitvalContext, 
                        SysYParser::ScalarInitValContext
                    > (dynamic_cast<SysYParser::ListInitvalContext *>(init_val), ty_stored, this);
            address = GlobalValue::CreatePtr(ty_alloca, init_value);
        } else {
            address = AllocaInst::DoAllocaAddr(ty_stored, ty_alloca, (in_loop ? out_loop_block : cur_block));
            parseLocalListInit(dynamic_cast<SysYParser::ListInitvalContext *>(init_val), ty_stored, address, cur_block);
        }
        addrTypeTable[address] = ty_stored;
    }
    return std::make_pair(name, address);
}

antlrcpp::Any AstVisitor::visitScalarInitVal(SysYParser::ScalarInitValContext *ctx) {
    return ctx->exp()->accept(this).as<BaseValuePtr>();
}

antlrcpp::Any AstVisitor::visitListInitval(SysYParser::ListInitvalContext *ctx) {
    assert(0);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitFuncDef(SysYParser::FuncDefContext *ctx) {
    BasicBlock::resetBlkIdx();
    Variable::resetVarIdx();

    ScalarTypePtr ret_type = ctx->funcType()->accept(this).as<ScalarTypePtr>();
    std::string func_name = ctx->Identifier()->getText();

    cur_position = PARAMETER;
    auto &&[param_name, param_list] = (ctx->funcFParams() == nullptr) ?
                                        std::make_pair(std::vector<std::string>(), ParamList()) :
                                        ctx->funcFParams()->accept(this).as<std::pair<std::vector<std::string>, ParamList>>()
                                    ;
    cur_position = LOCAL;

    NormalFuncPtr function = NormalFunction::CreatePtr(ret_type, func_name, param_list);
    comp_unit.insertFunction(function); // for recursion
    cur_func = function;

    cur_block = cur_func->createBB();

    ret_addr = ret_type->voidType() ? 
                nullptr :
                AllocaInst::DoAllocaAddr(ret_type, (ret_type->intType() ? type_int_ptr_L : type_float_ptr_L), cur_block);

    // create a local-table layer for func-parameter to convenient resolveTable
    SymbolTable *last_table = cur_table;
    cur_table = initParamList(cur_block, last_table, param_name); 

    ctx->block()->accept(this);

    ret_block = cur_func->createBB();
    cur_block->insertInst(JumpInst::CreatePtr(ret_block));

    for (auto &&ret_inst : return_list) {
        ret_inst->setTarget(ret_block);
    }
    ret_block->insertInst(
        ReturnInst::CreatePtr(
            ret_type, 
            ret_type->voidType() ? nullptr : LoadInst::DoLoadValue(ret_addr, ret_block)
        )
    );
    cur_position = GLOBAL;
    cur_table = last_table;

    clearTableList();
    lAnd_list.clear();
    lOr_list.clear();
    return_list.clear();

    return nullptr;
}
    
antlrcpp::Any AstVisitor::visitFuncType(SysYParser::FuncTypeContext *ctx) {
    std::string type_name = ctx->getText();
    if (type_name == "int") {
        return type_int_L;
    } else if (type_name == "float") {
        return type_float_L;
    } else if (type_name == "void") {
        return type_void;
    }
    assert(false);
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
    ATTR_TYPE _type = ctx->bType()->accept(this).as<ATTR_TYPE>();
    std::string param_name = ctx->Identifier()->getText();
    BaseValuePtr value = nullptr;
    if (ctx->getText().find("[") == std::string::npos) {
        value = Variable::CreatePtr((_type == INT) ? param_int : param_float);
    } else {
        auto &&dims_vec = ctx->constExp();
        ArrDims arr_dims = getArrayDims(dims_vec);
        arr_dims.insert(arr_dims.begin(), 1);
        ListTypePtr ty_stored = ListType::CreatePtr(_type, MUTABLE, NOTPTR, ARRAY, PARAMETER, arr_dims);
        value = Variable::CreatePtr((_type == INT) ? param_intp : param_floatp);
        addrTypeTable[value] = ty_stored;
    }
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
    BaseValuePtr store_value = ctx->exp()->accept(this);
    BaseValuePtr store_addr  = ctx->lVal()->accept(this);
    // in SysY, only care about '='
    StoreInst::DoStoreValue(store_addr, store_value, cur_block);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitAssignOp(SysYParser::AssignOpContext *ctx) {
    assert(0);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitExpStmt(SysYParser::ExpStmtContext *ctx) {
    visitChildren(ctx);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitBlockStmt(SysYParser::BlockStmtContext *ctx) {
    SymbolTable *last_table = cur_table;
    SymbolTable *new_table  = newLocalTable(last_table);
    cur_table = new_table;
    ctx->block()->accept(this);
    cur_table = last_table;
    return nullptr;
}

antlrcpp::Any AstVisitor::visitIfStmt(SysYParser::IfStmtContext *ctx) {
    BranchInstList last_lOr_list  = lOr_list;
    BranchInstList last_lAnd_list = lAnd_list;
    lOr_list  = BranchInstList();
    lAnd_list = BranchInstList(); 
    BaseValuePtr cond = ctx->condExp()->accept(this).as<BaseValuePtr>();

    BlockPtr last_block = cur_block;

    SymbolTable *last_table  = cur_table;
    SymbolTable *table_true  = newLocalTable(last_table);
    SymbolTable *table_false = newLocalTable(last_table);

    BlockPtr branch_true = cur_func->createBB();
    cur_table = table_true;
    cur_block = branch_true;
    ctx->stmt(0)->accept(this);
    BlockPtr true_end = cur_block;

    BlockPtr branch_false = cur_func->createBB();
    cur_table = table_false;
    cur_block = branch_false;
    if (ctx->Else() != nullptr) {
        ctx->stmt(1)->accept(this);
    }
    BlockPtr false_end = cur_block;

    for (auto &&lAnd_inst : lAnd_list) {
        lAnd_inst->setFalseTarget(branch_false);
    }
    for (auto &&lOr_inst  : lOr_list) {
        lOr_inst->setTrueTarget(branch_true);
    }
    lAnd_list = last_lAnd_list;
    lOr_list  = last_lOr_list;

    BranchInstPtr branch_inst = BranchInst::CreatePtr(cond, branch_true, branch_false);
    last_block->insertInst(branch_inst);

    BlockPtr branch_out = cur_func->createBB();
    cur_table = last_table;
    cur_block = branch_out;

    JumpInstPtr jump_inst = JumpInst::CreatePtr(branch_out);
    true_end ->insertInst(jump_inst);
    false_end->insertInst(jump_inst);

    return nullptr;
}

antlrcpp::Any AstVisitor::visitWhileLoop(SysYParser::WhileLoopContext *ctx) {
    bool last_in_loop = in_loop;
    if (last_in_loop == false) {
        out_loop_block = cur_block;
    }
    in_loop = true;

    BlockPtr block_before_cond = cur_block;
    BlockPtr cond_block_begin = cur_func->createBB();
    BlockPtr last_target_continue = target_continue;
    target_continue = cond_block_begin;

    BreakInstList last_break_list = break_list;
    break_list = BreakInstList();

    BranchInstList last_lOr_list  = lOr_list;
    BranchInstList last_lAnd_list = lAnd_list;
    lOr_list  = BranchInstList();
    lAnd_list = BranchInstList(); 
    cur_block = cond_block_begin;
    BaseValuePtr cond = ctx->condExp()->accept(this);
    BlockPtr cond_block_end = cur_block;

    SymbolTable *last_table = cur_table;
    cur_table = newLocalTable(last_table);
    BlockPtr loop_begin = cur_func->createBB();
    cur_block = loop_begin;
    ctx->stmt()->accept(this);
    block_before_cond->insertInst(JumpInst::CreatePtr(cond_block_begin));
    BlockPtr loop_end = cur_block;

    loop_end->insertInst(JumpInst::CreatePtr(cond_block_begin));

    BlockPtr loop_exit = cur_func->createBB();

    cond_block_end->insertInst(BranchInst::CreatePtr(cond, loop_begin, loop_exit));

    for (auto &&break_inst : break_list) {
        break_inst->setTarget(loop_exit);
    }
    break_list = last_break_list;

    for (auto &&lAnd_inst : lAnd_list) {
        lAnd_inst->setFalseTarget(loop_exit);
    }
    for (auto &&lOr_inst  : lOr_list) {
        lOr_inst->setTrueTarget(loop_begin);
    }
    lAnd_list = last_lAnd_list;
    lOr_list  = last_lOr_list;
    
    target_continue = last_target_continue;
    cur_table = last_table;
    cur_block = loop_exit;
    in_loop = last_in_loop;

    return nullptr;
}

antlrcpp::Any AstVisitor::visitContinueStmt(SysYParser::ContinueStmtContext *ctx) {
    assert(target_continue != nullptr);
    cur_block->insertInst(JumpInst::CreatePtr(target_continue));
    cur_block = cur_func->createBB(false);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitBreakStmt(SysYParser::BreakStmtContext *ctx) {
    JumpInstPtr break_inst = JumpInst::CreatePtr(nullptr);
    cur_block->insertInst(break_inst);
    break_list.push_back(break_inst);
    cur_block = cur_func->createBB(false);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitReturnStmt(SysYParser::ReturnStmtContext *ctx) {
    ScalarTypePtr ret_type  = cur_func->getReturnType();
    if (ret_type->voidType()) {
        assert(ctx->exp() == nullptr);
    } else {
        assert(ctx->exp() != nullptr);
        BaseValuePtr ret_value = Value::scalarTypeConvert(ret_type->getAttrType(), ctx->exp()->accept(this).as<BaseValuePtr>(), cur_block);
        StoreInst::DoStoreValue(ret_addr, ret_value, cur_block);
    }
    JumpInstPtr ret_inst = JumpInst::CreatePtr(nullptr);
    cur_block->insertInst(ret_inst);
    return_list.push_back(ret_inst);

    cur_block = cur_func->createBB(false);

    return nullptr;
}

antlrcpp::Any AstVisitor::visitExp(SysYParser::ExpContext *ctx) {
    return ctx->addExp()->accept(this).as<BaseValuePtr>();
}

antlrcpp::Any AstVisitor::visitLVal(SysYParser::LValContext *ctx) {
    std::string name = ctx->Identifier()->getText();
    BaseValuePtr value = resolveTable(name);
    
    BaseTypePtr type_value = value->getBaseType();
    if (type_value->IsNotPtr()) {
        // only for Constant
        return value;
    } else {
        auto &&exp_list = ctx->exp();
        if (type_value->IsScalar() && exp_list.size() == 0) {
            return value;
        }
        ListTypePtr list_type = addrTypeTable[value];
        ArrDims arr_dims = list_type->getDimSize();
        ScalarTypePtr scalar_type = type_value->intType() ? type_int_L : type_float_L;

        BaseValuePtr offset = zero_int32;
        if (exp_list.size() > 0) {
            for (size_t idx = 0; idx < exp_list.size(); ++idx) {
                ConstantPtr dim_size = Constant::CreatePtr(type_const_int, static_cast<int32_t>(arr_dims[idx]));
                BaseValuePtr cur_off = Value::binaryOperate(OP_MUL, exp_list[idx]->accept(this).as<BaseValuePtr>(), dim_size, cur_block);
                offset = Value::binaryOperate(OP_ADD, offset, cur_off, cur_block);
            }
        }
        OffsetList off_list = type_value->IsScalar() ? OffsetList() : OffsetList(1, zero_int32);
        off_list.push_back(offset);
        BaseTypePtr base_type = type_value->IsScalar() ? 
                                    std::static_pointer_cast<BaseType>(scalar_type) : 
                                    std::static_pointer_cast<BaseType>(list_type)
                                ;
        return GetElementPtrInst::DoGetPointer(base_type, value, off_list, cur_block);
    }
    assert(0);
}

antlrcpp::Any AstVisitor::visitPrimaryExp1(SysYParser::PrimaryExp1Context *ctx) {
    return ctx->exp()->accept(this).as<BaseValuePtr>();
}

antlrcpp::Any AstVisitor::visitPrimaryExp2(SysYParser::PrimaryExp2Context *ctx) {
    return ctx->lVal()->accept(this).as<BaseValuePtr>();
}

antlrcpp::Any AstVisitor::visitPrimaryExp3(SysYParser::PrimaryExp3Context *ctx) {
    return std::static_pointer_cast<BaseValue>(ctx->number()->accept(this).as<ConstantPtr>());
}

antlrcpp::Any AstVisitor::visitNumber1(SysYParser::Number1Context *ctx) {
    return Constant::CreatePtr(type_const_int, ConstType(std::stoi(ctx->getText(), nullptr, 0)));
}

antlrcpp::Any AstVisitor::visitNumber2(SysYParser::Number2Context *ctx) {
    return Constant::CreatePtr(type_const_float, ConstType(std::stof(ctx->getText())));
}

antlrcpp::Any AstVisitor::visitFuncRParams(SysYParser::FuncRParamsContext *ctx) {
    RParamList rparam_list;

    auto &&rparam_node = ctx->funcRParam();
    auto &&fparam_list = callee_func->getParamList();

    size_t rparam_size =  rparam_node.size();
    assert(rparam_size == fparam_list.size());
    rparam_list.reserve(rparam_size);

    for (size_t idx = 0; idx < rparam_size; ++idx) {
        BaseValuePtr fparam = fparam_list[idx];
        BaseValuePtr rparam = rparam_node[idx]->accept(this).as<BaseValuePtr>();

        BaseTypePtr type_rparam = rparam->getBaseType();
        BaseTypePtr type_fparam = fparam->getBaseType();
        
        if (type_fparam->IsNotPtr()) {
            assert(type_rparam->IsScalar());
            if (type_rparam->IsPointer()) {
                rparam = LoadInst::DoLoadValue(rparam, cur_block);
            }
            rparam = Value::scalarTypeConvert(type_fparam->getAttrType(), rparam, cur_block);
        } else {
            assert(type_rparam->getAttrType() == type_fparam->getAttrType());
            if (type_rparam->IsArray()) {
                rparam = GetElementPtrInst::DoGetPointer(addrTypeTable[rparam], rparam, OffsetList(1, zero_int32), cur_block);
            }
        }
        rparam_list.push_back(rparam);
    }

    return rparam_list;
}

antlrcpp::Any AstVisitor::visitFuncRParam(SysYParser::FuncRParamContext *ctx) {
    return ctx->exp()->accept(this).as<BaseValuePtr>();
}

antlrcpp::Any AstVisitor::visitUnary1(SysYParser::Unary1Context *ctx) {
    return ctx->primaryExp()->accept(this).as<BaseValuePtr>();
}

antlrcpp::Any AstVisitor::visitUnary2(SysYParser::Unary2Context *ctx) {
    std::string callee_name = ctx->Identifier()->getText();
    bool time_function = false;
    if (callee_name == "starttime" || callee_name == "stoptime") {
        callee_name = (callee_name == "starttime") ? "_sysy_starttime" : "_sysy_stoptime";
        time_function = true;
    }
    callee_func = comp_unit.getFunction(callee_name);
    ScalarTypePtr ret_type = callee_func->getReturnType();

    RParamList rparam_list = time_function ?
        RParamList(1, Constant::CreatePtr(type_const_int, static_cast<int32_t>(ctx->start->getLine()))) :
        (ctx->funcRParams() != nullptr ? ctx->funcRParams()->accept(this).as<RParamList>() : RParamList())
        ;

    return CallInst::DoCallFunction(ret_type, callee_name, rparam_list, cur_block);
}

antlrcpp::Any AstVisitor::visitUnary3(SysYParser::Unary3Context *ctx) {
    OpCode unary_op = ctx->unaryOp()->accept(this).as<OpCode>();
    BaseValuePtr value = ctx->unaryExp()->accept(this).as<BaseValuePtr>();
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
    } 
    assert(0);
}

antlrcpp::Any AstVisitor::visitMul1(SysYParser::Mul1Context *ctx) {
    return ctx->unaryExp()->accept(this).as<BaseValuePtr>();
}

antlrcpp::Any AstVisitor::visitMul2(SysYParser::Mul2Context *ctx) {
    BaseValuePtr lhs = ctx->mulExp  ()->accept(this).as<BaseValuePtr>();
    OpCode       op  = ctx->mulOp   ()->accept(this).as<OpCode>();
    BaseValuePtr rhs = ctx->unaryExp()->accept(this).as<BaseValuePtr>();
    return Value::binaryOperate(op, lhs, rhs, cur_block);
}

antlrcpp::Any AstVisitor::visitMulOp(SysYParser::MulOpContext *ctx) {
    std::string op = ctx->getText();
    if (op == "*") {
        return OP_MUL;
    } else if (op == "/") {
        return OP_DIV;
    } else if (op == "%") {
        return OP_REM;
    }
    assert(0);
}

antlrcpp::Any AstVisitor::visitAdd1(SysYParser::Add1Context *ctx) {
    return ctx->mulExp()->accept(this).as<BaseValuePtr>();
}

antlrcpp::Any AstVisitor::visitAdd2(SysYParser::Add2Context *ctx) {
    BaseValuePtr lhs = ctx->addExp()->accept(this).as<BaseValuePtr>();
    OpCode       op  = ctx->addOp ()->accept(this).as<OpCode>();
    BaseValuePtr rhs = ctx->mulExp()->accept(this).as<BaseValuePtr>();
    return Value::binaryOperate(op, lhs, rhs, cur_block);
}

antlrcpp::Any AstVisitor::visitAddOp(SysYParser::AddOpContext *ctx) {
    std::string op = ctx->getText();
    if (op == "+") {
        return OP_ADD;
    } else if (op == "-") {
        return OP_SUB;
    } 
    assert(0);
}

antlrcpp::Any AstVisitor::visitRel1(SysYParser::Rel1Context *ctx) {
    return ctx->addExp()->accept(this).as<BaseValuePtr>();
}

antlrcpp::Any AstVisitor::visitRel2(SysYParser::Rel2Context *ctx) {
    BaseValuePtr lhs = ctx->relExp()->accept(this).as<BaseValuePtr>();
    OpCode       op  = ctx->relOp ()->accept(this).as<OpCode>();
    BaseValuePtr rhs = ctx->addExp()->accept(this).as<BaseValuePtr>();
    return Value::binaryOperate(op, lhs, rhs, cur_block);
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
    }
    assert(0);
}

antlrcpp::Any AstVisitor::visitEq1(SysYParser::Eq1Context *ctx) {
    return ctx->relExp()->accept(this).as<BaseValuePtr>();
}

antlrcpp::Any AstVisitor::visitEq2(SysYParser::Eq2Context *ctx) {
    BaseValuePtr lhs = ctx->eqExp ()->accept(this).as<BaseValuePtr>();
    OpCode       op  = ctx->eqOp  ()->accept(this).as<OpCode>();
    BaseValuePtr rhs = ctx->relExp()->accept(this).as<BaseValuePtr>();
    return Value::binaryOperate(op, lhs, rhs, cur_block);
}

antlrcpp::Any AstVisitor::visitEqOp(SysYParser::EqOpContext *ctx) {
    std::string op = ctx->getText();
    if (op == "==") {
        return OP_EQU;
    } else if (op == "!=") {
        return OP_NEQ;
    } 
    assert(0);
}

antlrcpp::Any AstVisitor::visitLAnd1(SysYParser::LAnd1Context *ctx) {
    return Value::scalarTypeConvert(BOOL, ctx->eqExp()->accept(this).as<BaseValuePtr>(), cur_block);
}

antlrcpp::Any AstVisitor::visitLAnd2(SysYParser::LAnd2Context *ctx) {
    BaseValuePtr lAnd_node = Value::scalarTypeConvert(BOOL, ctx->lAndExp()->accept(this).as<BaseValuePtr>(), cur_block);
    BlockPtr lAnd_true = cur_func->createBB();
    
    BranchInstPtr br_inst = BranchInst::CreatePtr(lAnd_node, lAnd_true, nullptr);
    cur_block->insertInst(br_inst);
    lAnd_list.push_back(br_inst);

    cur_block = lAnd_true;
    return Value::scalarTypeConvert(BOOL, ctx->eqExp()->accept(this).as<BaseValuePtr>(), cur_block);
}

antlrcpp::Any AstVisitor::visitLOr1(SysYParser::LOr1Context *ctx) {
    return Value::scalarTypeConvert(BOOL, ctx->lAndExp()->accept(this).as<BaseValuePtr>(), cur_block);
}

antlrcpp::Any AstVisitor::visitLOr2(SysYParser::LOr2Context *ctx) {
    BranchInstList last_lAnd_list = lAnd_list;
    lAnd_list = BranchInstList();

    BaseValuePtr lOr_node = ctx->lOrExp()->accept(this).as<BaseValuePtr>();
    BlockPtr lOr_false = cur_func->createBB();

    for (auto &&lAnd_inst : lAnd_list) {
        lAnd_inst->setFalseTarget(lOr_false);
    }
    lAnd_list = last_lAnd_list;

    BranchInstPtr br_inst = BranchInst::CreatePtr(lOr_node, nullptr, lOr_false);
    cur_block->insertInst(br_inst);
    lOr_list.push_back(br_inst);

    cur_block = lOr_false;
    return Value::scalarTypeConvert(BOOL, ctx->lAndExp()->accept(this).as<BaseValuePtr>(), cur_block);
}

antlrcpp::Any AstVisitor::visitCondExp(SysYParser::CondExpContext *ctx) {
    return Value::scalarTypeConvert(BOOL, ctx->lOrExp()->accept(this).as<BaseValuePtr>(), cur_block);
}

antlrcpp::Any AstVisitor::visitConstExp(SysYParser::ConstExpContext *ctx) {
    BaseValuePtr constant = ctx->addExp()->accept(this).as<BaseValuePtr>();
    assert(constant->isConstant());
    return constant;
}

ArrDims AstVisitor::getArrayDims(std::vector<SysYParser::ConstExpContext *> &constExpVec) {
    ArrDims arr_dims;
    for (auto &&const_exp : constExpVec) {
        BaseValuePtr value = const_exp->accept(this).as<BaseValuePtr>();
        ConstantPtr constant = std::dynamic_pointer_cast<Constant>(value);
        constant->fixValue(INT);
        arr_dims.push_back(std::get<int32_t>(constant->getValue()));
    }
    return arr_dims;
}

SymbolTable *AstVisitor::newLocalTable(SymbolTable *parent) {
    SymbolTable *table = new SymbolTable(parent);
    table_list.push_back(table);
    return table;
}

void AstVisitor::clearTableList() {
    for (auto &&table : table_list) {
        delete table;
    }
    table_list.clear();
}

BaseValuePtr AstVisitor::resolveTable(std::string &name) {
    SymbolTable *itre_table = cur_table;
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
    auto &&param_list = cur_func->getParamList();
    for (size_t idx = 0; idx < size; ++idx) {
        auto &&name  = param_name[idx];
        auto &&param = param_list[idx];
        if (param->getBaseType()->IsPointer()) {
            new_table->insertSymbol(name, param);
        } else {
            ATTR_TYPE _type = param->getBaseType()->getAttrType();
            BaseTypePtr ty_stored = (_type == INT) ? type_int_L     : type_float_L;
            BaseTypePtr ty_alloca = (_type == INT) ? type_int_ptr_L : type_float_ptr_L;
            BaseValuePtr addr_alloca = AllocaInst::DoAllocaAddr(ty_stored, ty_alloca, first_block);
            StoreInst::DoStoreValue(addr_alloca, param, first_block);
            new_table->insertSymbol(name, addr_alloca);
        }
    }
    return new_table;
}

void AstVisitor::parseLocalListInit(SysYParser::ListInitvalContext *ctx, ListTypePtr list_type, BaseValuePtr base_addr, BlockPtr cur_block) {
    ATTR_TYPE _type = list_type->getAttrType();

    ConstantPtr zero = (_type == INT) ? zero_int32 : zero_float;
    ArrDims dim_size = list_type->getDimSize();

    std::function<void(SysYParser::ListInitvalContext *, const ArrDims &, int32_t, size_t)> 
        function = [&](SysYParser::ListInitvalContext *node, const ArrDims &arr_dims, size_t idx_offset, size_t level) {
        size_t total_size = 1;
        for (size_t idx = level; idx < arr_dims.size(); ++idx) {
            total_size *= arr_dims[idx];
        }
        if (total_size == 0) return;
        size_t cnt = 0;
        for (auto &&child : node->initVal()) {
            if (auto &&scalar_node = dynamic_cast<SysYParser::ScalarInitValContext *>(child)) {
                OffsetList off_list = OffsetList(1, zero_int32);
                ConstantPtr offset = Constant::CreatePtr(type_const_int, static_cast<int32_t>(idx_offset));
                off_list.push_back(offset);
                BaseValuePtr store_addr = GetElementPtrInst::DoGetPointer(list_type, base_addr, off_list, cur_block);
                BaseValuePtr value = Value::scalarTypeConvert(_type, scalar_node->exp()->accept(this).as<BaseValuePtr>(), cur_block);
                StoreInst::DoStoreValue(store_addr, value, cur_block);
                ++cnt;
                ++idx_offset;
            } else {
                ArrDims child_dims = arr_dims;
                if (cnt > 0) {
                    for (level = 0; level < dim_size.size(); ++level) {
                        if (level == dim_size.size() - 1) assert(0);
                        if (cnt % dim_size[level] == 0) break;
                    }
                } 
                level = level + 1;
                auto &&list_node = dynamic_cast<SysYParser::ListInitvalContext *>(child);
                function(list_node, child_dims, idx_offset, level);
                cnt += dim_size[level - 1];
                idx_offset += dim_size[level - 1];
            }
        }
        while (cnt < total_size) {
            OffsetList off_list = OffsetList(1, zero_int32);
            ConstantPtr offset = Constant::CreatePtr(type_const_int, static_cast<int32_t>(idx_offset));
            off_list.push_back(offset);
            BaseValuePtr store_addr = GetElementPtrInst::DoGetPointer(list_type, base_addr, off_list, cur_block);
            StoreInst::DoStoreValue(store_addr, zero, cur_block);
            ++cnt;
            ++idx_offset;
        }
        return;
    };
    function(ctx, list_type->getArrDims(), 0, 0);
    return;
}
