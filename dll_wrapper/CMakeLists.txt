cmake_minimum_required(VERSION 3.17)

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/OsirisDLL.lib
        COMMAND lib.exe /machine:x86 /def:${CMAKE_CURRENT_SOURCE_DIR}/OsirisDLL.def /out:${CMAKE_CURRENT_BINARY_DIR}/OsirisDLL.lib
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/OsirisDLL.def
        VERBATIM)

add_custom_target(OsirisImplib
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/OsirisDLL.lib
)

add_library(Osiris SHARED IMPORTED GLOBAL
        include/Osiris.h
        include/OsiSmartBuf.h
        include/OsiCommon.h
        include/OsirisFuncs.h
)
set_property(TARGET Osiris PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include")
set_property(TARGET Osiris PROPERTY IMPORTED_IMPLIB "${CMAKE_CURRENT_BINARY_DIR}/OsirisDLL.lib")
add_dependencies(Osiris OsirisImplib)
