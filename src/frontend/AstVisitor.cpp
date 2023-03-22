#include "AstVisitor.hh"

AstVisitor::AstVisitor(CompilationUnit &comp_unit) : comp_unit(comp_unit) {
    have_main_func = false;

    in_function = false;
    in_loop = false;

    cur_type = NONE;
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
    auto &&extern_decl = ctx->externalDeclaration();
    for (auto &&extern_node : extern_decl) {
        if (auto &&global_decl = dynamic_cast<SysYParser::GlobalDeclContext *>(extern_node)) {
            global_decl->accept(this);
        } else if (auto &&global_func = dynamic_cast<SysYParser::GlobalFuncContext *>(extern_node)) {
            global_func->accept(this);
        } else { /* ignore */ }
    }
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
    assert(0);
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
    cur_type = ctx->bType()->accept(this).as<TypeID>() | (CONST | (in_function ? NONE : GLOBAL));

    auto &&const_def = ctx->constDef();

    for (auto &&def_node : const_def) {
        auto [name, value] = def_node->accept(this).as<NameValue>();
        value->fixValue(cur_type);

        if (!in_function) {
            comp_unit.insertSymbol(name, value);
        }
    }

    return nullptr;
}

antlrcpp::Any AstVisitor::visitConstDef(SysYParser::ConstDefContext *ctx) {
    std::string name = ctx->Identifier()->getText();

    auto &&dims_vec = ctx->constExp();
    auto &&init_val = ctx->constInitVal();

    auto &&arr_dims = getArrDims(dims_vec);

    BaseValuePtr value = (arr_dims.size() == 0) ? 
                            init_val->accept(this).as<BaseValuePtr>() :
                            parseConstListInit(dynamic_cast<SysYParser::ListConstInitValContext *>(init_val), arr_dims)
                            ;
    
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
        }
    }

    return nullptr;
}

antlrcpp::Any AstVisitor::visitUninitVarDef(SysYParser::UninitVarDefContext *ctx) {
    std::string name = ctx->Identifier()->getText();

    auto &&dims_vec = ctx->constExp();

    auto &&arr_dims = getArrDims(dims_vec);

    BaseValuePtr value = (arr_dims.size() == 0) ? 
                            UnInitVar::CreatePtr(cur_type) :
                            UnInitVar::CreatePtr(ListType::CreatePtr(cur_type | ARRAY, arr_dims, false))
                            ;
    
    return std::make_pair(name, value);
}

antlrcpp::Any AstVisitor::visitInitVarDef(SysYParser::InitVarDefContext *ctx) {
    std::string name = ctx->Identifier()->getText();

    auto &&dims_vec = ctx->constExp();
    auto &&init_val = ctx->initVal();

    auto &&arr_dims = getArrDims(dims_vec);

    BaseValuePtr value = (arr_dims.size() == 0) ?
                            init_val->accept(this).as<BaseValuePtr>() :
                            (
                                in_function ? 
                                    init_val->accept(this).as<BaseValuePtr>() :
                                    parseGlbVarListInit(dynamic_cast<SysYParser::ListInitvalContext *>(init_val), arr_dims)
                            );

    
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
    TypeID type_id = ctx->funcType()->accept(this).as<TypeID>();
    ScalarTypePtr scalar_type = ScalarType::CreatePtr(type_id);

    std::string func_name = ctx->Identifier()->getText();

    auto &&param_node = ctx->funcFParams();
    ParamList param_list = (param_node == nullptr) ?
                                ParamList() :
                                ctx->funcFParams()->accept(this).as<ParamList>()
                            ;

    FunctionPtr function = Function::CreatePtr(scalar_type, func_name, param_list);

    comp_unit.insertFunction(func_name, function);

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
        value = UnInitVar::CreatePtr(ListType::CreatePtr(param_tid | ARRAY, arr_dims, true));
    } else {
        value = UnInitVar::CreatePtr(param_tid);
    }
    Parameter param = std::make_pair(param_name, value);
    return param;
}

antlrcpp::Any AstVisitor::visitExp(SysYParser::ExpContext *ctx) {
    BaseValuePtr value = ctx->condExp()->accept(this).as<BaseValuePtr>();
    return value;
}

antlrcpp::Any AstVisitor::visitPrimaryExp1(SysYParser::PrimaryExp1Context *ctx) {
    BaseValuePtr value = ctx->exp()->accept(this).as<BaseValuePtr>();
    return value;
}

antlrcpp::Any AstVisitor::visitPrimaryExp3(SysYParser::PrimaryExp3Context *ctx) {
    ConstantPtr constant = ctx->number()->accept(this).as<ConstantPtr>();
    return std::static_pointer_cast<BaseValue>(constant);
}

antlrcpp::Any AstVisitor::visitNumber1(SysYParser::Number1Context *ctx) {
    int32_t number1 = 0;
    parseIntLiteral(ctx->getText().c_str(), &number1);
    ConstantPtr constant1 = Constant::CreatePtr(INT | CONST, std::variant<bool, int32_t, float>(number1));
    return constant1;
}

antlrcpp::Any AstVisitor::visitNumber2(SysYParser::Number2Context *ctx) {
    float number2 = 0;
    sscanf(ctx->getText().c_str(), "%f", &number2);
    ConstantPtr constant2 = Constant::CreatePtr(FLOAT | CONST, std::variant<bool, int32_t, float>(number2));
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

    BaseValuePtr lhs = unary_exp[0]->accept(this).as<BaseValuePtr>(), *rhs = nullptr;
    // assert(lhs != nullptr);
    return lhs;
}

antlrcpp::Any AstVisitor::visitMulOp(SysYParser::MulOpContext *ctx) {
    return visitChildren(ctx);
}

antlrcpp::Any AstVisitor::visitAddExp(SysYParser::AddExpContext *ctx) {
    auto &&mul_exp = ctx->mulExp();
    auto &&add_op  = ctx->addOp();

    BaseValuePtr lhs = mul_exp[0]->accept(this).as<BaseValuePtr>(), *rhs = nullptr;
    // assert(lhs != nullptr);
    return lhs;
}

antlrcpp::Any AstVisitor::visitAddOp(SysYParser::AddOpContext *ctx) {
    return visitChildren(ctx);
}

antlrcpp::Any AstVisitor::visitShiftExp(SysYParser::ShiftExpContext *ctx) {
    auto &&add_exp  = ctx->addExp();
    auto &&shift_op = ctx->shiftOp();

    BaseValuePtr lhs = add_exp[0]->accept(this).as<BaseValuePtr>(), *rhs = nullptr;
    // assert(lhs != nullptr);
    return lhs;
}

antlrcpp::Any AstVisitor::visitShiftOp(SysYParser::ShiftOpContext *ctx) {
    return visitChildren(ctx);
}

antlrcpp::Any AstVisitor::visitRelExp(SysYParser::RelExpContext *ctx) {
    auto &&shift_exp = ctx->shiftExp();
    auto &&rel_op    = ctx->relOp();

    BaseValuePtr lhs = shift_exp[0]->accept(this).as<BaseValuePtr>(), *rhs = nullptr;
    // assert(lhs != nullptr);
    return lhs;
}

antlrcpp::Any AstVisitor::visitRelOp(SysYParser::RelOpContext *ctx) {
    return visitChildren(ctx);
}

antlrcpp::Any AstVisitor::visitEqExp(SysYParser::EqExpContext *ctx) {
    auto &&rel_exp = ctx->relExp();
    auto &&eq_op   = ctx->eqOp();

    BaseValuePtr lhs = rel_exp[0]->accept(this).as<BaseValuePtr>(), *rhs = nullptr;
    // assert(lhs != nullptr);
    return lhs;
}

antlrcpp::Any AstVisitor::visitEqOp(SysYParser::EqOpContext *ctx) {
    return visitChildren(ctx);
}

antlrcpp::Any AstVisitor::visitAndExp(SysYParser::AndExpContext *ctx) {
    auto &&eq_exp = ctx->eqExp();

    BaseValuePtr lhs = eq_exp[0]->accept(this).as<BaseValuePtr>(), *rhs = nullptr;
    // assert(lhs != nullptr);
    return lhs;
}

antlrcpp::Any AstVisitor::visitExOrExp(SysYParser::ExOrExpContext *ctx) {
    auto &&and_exp = ctx->andExp();

    BaseValuePtr lhs = and_exp[0]->accept(this).as<BaseValuePtr>(), *rhs = nullptr;
    // assert(lhs != nullptr);
    return lhs;
}

antlrcpp::Any AstVisitor::visitInOrExp(SysYParser::InOrExpContext *ctx) {
    auto &&exor_exp = ctx->exOrExp();

    BaseValuePtr lhs = exor_exp[0]->accept(this).as<BaseValuePtr>(), *rhs = nullptr;
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
    TypeID last_type = cur_type;
    cur_type = INT;

    ArrDims arr_dims;
    for (auto &&const_exp : constExpVec) {
        BaseValuePtr base_value = const_exp->accept(this).as<BaseValuePtr>();
        ConstantPtr constant = std::dynamic_pointer_cast<Constant>(base_value);
        arr_dims.push_back(constant->getValue<int32_t>());
    }

    cur_type = last_type;
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
            const_arr.push_back(Constant::CreatePtr(INT | CONST, std::variant<bool, int32_t, float>(0)));
            ++cnt;
        }
        return;
    };

    function(node, arr_dims, const_arr);

    ConstArrayPtr value = ConstArray::CreatePtr(list_type, const_arr);

    return std::static_pointer_cast<BaseValue>(value);
}

BaseValuePtr AstVisitor::parseGlbVarListInit(SysYParser::ListInitvalContext *node, ArrDims &arr_dims) {
    ListTypePtr list_type = ListType::CreatePtr(cur_type | ARRAY, arr_dims, false);

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
            const_arr.push_back(Constant::CreatePtr(INT | CONST, std::variant<bool, int32_t, float>(0)));
            ++cnt;
        }
        return;
    };

    function(node, arr_dims, const_arr);

    ConstArrayPtr value = ConstArray::CreatePtr(list_type, const_arr);

    return std::static_pointer_cast<BaseValue>(value);
}
