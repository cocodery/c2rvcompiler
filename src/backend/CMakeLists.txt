project(c2rvc)

cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

aux_source_directory(${CMAKE_SOURCE_DIR} CSRC)

add_executable(${PROJECT_NAME} ${CSRC})