cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)

add_library(osi_extender SHARED main.cpp cpptoml.h)
target_link_libraries(osi_extender version Osiris)
set_target_properties(osi_extender PROPERTIES SUFFIX ".asi")
