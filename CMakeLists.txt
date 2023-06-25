cmake_minimum_required(VERSION 3.16)

project(compiler)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

file(GLOB_RECURSE CXXSRC
  ${CMAKE_SOURCE_DIR}/src/*.cpp)

# 链接 antlr4 cpp runtime
# 设置寻找路径 official path
set(ANTLR4_LIB_DIR /usr/local/lib)
set(ANTLR4_INC_DIR /usr/local/include/antlr4.12-runtime)

set(ANTLR4_CXX_RUNTIME libantlr4-runtime.a)

if(EXISTS ${ANTLR4_LIB_DIR}/${ANTLR4_CXX_RUNTIME})
  link_libraries(${ANTLR4_LIB_DIR}/${ANTLR4_CXX_RUNTIME})
else()
  message("Can't find " ${ANTLR4_LIB_DIR}/${ANTLR4_CXX_RUNTIME})

  # 下面参数用于设置 antlr4 cpp runtime 的位置
  set(ANTLR4_LIB_DIR ${CMAKE_SOURCE_DIR}/ext/antlr4-cpp-runtime-4.12.0/build/runtime)

  if(EXISTS ${ANTLR4_LIB_DIR}/${ANTLR4_CXX_RUNTIME})
    link_libraries(${ANTLR4_LIB_DIR}/${ANTLR4_CXX_RUNTIME})
    message("Using " ${ANTLR4_LIB_DIR}/${ANTLR4_CXX_RUNTIME})
  else()
    message(FATAL_ERROR "Can't find " ${ANTLR4_CXX_RUNTIME})
  endif()
endif()

if(EXISTS ${ANTLR4_INC_DIR})
  include_directories(${ANTLR4_INC_DIR})
else()
  message("Can't find " ${ANTLR4_INC_DIR})

  # 下面参数用于设置 antlr4 cpp 头文件的位置
  set(ANTLR4_INC_DIR ${CMAKE_SOURCE_DIR}/ext/antlr4-cpp-runtime-4.12.0/runtime/src)

  if(EXISTS ${ANTLR4_INC_DIR})
    include_directories(${ANTLR4_INC_DIR})
    message("Using " ${ANTLR4_INC_DIR})
  else()
    message(FATAL_ERROR "Can't find " ${ANTLR4_INC_DIR})
  endif()
endif()

# 设置编译参数
add_compile_options(-std=c++17)
add_compile_options(-O2)
add_compile_options(-Wall -Wextra -Wno-unused-parameter)

# 包含头文件
include_directories(src/include)
include_directories(src/antlr)

# 编译程序
add_executable(${PROJECT_NAME} ${CXXSRC})

target_link_libraries(${PROJECT_NAME} m)