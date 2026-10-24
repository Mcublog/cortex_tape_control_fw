cmake_minimum_required(VERSION 3.22)

project(mech_test LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(mech_test
    mech_test.cpp
    ../../app/mech/crp42602y.cpp
    ../../libs/debug/log_libs.c
)

target_include_directories(mech_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../libs
)
