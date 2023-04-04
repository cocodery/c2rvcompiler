#include "funcTable.hh"

FunctionTable::FunctionTable() {
    libfunc_table.reserve(12);

    ScalarTypePtr ret_type[3];
    ret_type[0] = ScalarType::CreatePtr(VOID);  // void
    ret_type[1] = ScalarType::CreatePtr(INT);   // i32
    ret_type[2] = ScalarType::CreatePtr(FLOAT); // float


    VariablePtr i32_param = Variable::CreatePtr(ScalarType::CreatePtr(INT | VARIABLE));
    VariablePtr float_param = Variable::CreatePtr(ScalarType::CreatePtr(FLOAT | VARIABLE));
    VariablePtr i32p_param = Variable::CreatePtr(ScalarType::CreatePtr(INT | VARIABLE | POINTER));
    VariablePtr floatp_param = Variable::CreatePtr(ScalarType::CreatePtr(FLOAT | VARIABLE | POINTER));
    // 0 - no parameters
    ParamList param_list[7];
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

    // 0 - getint | 1 - getch | 2 - getfloat | 3 - getarray | 4 - getfarray
    // 5 - putint | 6 - putch | 7 - putfloat | 8 - putarray | 9 - putfarray
    // 10 - _sysy_starttime
    // 11 - _sysy_stoptime
    LibFuncPtr lib_func[12];
    lib_func[0]  = LibraryFunction::CreatePtr(ret_type[1], "getint"         , param_list[0]);
    lib_func[1]  = LibraryFunction::CreatePtr(ret_type[1], "getch"          , param_list[0]);
    lib_func[2]  = LibraryFunction::CreatePtr(ret_type[2], "getfloat"       , param_list[0]);
    lib_func[3]  = LibraryFunction::CreatePtr(ret_type[1], "getarray"       , param_list[3]);
    lib_func[4]  = LibraryFunction::CreatePtr(ret_type[1], "getfarray"      , param_list[4]);
    lib_func[5]  = LibraryFunction::CreatePtr(ret_type[0], "putint"         , param_list[1]);
    lib_func[6]  = LibraryFunction::CreatePtr(ret_type[0], "putch"          , param_list[1]);
    lib_func[7]  = LibraryFunction::CreatePtr(ret_type[0], "putfloat"       , param_list[2]);
    lib_func[8]  = LibraryFunction::CreatePtr(ret_type[0], "putarray"       , param_list[5]);
    lib_func[9]  = LibraryFunction::CreatePtr(ret_type[0], "putfarray"      , param_list[6]);
    lib_func[10] = LibraryFunction::CreatePtr(ret_type[0], "_sysy_starttime", param_list[1]);
    lib_func[11] = LibraryFunction::CreatePtr(ret_type[0], "_sysy_stoptime" , param_list[1]);
    
    for (auto &&func_ptr : lib_func) {
        libfunc_table.push_back(func_ptr);
    }
}

BaseFuncPtr FunctionTable::getFunction(std::string &name) {
    for (auto &&usr_func : usrfunc_table) {
        if (name == usr_func->getFuncName()) {
            return usr_func;
        }
    }
    for (auto &&lib_func : libfunc_table) {
        if (name == lib_func->getFuncName()) {
            return lib_func;
        }
    }
    assert(false);
}

void FunctionTable::insertFunction(BaseFuncPtr func_ptr) {
    usrfunc_table.push_back(func_ptr);
}

std::ostream &operator<<(std::ostream &os, FunctionTable func_table) {
    auto &&usrfunc_table = func_table.usrfunc_table;
    auto &&libfunc_talbe = func_table.libfunc_table;

    for (auto &&usr_func : usrfunc_table) {
        os << usr_func << endl;
    }
    for (auto &&lib_func : libfunc_talbe) {
        os << lib_func << endl;
    }

    return os;
}