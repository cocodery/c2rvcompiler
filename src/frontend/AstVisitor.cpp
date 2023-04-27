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
BaseValuePtr parseGlobalListInit(_ListType *node, ListTypePtr list_type, AstVisitor *_this) {
    ConstArr const_arr;
    const_arr.reserve(list_type->getArrSize());

    const ArrDims arr_dims = list_type->getArrDims();
    const ArrDims dim_size = list_type->getDimSize();
    ConstantPtr zero = (list_type->IntType()) ? zero_int32 : zero_float;

    std::function<void(_ListType *, ConstArr &, size_t)> function = [&](_ListType *node, ConstArr &const_arr,
                                                                        size_t level) {
        size_t total_size = 1;
        for (size_t idx = level; idx < arr_dims.size(); ++idx) {
            total_size *= arr_dims[idx];
        }
        if (total_size == 0) return;
        size_t cnt = 0;
        for (auto &&child : getInitVal<_Type, _ListType>(node)) {
            if (auto &&scalar_node = dynamic_cast<_ScalarType *>(child)) {
                ConstantPtr value =
                    std::dynamic_pointer_cast<Constant>(std::any_cast<BaseValuePtr>(scalar_node->accept(_this)));
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

}  // namespace

AstVisitor::AstVisitor(CompilationUnit &_comp_unit) : comp_unit(_comp_unit) {
    have_main_func = false;

    in_loop = false;
    out_loop_block = nullptr;

    ret_addr = nullptr;
    ret_block = nullptr;

    cur_type = VOID;
    cur_position = GLOBAL;
    ptr_or_not = NOTPTR;

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

std::any AstVisitor::visitChildren(antlr4::tree::ParseTree *ctx) {
    for (auto &&child : ctx->children) {
        child->accept(this);
    }
    return nullptr;
}

std::any AstVisitor::visitCompilationUnit(SysYParser::CompilationUnitContext *ctx) {
    visitChildren(ctx);
    return have_main_func;
}

std::any AstVisitor::visitTranslationUnit(SysYParser::TranslationUnitContext *ctx) {
    visitChildren(ctx);
    return nullptr;
}

std::any AstVisitor::visitDecl(SysYParser::DeclContext *ctx) {
    visitChildren(ctx);
    return nullptr;
}

std::any AstVisitor::visitBType(SysYParser::BTypeContext *ctx) {
    std::string type_name = ctx->getText();
    if (type_name == "int") {
        return INT;
    } else if (type_name == "float") {
        return FLOAT;
    }
    assert(false);
}

std::any AstVisitor::visitConstDecl(SysYParser::ConstDeclContext *ctx) {
    // ConstDecl don't generate any calculation llvmIR
    cur_type = std::any_cast<ATTR_TYPE>(ctx->bType()->accept(this));

    for (auto &&def_node : ctx->constDef()) {
        auto [name, value] = std::any_cast<NameValue>(def_node->accept(this));
        value->fixValue(cur_type);

        cur_table->InsertSymbol(name, value);
        // for Local Constant-Array
        // store one copy at Global-Table to generate in .data section
        if (cur_position == LOCAL && value->getBaseType()->IsArray()) {
            comp_unit.InsertSymbol(name, value);
        }
    }
    return nullptr;
}

std::any AstVisitor::visitConstDef(SysYParser::ConstDefContext *ctx) {
    std::string name = ctx->Identifier()->getText();

    auto &&dims_vec = ctx->constExp();
    auto &&init_val = ctx->constInitVal();

    BaseValuePtr value = nullptr;

    if (dims_vec.size() == 0) {
        value = std::any_cast<BaseValuePtr>(init_val->accept(this));
    } else {
        // Const-Array is always GLOBAL no matter GLOBAL or LOCAL
        auto &&arr_dims = getArrayDims(dims_vec);
        ListTypePtr ty_stored = ListType::CreatePtr(cur_type, IMMUTABLE, NOTPTR, ARRAY, GLOBAL, arr_dims);
        ListTypePtr ty_alloca = ListType::CreatePtr(cur_type, IMMUTABLE, POINTER, ARRAY, GLOBAL, arr_dims);
        BaseValuePtr init_value =
            parseGlobalListInit<SysYParser::ConstInitValContext, SysYParser::ListConstInitValContext,
                                SysYParser::ScalarConstInitValContext>(
                dynamic_cast<SysYParser::ListConstInitValContext *>(init_val), ty_stored, this);
        value = GlobalValue::CreatePtr(ty_alloca, init_value);
        addrTypeTable[value] = ty_stored;
    }
    return std::make_pair(name, value);
}

std::any AstVisitor::visitScalarConstInitVal(SysYParser::ScalarConstInitValContext *ctx) {
    return ctx->constExp()->accept(this);
}

std::any AstVisitor::visitListConstInitVal(SysYParser::ListConstInitValContext *ctx) {
    assert(0);
    return nullptr;
}

std::any AstVisitor::visitVarDecl(SysYParser::VarDeclContext *ctx) {
    cur_type = std::any_cast<ATTR_TYPE>(ctx->bType()->accept(this));

    auto &&var_def = ctx->varDef();

    for (auto &&def_node : var_def) {
        auto [name, value] = std::any_cast<NameValue>(def_node->accept(this));
        value->fixValue(cur_type);

        cur_table->InsertSymbol(name, value);
    }
    return nullptr;
}

std::any AstVisitor::visitUninitVarDef(SysYParser::UninitVarDefContext *ctx) {
    std::string name = ctx->Identifier()->getText();

    auto &&dims_vec = ctx->constExp();

    BaseValuePtr address = nullptr;
    // ty_stored and ty_alloca are created at the same time
    // and ty_alloca is used to create addr_alloca
    // so no need to check type
    if (dims_vec.size() == 0) {
        ScalarTypePtr ty_stored = (cur_position == GLOBAL) ? (cur_type == INT ? type_int_G : type_float_G)
                                                           : (cur_type == INT ? type_int_L : type_float_L);
        ScalarTypePtr ty_alloca = (cur_position == GLOBAL) ? (cur_type == INT ? type_int_ptr_G : type_float_ptr_G)
                                                           : (cur_type == INT ? type_int_ptr_L : type_float_ptr_L);
        if (cur_position == GLOBAL) {
            address = GlobalValue::CreatePtr(ty_alloca, UnInitVar::CreatePtr(ty_stored));
        } else {
            address = AllocaInst::DoAllocaAddr(ty_stored, ty_alloca, (in_loop ? out_loop_block : cur_block));
        }
    } else {
        ArrDims &&arr_dims = getArrayDims(dims_vec);
        ListTypePtr ty_stored = ListType::CreatePtr(cur_type, MUTABLE, NOTPTR, ARRAY, cur_position, arr_dims);
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

std::any AstVisitor::visitInitVarDef(SysYParser::InitVarDefContext *ctx) {
    std::string name = ctx->Identifier()->getText();

    auto &&dims_vec = ctx->constExp();
    auto &&init_val = ctx->initVal();

    BaseValuePtr address = nullptr;
    // ty_stored and ty_alloca are created at the same time
    // and ty_alloca is used to create addr_alloca
    // so no need to check type
    if (dims_vec.size() == 0) {
        ScalarTypePtr ty_stored = (cur_position == GLOBAL) ? (cur_type == INT ? type_int_G : type_float_G)
                                                           : (cur_type == INT ? type_int_L : type_float_L);
        ScalarTypePtr ty_alloca = (cur_position == GLOBAL) ? (cur_type == INT ? type_int_ptr_G : type_float_ptr_G)
                                                           : (cur_type == INT ? type_int_ptr_L : type_float_ptr_L);
        if (cur_position == GLOBAL) {
            address = GlobalValue::CreatePtr(ty_alloca, std::any_cast<BaseValuePtr>(init_val->accept(this)));
        } else {
            address = AllocaInst::DoAllocaAddr(ty_stored, ty_alloca, (in_loop ? out_loop_block : cur_block));
            StoreInst::DoStoreValue(address, std::any_cast<BaseValuePtr>(init_val->accept(this)), cur_block);
        }
    } else {
        auto &&arr_dims = getArrayDims(dims_vec);
        ListTypePtr ty_stored = ListType::CreatePtr(cur_type, MUTABLE, NOTPTR, ARRAY, cur_position, arr_dims);
        ListTypePtr ty_alloca = ListType::CreatePtr(cur_type, MUTABLE, POINTER, ARRAY, cur_position, arr_dims);
        if (cur_position == GLOBAL) {
            BaseValuePtr init_value = parseGlobalListInit<SysYParser::InitValContext, SysYParser::ListInitvalContext,
                                                          SysYParser::ScalarInitValContext>(
                dynamic_cast<SysYParser::ListInitvalContext *>(init_val), ty_stored, this);
            address = GlobalValue::CreatePtr(ty_alloca, init_value);
        } else {
            address = AllocaInst::DoAllocaAddr(ty_stored, ty_alloca, (in_loop ? out_loop_block : cur_block));
            parseLocalListInit(dynamic_cast<SysYParser::ListInitvalContext *>(init_val), ty_stored, address, cur_block);
        }
        addrTypeTable[address] = ty_stored;
    }
    return std::make_pair(name, address);
}

std::any AstVisitor::visitScalarInitVal(SysYParser::ScalarInitValContext *ctx) {
    ptr_or_not = NOTPTR;
    BaseValuePtr init_value = std::any_cast<BaseValuePtr>(ctx->exp()->accept(this));
    assert(init_value->IsOprand());
    return init_value;
}

std::any AstVisitor::visitListInitval(SysYParser::ListInitvalContext *ctx) {
    assert(0);
    return nullptr;
}

std::any AstVisitor::visitFuncDef(SysYParser::FuncDefContext *ctx) {
    BasicBlock::ResetBlkIdx();
    Variable::ResetVarIdx();

    ScalarTypePtr ret_type = std::any_cast<ScalarTypePtr>(ctx->funcType()->accept(this));
    std::string func_name = ctx->Identifier()->getText();

    cur_position = PARAMETER;
    auto &&[param_name, param_list] =
        (ctx->funcFParams() == nullptr)
            ? std::make_pair(std::vector<std::string>(), ParamList())
            : std::any_cast<std::pair<std::vector<std::string>, ParamList>>(ctx->funcFParams()->accept(this));
    cur_position = LOCAL;

    NormalFuncPtr function = NormalFunction::CreatePtr(ret_type, func_name, param_list);
    comp_unit.InsertFunction(function);  // for recursion
    cur_func = function;

    cur_block = cur_func->CreateEntry();

    ret_addr =
        ret_type->VoidType()
            ? nullptr
            : AllocaInst::DoAllocaAddr(ret_type, (ret_type->IntType() ? type_int_ptr_L : type_float_ptr_L), cur_block);

    // create a local-table layer for func-parameter to convenient resolveTable
    SymbolTable *last_table = cur_table;
    cur_table = initParamList(cur_block, last_table, param_name);

    ctx->block()->accept(this);

    ret_block = cur_func->CreateExit();
    cur_block->InsertInst(JumpInst::CreatePtr(ret_block, cur_block));

    for (auto &&ret_inst : return_list) {
        ret_inst->setTarget(ret_block);
    }
    ret_block->InsertInst(ReturnInst::CreatePtr(
        ret_type, ret_type->VoidType() ? nullptr : LoadInst::DoLoadValue(ret_addr, ret_block), ret_block));
    cur_position = GLOBAL;
    cur_table = last_table;

    cur_func->SetVarIdx(Variable::GetVarIdx());
    cur_func->SetBlkIdx(BasicBlock::GetBlkIdx());

    out_loop_block = nullptr;
    ret_addr = nullptr;
    ret_block = nullptr;
    cur_block = nullptr;
    cur_func = nullptr;
    callee_func = nullptr;
    assert(cur_table == &comp_unit.getGlbTable());
    clearTableList();
    return_list.clear();
    target_continue = nullptr;
    break_list.clear();
    lAnd_list.clear();
    lOr_list.clear();

    return nullptr;
}

std::any AstVisitor::visitFuncType(SysYParser::FuncTypeContext *ctx) {
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

std::any AstVisitor::visitFuncFParams(SysYParser::FuncFParamsContext *ctx) {
    std::vector<std::string> param_name;
    ParamList param_list;
    for (auto &&param_node : ctx->funcFParam()) {
        auto [name, param] = std::any_cast<std::pair<std::string, BaseValuePtr>>(param_node->accept(this));
        param_name.push_back(name);
        param_list.push_back(param);
    }
    return std::make_pair(param_name, param_list);
}

std::any AstVisitor::visitFuncFParam(SysYParser::FuncFParamContext *ctx) {
    ATTR_TYPE _type = std::any_cast<ATTR_TYPE>(ctx->bType()->accept(this));
    std::string param_name = ctx->Identifier()->getText();
    BaseValuePtr value = nullptr;
    if (ctx->getText().find("[") == std::string::npos) {
        value = Variable::CreatePtr((_type == INT) ? param_int : param_float, nullptr);
    } else {
        auto &&dims_vec = ctx->constExp();
        ArrDims arr_dims = getArrayDims(dims_vec);
        arr_dims.insert(arr_dims.begin(), 1);
        ListTypePtr ty_stored = ListType::CreatePtr(_type, MUTABLE, NOTPTR, ARRAY, PARAMETER, arr_dims);
        value = Variable::CreatePtr((_type == INT) ? param_intp : param_floatp, nullptr);
        addrTypeTable[value] = ty_stored;
    }
    return std::make_pair(param_name, value);
}

std::any AstVisitor::visitBlock(SysYParser::BlockContext *ctx) {
    visitChildren(ctx);
    return nullptr;
}

std::any AstVisitor::visitBlockItemList(SysYParser::BlockItemListContext *ctx) {
    visitChildren(ctx);
    return nullptr;
}

std::any AstVisitor::visitBlockItem(SysYParser::BlockItemContext *ctx) {
    visitChildren(ctx);
    return nullptr;
}

std::any AstVisitor::visitStmt(SysYParser::StmtContext *ctx) {
    visitChildren(ctx);
    return nullptr;
}

std::any AstVisitor::visitAssignStmt(SysYParser::AssignStmtContext *ctx) {
    ptr_or_not = NOTPTR;
    BaseValuePtr store_value = std::any_cast<BaseValuePtr>(ctx->exp()->accept(this));
    assert(store_value->IsOprand());
    ptr_or_not = POINTER;
    BaseValuePtr store_addr = std::any_cast<BaseValuePtr>(ctx->lVal()->accept(this));
    ptr_or_not = NOTPTR;
    assert(store_addr->getBaseType()->IsMutable());
    // in SysY, only care about '='
    StoreInst::DoStoreValue(store_addr, store_value, cur_block);
    return nullptr;
}

std::any AstVisitor::visitAssignOp(SysYParser::AssignOpContext *ctx) {
    assert(0);
    return nullptr;
}

std::any AstVisitor::visitExpStmt(SysYParser::ExpStmtContext *ctx) {
    ptr_or_not = NOTPTR;
    visitChildren(ctx);
    return nullptr;
}

std::any AstVisitor::visitBlockStmt(SysYParser::BlockStmtContext *ctx) {
    SymbolTable *last_table = cur_table;
    SymbolTable *new_table = newLocalTable(last_table);
    cur_table = new_table;
    ctx->block()->accept(this);
    cur_table = last_table;
    return nullptr;
}

std::any AstVisitor::visitIfStmt(SysYParser::IfStmtContext *ctx) {
    BranchInstList last_lOr_list = lOr_list;
    BranchInstList last_lAnd_list = lAnd_list;
    lOr_list = BranchInstList();
    lAnd_list = BranchInstList();
    BaseValuePtr cond = std::any_cast<BaseValuePtr>(ctx->condExp()->accept(this));

    CfgNodePtr last_block = cur_block;  // last-condition block

    SymbolTable *last_table = cur_table;
    SymbolTable *table_true = newLocalTable(last_table);
    SymbolTable *table_false = newLocalTable(last_table);

    CfgNodePtr branch_true = cur_func->CreateCfgNode();  // first-block-of-true-branch
    cur_table = table_true;
    cur_block = branch_true;
    ctx->stmt(0)->accept(this);
    CfgNodePtr true_end = cur_block;  // last-block-of-true-branch

    CfgNodePtr branch_false = cur_func->CreateCfgNode();  // first-block-of-false-branch
    cur_table = table_false;
    cur_block = branch_false;
    if (ctx->Else() != nullptr) {
        ctx->stmt(1)->accept(this);
    }
    CfgNodePtr false_end = cur_block;  // last-block-of-false-branch

    for (auto &&lAnd_inst : lAnd_list) {
        lAnd_inst->setFalseTarget(branch_false);
    }
    for (auto &&lOr_inst : lOr_list) {
        lOr_inst->setTrueTarget(branch_true);
    }
    lAnd_list = last_lAnd_list;
    lOr_list = last_lOr_list;

    last_block->InsertInst(BranchInst::CreatePtr(cond, branch_true, branch_false, last_block));

    CfgNodePtr branch_out = cur_func->CreateCfgNode();  // after-branch
    cur_table = last_table;
    cur_block = branch_out;

    true_end->InsertInst(JumpInst::CreatePtr(branch_out, true_end));
    false_end->InsertInst(JumpInst::CreatePtr(branch_out, false_end));

    return nullptr;
}

std::any AstVisitor::visitWhileLoop(SysYParser::WhileLoopContext *ctx) {
    bool last_in_loop = in_loop;
    if (last_in_loop == false) {
        out_loop_block = cur_block;
    }
    in_loop = true;

    CfgNodePtr block_before_cond = cur_block;                 // block-before-enter-while-condition
    CfgNodePtr cond_block_begin = cur_func->CreateCfgNode();  // first-block-of-loop-condition

    CfgNodePtr last_target_continue = target_continue;
    target_continue = cond_block_begin;

    BreakInstList last_break_list = break_list;
    break_list = BreakInstList();

    BranchInstList last_lOr_list = lOr_list;
    BranchInstList last_lAnd_list = lAnd_list;
    lOr_list = BranchInstList();
    lAnd_list = BranchInstList();

    cur_block = cond_block_begin;
    BaseValuePtr cond = std::any_cast<BaseValuePtr>(ctx->condExp()->accept(this));
    CfgNodePtr cond_block_end = cur_block;  // last-condition block

    SymbolTable *last_table = cur_table;
    cur_table = newLocalTable(last_table);
    CfgNodePtr loop_begin = cur_func->CreateCfgNode();  // first-block-of-loop-body
    cur_block = loop_begin;
    ctx->stmt()->accept(this);
    block_before_cond->InsertInst(JumpInst::CreatePtr(cond_block_begin, block_before_cond));
    CfgNodePtr loop_end = cur_block;

    loop_end->InsertInst(JumpInst::CreatePtr(cond_block_begin, loop_end));

    CfgNodePtr loop_exit = cur_func->CreateCfgNode();  // exit-block-of-loop

    cond_block_end->InsertInst(BranchInst::CreatePtr(cond, loop_begin, loop_exit, cond_block_end));

    for (auto &&break_inst : break_list) {
        break_inst->setTarget(loop_exit);
    }
    break_list = last_break_list;

    for (auto &&lAnd_inst : lAnd_list) {
        lAnd_inst->setFalseTarget(loop_exit);
    }
    for (auto &&lOr_inst : lOr_list) {
        lOr_inst->setTrueTarget(loop_begin);
    }
    lAnd_list = last_lAnd_list;
    lOr_list = last_lOr_list;

    target_continue = last_target_continue;
    cur_table = last_table;
    cur_block = loop_exit;
    in_loop = last_in_loop;

    return nullptr;
}

std::any AstVisitor::visitContinueStmt(SysYParser::ContinueStmtContext *ctx) {
    assert(target_continue != nullptr);
    cur_block->InsertInst(JumpInst::CreatePtr(target_continue, cur_block));
    cur_block = cur_func->CreateCfgNode();
    return nullptr;
}

std::any AstVisitor::visitBreakStmt(SysYParser::BreakStmtContext *ctx) {
    JumpInstPtr break_inst = JumpInst::CreatePtr(nullptr, cur_block);
    cur_block->InsertInst(break_inst);
    break_list.push_back(break_inst);
    cur_block = cur_func->CreateCfgNode();
    return nullptr;
}

std::any AstVisitor::visitReturnStmt(SysYParser::ReturnStmtContext *ctx) {
    ScalarTypePtr ret_type = cur_func->GetReturnType();
    if (ret_type->VoidType()) {
        assert(ctx->exp() == nullptr);
    } else {
        assert(ctx->exp() != nullptr);
        ptr_or_not = NOTPTR;
        BaseValuePtr ret_value = Value::ScalarTypeConvert(
            ret_type->getAttrType(), std::any_cast<BaseValuePtr>(ctx->exp()->accept(this)), cur_block);
        StoreInst::DoStoreValue(ret_addr, ret_value, cur_block);
    }
    JumpInstPtr ret_inst = JumpInst::CreatePtr(nullptr, cur_block);
    cur_block->InsertInst(ret_inst);
    return_list.push_back(ret_inst);

    cur_block = cur_func->CreateCfgNode();

    return nullptr;
}

std::any AstVisitor::visitExp(SysYParser::ExpContext *ctx) {
    return std::any_cast<BaseValuePtr>(ctx->addExp()->accept(this));
}

std::any AstVisitor::visitLVal(SysYParser::LValContext *ctx) {
    std::string name = ctx->Identifier()->getText();
    BaseValuePtr address = resolveTable(name);
    if (address->IsConstant()) return address;

    BaseTypePtr type_addr = address->getBaseType();
    assert(type_addr->IsPointer());
    auto &&exp_list = ctx->exp();
    if (type_addr->IsScalar() && !type_addr->IsParameter()) {
        // Do NoThing
    } else {  // For Array or Pointer-Parameter
        assert(type_addr->IsPointer() && (type_addr->IsArray() || type_addr->IsParameter()));
        ListTypePtr list_type = addrTypeTable[address];
        ArrDims arr_dims = list_type->getDimSize();

        BaseValuePtr offset = zero_int32;
        ATTR_POINTER last_ptr_or_not = ptr_or_not;
        ptr_or_not = NOTPTR;
        for (size_t idx = 0; idx < exp_list.size(); ++idx) {
            ConstantPtr dim_size = Constant::CreatePtr(type_const_int, static_cast<int32_t>(arr_dims[idx]));
            BaseValuePtr cur_off = Value::BinaryOperate(
                OP_MUL, std::any_cast<BaseValuePtr>(exp_list[idx]->accept(this)), dim_size, cur_block);
            offset = Value::BinaryOperate(OP_ADD, offset, cur_off, cur_block);
        }
        ptr_or_not = last_ptr_or_not;
        OffsetList off_list = type_addr->IsScalar() ? OffsetList() : OffsetList(1, zero_int32);
        off_list.push_back(offset);
        BaseTypePtr base_type =
            type_addr->IsScalar() ? std::static_pointer_cast<BaseType>(type_addr->IntType() ? type_int_L : type_float_L)
                                  : std::static_pointer_cast<BaseType>(list_type);
        address = GetElementPtrInst::DoGetPointer(base_type, address, off_list, cur_block);
    }
    if (ptr_or_not == POINTER) {
        return address;
    } else if (ptr_or_not == NOTPTR) {
        return LoadInst::DoLoadValue(address, cur_block);
    }
    assert(0);
}

std::any AstVisitor::visitPrimaryExp1(SysYParser::PrimaryExp1Context *ctx) {
    return std::any_cast<BaseValuePtr>(ctx->exp()->accept(this));
}

std::any AstVisitor::visitPrimaryExp2(SysYParser::PrimaryExp2Context *ctx) { return ctx->lVal()->accept(this); }

std::any AstVisitor::visitPrimaryExp3(SysYParser::PrimaryExp3Context *ctx) {
    return std::static_pointer_cast<BaseValue>(std::any_cast<ConstantPtr>(ctx->number()->accept(this)));
}

std::any AstVisitor::visitNumber1(SysYParser::Number1Context *ctx) {
    return Constant::CreatePtr(type_const_int, ConstType(std::stoi(ctx->getText(), nullptr, 0)));
}

std::any AstVisitor::visitNumber2(SysYParser::Number2Context *ctx) {
    return Constant::CreatePtr(type_const_float, ConstType(std::stof(ctx->getText())));
}

std::any AstVisitor::visitFuncRParams(SysYParser::FuncRParamsContext *ctx) {
    RParamList rparam_list;

    auto &&rparam_node = ctx->funcRParam();
    auto &&fparam_list = callee_func->GetParamList();

    size_t rparam_size = rparam_node.size();
    assert(rparam_size == fparam_list.size());
    rparam_list.reserve(rparam_size);

    ATTR_POINTER last_ptr_or_not = ptr_or_not;
    for (size_t idx = 0; idx < rparam_size; ++idx) {
        BaseValuePtr fparam = fparam_list[idx];
        ptr_or_not = fparam->getBaseType()->getAttrPointer();
        BaseValuePtr rparam = std::any_cast<BaseValuePtr>(rparam_node[idx]->accept(this));

        BaseTypePtr type_rparam = rparam->getBaseType();
        BaseTypePtr type_fparam = fparam->getBaseType();

        if (type_fparam->IsNotPtr()) {
            assert(type_rparam->IsScalar());
            rparam = Value::ScalarTypeConvert(type_fparam->getAttrType(), rparam, cur_block);
        }
        rparam_list.push_back(rparam);
    }
    ptr_or_not = last_ptr_or_not;

    return rparam_list;
}

std::any AstVisitor::visitFuncRParam(SysYParser::FuncRParamContext *ctx) {
    return std::any_cast<BaseValuePtr>(ctx->exp()->accept(this));
}

std::any AstVisitor::visitUnary1(SysYParser::Unary1Context *ctx) {
    return std::any_cast<BaseValuePtr>(ctx->primaryExp()->accept(this));
}

std::any AstVisitor::visitUnary2(SysYParser::Unary2Context *ctx) {
    std::string callee_name = ctx->Identifier()->getText();
    bool time_function = false;
    if (callee_name == "starttime" || callee_name == "stoptime") {
        callee_name = (callee_name == "starttime") ? "_sysy_starttime" : "_sysy_stoptime";
        time_function = true;
    }
    callee_func = comp_unit.GetFunction(callee_name);
    ScalarTypePtr ret_type = callee_func->GetReturnType();

    RParamList rparam_list =
        time_function ? RParamList(1, Constant::CreatePtr(type_const_int, static_cast<int32_t>(ctx->start->getLine())))
                      : (ctx->funcRParams() != nullptr ? std::any_cast<RParamList>(ctx->funcRParams()->accept(this))
                                                       : RParamList());

    return CallInst::DoCallFunction(ret_type, callee_name, rparam_list, cur_block);
}

std::any AstVisitor::visitUnary3(SysYParser::Unary3Context *ctx) {
    assert(ptr_or_not == NOTPTR);
    OpCode unary_op = std::any_cast<OpCode>(ctx->unaryOp()->accept(this));
    BaseValuePtr value = std::any_cast<BaseValuePtr>(ctx->unaryExp()->accept(this));
    return Value::UnaryOperate(unary_op, value, cur_block);
}

std::any AstVisitor::visitUnaryOp(SysYParser::UnaryOpContext *ctx) {
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

std::any AstVisitor::visitMul1(SysYParser::Mul1Context *ctx) {
    return std::any_cast<BaseValuePtr>(ctx->unaryExp()->accept(this));
}

std::any AstVisitor::visitMul2(SysYParser::Mul2Context *ctx) {
    assert(ptr_or_not == NOTPTR);
    BaseValuePtr lhs = std::any_cast<BaseValuePtr>(ctx->mulExp()->accept(this));
    OpCode op = std::any_cast<OpCode>(ctx->mulOp()->accept(this));
    BaseValuePtr rhs = std::any_cast<BaseValuePtr>(ctx->unaryExp()->accept(this));
    return Value::BinaryOperate(op, lhs, rhs, cur_block);
}

std::any AstVisitor::visitMulOp(SysYParser::MulOpContext *ctx) {
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

std::any AstVisitor::visitAdd1(SysYParser::Add1Context *ctx) {
    return std::any_cast<BaseValuePtr>(ctx->mulExp()->accept(this));
}

std::any AstVisitor::visitAdd2(SysYParser::Add2Context *ctx) {
    assert(ptr_or_not == NOTPTR);
    BaseValuePtr lhs = std::any_cast<BaseValuePtr>(ctx->addExp()->accept(this));
    OpCode op = std::any_cast<OpCode>(ctx->addOp()->accept(this));
    BaseValuePtr rhs = std::any_cast<BaseValuePtr>(ctx->mulExp()->accept(this));
    return Value::BinaryOperate(op, lhs, rhs, cur_block);
}

std::any AstVisitor::visitAddOp(SysYParser::AddOpContext *ctx) {
    std::string op = ctx->getText();
    if (op == "+") {
        return OP_ADD;
    } else if (op == "-") {
        return OP_SUB;
    }
    assert(0);
}

std::any AstVisitor::visitRel1(SysYParser::Rel1Context *ctx) {
    return std::any_cast<BaseValuePtr>(ctx->addExp()->accept(this));
}

std::any AstVisitor::visitRel2(SysYParser::Rel2Context *ctx) {
    assert(ptr_or_not == NOTPTR);
    BaseValuePtr lhs = std::any_cast<BaseValuePtr>(ctx->relExp()->accept(this));
    OpCode op = std::any_cast<OpCode>(ctx->relOp()->accept(this));
    BaseValuePtr rhs = std::any_cast<BaseValuePtr>(ctx->addExp()->accept(this));
    return Value::BinaryOperate(op, lhs, rhs, cur_block);
}

std::any AstVisitor::visitRelOp(SysYParser::RelOpContext *ctx) {
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

std::any AstVisitor::visitEq1(SysYParser::Eq1Context *ctx) {
    return std::any_cast<BaseValuePtr>(ctx->relExp()->accept(this));
}

std::any AstVisitor::visitEq2(SysYParser::Eq2Context *ctx) {
    assert(ptr_or_not == NOTPTR);
    BaseValuePtr lhs = std::any_cast<BaseValuePtr>(ctx->eqExp()->accept(this));
    OpCode op = std::any_cast<OpCode>(ctx->eqOp()->accept(this));
    BaseValuePtr rhs = std::any_cast<BaseValuePtr>(ctx->relExp()->accept(this));
    return Value::BinaryOperate(op, lhs, rhs, cur_block);
}

std::any AstVisitor::visitEqOp(SysYParser::EqOpContext *ctx) {
    std::string op = ctx->getText();
    if (op == "==") {
        return OP_EQU;
    } else if (op == "!=") {
        return OP_NEQ;
    }
    assert(0);
}

std::any AstVisitor::visitLAnd1(SysYParser::LAnd1Context *ctx) {
    return Value::ScalarTypeConvert(BOOL, std::any_cast<BaseValuePtr>(ctx->eqExp()->accept(this)), cur_block);
}

std::any AstVisitor::visitLAnd2(SysYParser::LAnd2Context *ctx) {
    BaseValuePtr lAnd_node =
        Value::ScalarTypeConvert(BOOL, std::any_cast<BaseValuePtr>(ctx->lAndExp()->accept(this)), cur_block);
    CfgNodePtr lAnd_true = cur_func->CreateCfgNode();

    BranchInstPtr br_inst = BranchInst::CreatePtr(lAnd_node, lAnd_true, nullptr, cur_block);
    cur_block->InsertInst(br_inst);
    lAnd_list.push_back(br_inst);

    cur_block = lAnd_true;
    return Value::ScalarTypeConvert(BOOL, std::any_cast<BaseValuePtr>(ctx->eqExp()->accept(this)), cur_block);
}

std::any AstVisitor::visitLOr1(SysYParser::LOr1Context *ctx) {
    return Value::ScalarTypeConvert(BOOL, std::any_cast<BaseValuePtr>(ctx->lAndExp()->accept(this)), cur_block);
}

std::any AstVisitor::visitLOr2(SysYParser::LOr2Context *ctx) {
    BranchInstList last_lAnd_list = lAnd_list;
    lAnd_list = BranchInstList();

    BaseValuePtr lOr_node = std::any_cast<BaseValuePtr>(ctx->lOrExp()->accept(this));
    CfgNodePtr lOr_false = cur_func->CreateCfgNode();

    for (auto &&lAnd_inst : lAnd_list) {
        lAnd_inst->setFalseTarget(lOr_false);
    }
    lAnd_list = last_lAnd_list;

    BranchInstPtr br_inst = BranchInst::CreatePtr(lOr_node, nullptr, lOr_false, cur_block);
    cur_block->InsertInst(br_inst);
    lOr_list.push_back(br_inst);

    cur_block = lOr_false;
    return Value::ScalarTypeConvert(BOOL, std::any_cast<BaseValuePtr>(ctx->lAndExp()->accept(this)), cur_block);
}

std::any AstVisitor::visitCondExp(SysYParser::CondExpContext *ctx) {
    ptr_or_not = NOTPTR;
    return Value::ScalarTypeConvert(BOOL, std::any_cast<BaseValuePtr>(ctx->lOrExp()->accept(this)), cur_block);
}

std::any AstVisitor::visitConstExp(SysYParser::ConstExpContext *ctx) {
    ptr_or_not = NOTPTR;
    BaseValuePtr constant = std::any_cast<BaseValuePtr>(std::any_cast<BaseValuePtr>(ctx->addExp()->accept(this)));
    assert(constant->IsConstant());
    return constant;
}

ArrDims AstVisitor::getArrayDims(std::vector<SysYParser::ConstExpContext *> &constExpVec) {
    ArrDims arr_dims;
    for (auto &&const_exp : constExpVec) {
        BaseValuePtr value = std::any_cast<BaseValuePtr>(const_exp->accept(this));
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
        auto &&sym_table = itre_table->GetNameValueMap();
        if (sym_table.find(name) != sym_table.end()) {
            return sym_table[name];
        }
        itre_table = itre_table->GetParentTable();
    }
    assert(0);
}

SymbolTable *AstVisitor::initParamList(CfgNodePtr first_block, SymbolTable *parent,
                                       std::vector<std::string> param_name) {
    SymbolTable *new_table = newLocalTable(parent);

    size_t size = param_name.size();
    auto &&param_list = cur_func->GetParamList();
    for (size_t idx = 0; idx < size; ++idx) {
        auto &&name = param_name[idx];
        auto &&param = param_list[idx];
        if (param->getBaseType()->IsPointer()) {
            new_table->InsertSymbol(name, param);
        } else {
            ATTR_TYPE _type = param->getBaseType()->getAttrType();
            BaseTypePtr ty_stored = (_type == INT) ? type_int_L : type_float_L;
            BaseTypePtr ty_alloca = (_type == INT) ? type_int_ptr_L : type_float_ptr_L;
            BaseValuePtr addr_alloca = AllocaInst::DoAllocaAddr(ty_stored, ty_alloca, first_block);
            StoreInst::DoStoreValue(addr_alloca, param, first_block);
            new_table->InsertSymbol(name, addr_alloca);
        }
    }
    return new_table;
}

void AstVisitor::parseLocalListInit(SysYParser::ListInitvalContext *ctx, ListTypePtr list_type, BaseValuePtr base_addr,
                                    CfgNodePtr cur_block) {
    ATTR_TYPE _type = list_type->getAttrType();

    ConstantPtr zero = (_type == INT) ? zero_int32 : zero_float;
    ArrDims dim_size = list_type->getDimSize();

    std::function<void(SysYParser::ListInitvalContext *, const ArrDims &, int32_t, size_t)> function =
        [&](SysYParser::ListInitvalContext *node, const ArrDims &arr_dims, size_t idx_offset, size_t level) {
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
                    BaseValuePtr store_addr =
                        GetElementPtrInst::DoGetPointer(list_type, base_addr, off_list, cur_block);
                    BaseValuePtr value = Value::ScalarTypeConvert(
                        _type, std::any_cast<BaseValuePtr>(scalar_node->exp()->accept(this)), cur_block);
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
