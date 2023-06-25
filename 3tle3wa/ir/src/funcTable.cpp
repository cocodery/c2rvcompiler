#include "3tle3wa/ir/funcTable.hh"

FunctionTable::FunctionTable() {
    // void, i32, float
    ScalarTypePtr ret_type[3] = {type_void, type_int_L, type_float_L};

    VariablePtr bool_param = Variable::CreatePtr(param_bool, nullptr);
    VariablePtr char_param = Variable::CreatePtr(param_char, nullptr);
    VariablePtr charp_param = Variable::CreatePtr(param_charp, nullptr);
    VariablePtr i32_param = Variable::CreatePtr(param_int, nullptr);
    VariablePtr i32p_param = Variable::CreatePtr(param_intp, nullptr);
    VariablePtr float_param = Variable::CreatePtr(param_float, nullptr);
    VariablePtr floatp_param = Variable::CreatePtr(param_floatp, nullptr);
    VariablePtr longlong_param = Variable::CreatePtr(param_longlong, nullptr);
    // 0 - no parameters
    ParamList param_list[8];
    // 1 - i32
    param_list[1].push_back(i32_param);
    // 2 - float
    param_list[2].push_back(float_param);
    // 3 - i32*
    param_list[3].push_back(i32p_param);
    // 4 - float*
    param_list[4].push_back(floatp_param);
    // 5 - i32, i32 *
    param_list[5].push_back(i32_param);
    param_list[5].push_back(i32p_param);
    // 6 - i32, float *
    param_list[6].push_back(i32_param);
    param_list[6].push_back(floatp_param);
    // 7 - i8 *, i8, i64, i1
    param_list[7].push_back(charp_param);
    param_list[7].push_back(char_param);
    param_list[7].push_back(longlong_param);
    param_list[7].push_back(bool_param);

    // 0 - getint | 1 - getch | 2 - getfloat | 3 - getarray | 4 - getfarray
    // 5 - putint | 6 - putch | 7 - putfloat | 8 - putarray | 9 - putfarray
    // 10 - _sysy_starttime
    // 11 - _sysy_stoptime
    // 12 - llvm.memset.p0i8.i64
    LibFuncPtr lib_func[13];
    lib_func[0] = LibraryFunction::CreatePtr(ret_type[1], "getint", param_list[0]);
    lib_func[1] = LibraryFunction::CreatePtr(ret_type[1], "getch", param_list[0]);
    lib_func[2] = LibraryFunction::CreatePtr(ret_type[2], "getfloat", param_list[0]);
    lib_func[3] = LibraryFunction::CreatePtr(ret_type[1], "getarray", param_list[3]);
    lib_func[4] = LibraryFunction::CreatePtr(ret_type[1], "getfarray", param_list[4]);
    lib_func[5] = LibraryFunction::CreatePtr(ret_type[0], "putint", param_list[1]);
    lib_func[6] = LibraryFunction::CreatePtr(ret_type[0], "putch", param_list[1]);
    lib_func[7] = LibraryFunction::CreatePtr(ret_type[0], "putfloat", param_list[2]);
    lib_func[8] = LibraryFunction::CreatePtr(ret_type[0], "putarray", param_list[5]);
    lib_func[9] = LibraryFunction::CreatePtr(ret_type[0], "putfarray", param_list[6]);
    lib_func[10] = LibraryFunction::CreatePtr(ret_type[0], "_sysy_starttime", param_list[1]);
    lib_func[11] = LibraryFunction::CreatePtr(ret_type[0], "_sysy_stoptime", param_list[1]);
    lib_func[12] = LibraryFunction::CreatePtr(ret_type[0], "llvm.memset.p0i8.i64", param_list[7]);

    for (auto &&func_ptr : lib_func) {
        libraryFuncTable.push_back(func_ptr);
    }
}

BaseFuncPtr FunctionTable::GetFunction(std::string &name) {
    for (auto &&func : normalFuncTable) {
        if (name == func->GetFuncName()) {
            return func;
        }
    }
    for (auto &&func : libraryFuncTable) {
        if (name == func->GetFuncName()) {
            return func;
        }
    }
    assert(0);
}

void FunctionTable::InsertFunction(NormalFuncPtr func_ptr) { normalFuncTable.push_back(func_ptr); }

NormalFuncList &FunctionTable::GetNormalFuncTable() { return normalFuncTable; }

std::ostream &operator<<(std::ostream &os, FunctionTable _func_table) {
    for (auto &&normal_func : _func_table.normalFuncTable) {
        if (normal_func->IsBeUsed()) os << std::static_pointer_cast<BaseFunction>(normal_func) << endl;
    }
    for (auto &&lib_func : _func_table.libraryFuncTable) {
        if (lib_func->IsBeUsed()) os << std::static_pointer_cast<BaseFunction>(lib_func) << endl;
    }
    return os;
}