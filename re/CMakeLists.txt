cmake_minimum_required(VERSION 3.20)

add_library(Osiris SHARED

        src/Osiris.cpp
        src/Osiris.h

        src/FunctionParseData.cpp
        src/FunctionParseData.h

        src/List.h
        src/OsirisFuncs.cpp
        src/OsirisFuncs.h
        src/OsiSmartBuf.h
        src/OsiCommon.cpp
        src/OsiCommon.h
        src/OsirisLogging.h
        src/OsirisLogging.cpp
        src/SearchIndex.h
        src/OsiDIVObjectData.cpp
        src/OsiDIVObjectData.h
        src/OsiFunctionData.cpp
        src/OsiFunctionData.h
        src/KeyList.h
        src/Factory.h src/Rete.cpp src/Rete.h src/ReteDBase.cpp src/ReteDBase.h)

target_compile_definitions(Osiris PRIVATE _CRT_SECURE_NO_WARNINGS)

# target_compile_options(Osiris PRIVATE /d1reportAllClassLayout)