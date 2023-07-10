#include "3tle3wa/utils/Logs.hh"

uint64_t compile_start_time__ = 0;

__attribute((constructor)) void before_main() { compile_start_time__ = clock(); }