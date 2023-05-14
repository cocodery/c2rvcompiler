cmake_minimum_required(VERSION 3.16)

project(compiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

file(GLOB_RECURSE CXXSRC "src/*.cpp" "src/*.cc" "src/*.cxx")

# 链接 antlr4 cpp runtime
# 设置寻找路径 official path
set(ANTLR4_LIB_DIR /usr/local/lib)
set(ANTLR4_INC_DIR /usr/local/include/antlr4.12-runtime)

set(ANTLR4_CXX_RUNTIME libantlr4-runtime.a)

if (EXISTS ${ANTLR4_LIB_DIR}/${ANTLR4_CXX_RUNTIME})
  link_libraries(${ANTLR4_LIB_DIR}/${ANTLR4_CXX_RUNTIME})
else()
  message("Can't find " ${ANTLR4_LIB_DIR}/${ANTLR4_CXX_RUNTIME})
  # set ur path to runtime
  set(ANTLR4_LIB_DIR /home/blur/code/antlr4-cpp-runtime-4.12.0/build/runtime)
  if (EXISTS ${ANTLR4_LIB_DIR}/${ANTLR4_CXX_RUNTIME})
    link_libraries(${ANTLR4_LIB_DIR}/${ANTLR4_CXX_RUNTIME})
    message("Using " ${ANTLR4_LIB_DIR}/${ANTLR4_CXX_RUNTIME})
  else()
    message(FATAL_ERROR "Can't find " ${ANTLR4_CXX_RUNTIME})
  endif()
endif()

include_directories(/usr/local/include/antlr4.12-runtime)
if (EXISTS ${ANTLR4_INC_DIR})
  include_directories(${ANTLR4_INC_DIR})
else()
  message("Can't find " ${ANTLR4_INC_DIR})
  # set ur path to includes
  set(ANTLR4_INC_DIR /home/blur/code/antlr4-cpp-runtime-4.12.0/runtime/src)
  if (EXISTS ${ANTLR4_INC_DIR})
    include_directories(${ANTLR4_INC_DIR})
    message("Using " ${ANTLR4_INC_DIR})
  else()
    message(FATAL_ERROR "Can't find " ${ANTLR4_INC_DIR})
  endif()
endif()


# 设置编译参数
add_compile_options(-std=c++17 -fdiagnostics-color=always)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-g -Og)
  add_compile_options(-Wall -Werror -Wextra -Wno-unused-parameter)
  add_compile_definitions(DEBUG_MODE)
  if (DEFINED ASAN)
    message("Using address sanitize")
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
  endif()
else()
  add_compile_options(-O2)
  add_compile_definitions(NDEBUG)
endif()

message("Build Type: " ${CMAKE_BUILD_TYPE})

# 包含头文件
include_directories(src)
include_directories(src/antlr)
include_directories(src/frontend)
include_directories(src/backend)
include_directories(src/debug)
include_directories(src/ir)
include_directories(src/ir/function)
include_directories(src/ir/instruction)
include_directories(src/ir/value/type)
include_directories(src/ir/value)
include_directories(src/pass)
include_directories(src/pass/analysis)
include_directories(src/pass/analysis/dominance)
include_directories(src/pass/interprocedural)
include_directories(src/pass/interprocedural/dce)
include_directories(src/pass/interprocedural/gvngcm)
include_directories(src/pass/interprocedural/dominance)
include_directories(src/pass/interprocedural/ssa)
include_directories(src/pass/intraprocedural)
include_directories(src/pass/intraprocedural/inline)

# 编译程序
add_executable(${PROJECT_NAME} ${CXXSRC})