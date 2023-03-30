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
            const_arr.push_back(Constant::CreatePtr(ScalarType::CreatePtr(INT | CONSTANT), std::variant<bool, int32_t, float>(0)));
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
}

}

AstVisitor::AstVisitor(CompilationUnit &comp_unit) : comp_unit(comp_unit) {
    have_main_func = false;

    in_function = false;
    in_loop = false;

    cur_type = NONE;
    cur_table = nullptr;
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

antlrcpp::Any AstVisitor::visitGlobalDecl(SysYParser::GlobalDeclContext *ctx) {
    ctx->decl()->accept(this);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitGlobalFunc(SysYParser::GlobalFuncContext *ctx) {
    ctx->funcDef()->accept(this);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitStray(SysYParser::StrayContext *ctx) {
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

    TypeID type_id = ctx->funcType()->accept(this).as<TypeID>();
    ScalarTypePtr scalar_type = ScalarType::CreatePtr(type_id);

    std::string func_name = ctx->Identifier()->getText();

    auto &&param_node = ctx->funcFParams();
    ParamList param_list = (param_node == nullptr) ?
                                ParamList() :
                                ctx->funcFParams()->accept(this).as<ParamList>()
                            ;

    BlockPtr first_block = BasicBlock::CreatePtr();
    cur_block = first_block;

    FunctionPtr function = Function::CreatePtr(scalar_type, func_name, param_list, first_block);
    cur_func = function;

    ctx->block()->accept(this);

    comp_unit.insertFunction(func_name, function);

    this->in_function = false;

    return function;
}
    
antlrcpp::Any AstVisitor::visitFuncType(SysYParser::FuncTypeContext *ctx) {
    return getTypeID(ctx->getText());
}
    
antlrcpp::Any AstVisitor::visitFuncFParams(SysYParser::FuncFParamsContext *ctx) {
    ParamList param_list;
    for (auto &&param_node : ctx->funcFParam()) {
        param_list.push_back(param_node->accept(this).as<Parameter>());
    }
    return param_list;
}
    
antlrcpp::Any AstVisitor::visitFuncFParam(SysYParser::FuncFParamContext *ctx) {
    TypeID param_tid = ctx->bType()->accept(this).as<TypeID>() | PARAM;

    std::string param_name = ctx->Identifier()->getText();

    BaseValuePtr value;
    if (ctx->getText().find("[") != std::string::npos) {
        auto &&dims_vec = ctx->constExp();
        ArrDims arr_dims = getArrDims(dims_vec);
        value = Variable::CreatePtr(ListType::CreatePtr(param_tid | ARRAY, arr_dims, true));
    } else {
        value = Variable::CreatePtr(ScalarType::CreatePtr(param_tid));
    }
    Parameter param = std::make_pair(param_name, value);
    return param;
}

antlrcpp::Any AstVisitor::visitBlock(SysYParser::BlockContext *ctx) {
    SymbolTable *last_table = this->cur_table;
    SymbolTable *new_table  = this->newLocalTable(last_table);
    this->cur_table = new_table;

    if (ctx->blockItemList() != nullptr) {
        ctx->blockItemList()->accept(this);
    }

    this->cur_table = last_table;
    
    return nullptr;
}

antlrcpp::Any AstVisitor::visitBlockItemList(SysYParser::BlockItemListContext *ctx) {
    visitChildren(ctx);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitMemoryDecl(SysYParser::MemoryDeclContext *ctx) {
    ctx->decl()->accept(this);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitBlockStmt(SysYParser::BlockStmtContext *ctx) {
    // ctx->stmt()->accept(this);
    assert(0); // don;t visit Stmt-Node currently
    return nullptr;
}

antlrcpp::Any AstVisitor::visitExp(SysYParser::ExpContext *ctx) {
    BaseValuePtr value = ctx->condExp()->accept(this).as<BaseValuePtr>();
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
    BaseTypePtr base_type = value->getBaseType();

    if (base_type->ArrayType() == false) {
        if (base_type->ConstantType() == false) {
            VariablePtr load_value = Variable::CreatePtr(ScalarType::CreatePtr(base_type->getMaskedType(INT | FLOAT, VARIABLE)));
            LoadInstPtr load_inst = LoadInst::CreatePtr(load_value, value);
            cur_block->insertInst(load_inst);
            value = load_value;
        }
    } else {
        assert(0);
    }

    return value;
}

antlrcpp::Any AstVisitor::visitPrimaryExp3(SysYParser::PrimaryExp3Context *ctx) {
    ConstantPtr constant = ctx->number()->accept(this).as<ConstantPtr>();
    return std::static_pointer_cast<BaseValue>(constant);
}

antlrcpp::Any AstVisitor::visitNumber1(SysYParser::Number1Context *ctx) {
    ConstantPtr constant1 = Constant::CreatePtr(ScalarType::CreatePtr(INT | CONSTANT), ConstType(std::stoi(ctx->getText())));
    return constant1;
}

antlrcpp::Any AstVisitor::visitNumber2(SysYParser::Number2Context *ctx) {
    ConstantPtr constant2 = Constant::CreatePtr(ScalarType::CreatePtr(FLOAT | CONSTANT), ConstType(std::stof(ctx->getText())));
    return constant2;
}

antlrcpp::Any AstVisitor::visitUnary1(SysYParser::Unary1Context *ctx) {
    BaseValuePtr value = ctx->primaryExp()->accept(this).as<BaseValuePtr>();
    return value;
}

antlrcpp::Any AstVisitor::visitUnary3(SysYParser::Unary3Context *ctx) {
    std::string unary_op = ctx->unaryOp()->getText();
    BaseValuePtr value   = ctx->unaryExp()->accept(this).as<BaseValuePtr>();
    return Value::unaryOperate(unary_op, value, cur_block);
}

antlrcpp::Any AstVisitor::visitUnaryOp(SysYParser::UnaryOpContext *ctx) {
    assert(0);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitMulExp(SysYParser::MulExpContext *ctx) {
    auto &&unary_exp = ctx->unaryExp();
    auto &&mul_op    = ctx->mulOp();

    BaseValuePtr lhs = unary_exp[0]->accept(this).as<BaseValuePtr>(), rhs = nullptr;

    size_t size = unary_exp.size();
    for (size_t idx = 1; idx < size; ++idx) {
        std::string op = mul_op[idx - 1]->getText();
        rhs = unary_exp[idx]->accept(this).as<BaseValuePtr>();
        lhs = Value::binaryOperate(op, lhs, rhs, cur_block);
    }

    return lhs;
}

antlrcpp::Any AstVisitor::visitMulOp(SysYParser::MulOpContext *ctx) {
    assert(0);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitAddExp(SysYParser::AddExpContext *ctx) {
    auto &&mul_exp = ctx->mulExp();
    auto &&add_op  = ctx->addOp();

    BaseValuePtr lhs = mul_exp[0]->accept(this).as<BaseValuePtr>(), rhs = nullptr;
    
    size_t size = mul_exp.size();
    for (size_t idx = 1; idx < size; ++idx) {
        std::string op = add_op[idx - 1]->getText();
        rhs = mul_exp[idx]->accept(this).as<BaseValuePtr>();
        lhs = Value::binaryOperate(op, lhs, rhs, cur_block);
    }
    // assert(lhs != nullptr);
    return lhs;
}

antlrcpp::Any AstVisitor::visitAddOp(SysYParser::AddOpContext *ctx) {
    assert(0);
    return nullptr;
}

antlrcpp::Any AstVisitor::visitShiftExp(SysYParser::ShiftExpContext *ctx) {
    auto &&add_exp  = ctx->addExp();
    auto &&shift_op = ctx->shiftOp();

    BaseValuePtr lhs = add_exp[0]->accept(this).as<BaseValuePtr>(), rhs = nullptr;
    // assert(lhs != nullptr);
    return lhs;
}

antlrcpp::Any AstVisitor::visitShiftOp(SysYParser::ShiftOpContext *ctx) {
    return visitChildren(ctx);
}

antlrcpp::Any AstVisitor::visitRelExp(SysYParser::RelExpContext *ctx) {
    auto &&shift_exp = ctx->shiftExp();
    auto &&rel_op    = ctx->relOp();

    BaseValuePtr lhs = shift_exp[0]->accept(this).as<BaseValuePtr>(), rhs = nullptr;
    // assert(lhs != nullptr);
    return lhs;
}

antlrcpp::Any AstVisitor::visitRelOp(SysYParser::RelOpContext *ctx) {
    return visitChildren(ctx);
}

antlrcpp::Any AstVisitor::visitEqExp(SysYParser::EqExpContext *ctx) {
    auto &&rel_exp = ctx->relExp();
    auto &&eq_op   = ctx->eqOp();

    BaseValuePtr lhs = rel_exp[0]->accept(this).as<BaseValuePtr>(), rhs = nullptr;
    // assert(lhs != nullptr);
    return lhs;
}

antlrcpp::Any AstVisitor::visitEqOp(SysYParser::EqOpContext *ctx) {
    return visitChildren(ctx);
}

antlrcpp::Any AstVisitor::visitAndExp(SysYParser::AndExpContext *ctx) {
    auto &&eq_exp = ctx->eqExp();

    BaseValuePtr lhs = eq_exp[0]->accept(this).as<BaseValuePtr>(), rhs = nullptr;
    // assert(lhs != nullptr);
    return lhs;
}

antlrcpp::Any AstVisitor::visitExOrExp(SysYParser::ExOrExpContext *ctx) {
    auto &&and_exp = ctx->andExp();

    BaseValuePtr lhs = and_exp[0]->accept(this).as<BaseValuePtr>(), rhs = nullptr;
    // assert(lhs != nullptr);
    return lhs;
}

antlrcpp::Any AstVisitor::visitInOrExp(SysYParser::InOrExpContext *ctx) {
    auto &&exor_exp = ctx->exOrExp();

    BaseValuePtr lhs = exor_exp[0]->accept(this).as<BaseValuePtr>(), rhs = nullptr;
    // assert(lhs != nullptr);
    return lhs;
}

antlrcpp::Any AstVisitor::visitLAndExp(SysYParser::LAndExpContext *ctx) {
    auto &&inor_exp = ctx->inOrExp();
    if (inor_exp.size() == 1) {
        return inor_exp[0]->accept(this).as<BaseValuePtr>();
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
    auto &&lor_exp = ctx->lOrExp();
    auto &&exp_node = ctx->exp();
    auto &&cond_exp = ctx->condExp();

    if (exp_node == nullptr) {

        return lor_exp->accept(this).as<BaseValuePtr>();
    }
    return visitChildren(ctx);
}

antlrcpp::Any AstVisitor::visitConstExp(SysYParser::ConstExpContext *ctx) {
    BaseValuePtr value = ctx->condExp()->accept(this).as<BaseValuePtr>();
    return value;
}

ArrDims AstVisitor::getArrDims(std::vector<SysYParser::ConstExpContext *> &constExpVec) {
    ArrDims arr_dims;
    for (auto &&const_exp : constExpVec) {
        BaseValuePtr base_value = const_exp->accept(this).as<BaseValuePtr>();
        ConstantPtr constant = std::dynamic_pointer_cast<Constant>(base_value);
        constant->fixValue(INT | CONST);
        arr_dims.push_back(std::get<int32_t>(constant->value));
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
    if (this->cur_func != nullptr) {
        for (auto [pname, pvalue] : this->cur_func->getParamList()) {
            if (pname == name) {
                return pvalue;
            }
        }
    }
    auto &&glb_talbe = this->comp_unit.getGlbTable().getNameValueMap();
    if (glb_talbe.find(name) != glb_talbe.end()) {
        return glb_talbe[name];
    }
    assert(0);
}
