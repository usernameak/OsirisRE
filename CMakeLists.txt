cmake_minimum_required(VERSION 3.17)

project(Osiris)

option(USE_RE "Use RE library" OFF)

set(CMAKE_CXX_STANDARD 17)

if(USE_RE)
    add_subdirectory(re)
else()
    add_subdirectory(dll_wrapper)
    add_subdirectory(compiler_wrapper)
    add_subdirectory(osi_extender)
endif()
