#pragma once

#include "FunctionParseData.h"
#include "OsirisFuncs.h"
#include "OsiSmartBuf.h"

class __declspec(dllexport) COsiris {
    CpOsiFunctionParseDataList *m_globalActionList;
    void *unknown04;

public:
    COsiris();
    ~COsiris();

    void ClearGlobalActionList();
    void CloseLogFile();
    void RegisterDIVFunctions(TOsirisInitFunction *funcs);
    bool OpenLogFile(const char *FileName, const char *Mode);

private:
    bool _ReadHeader(
        COsiSmartBuf &buf,
        uint8_t &versionMajor,
        uint8_t &versionMinor,
        uint8_t &versionPatch,
        uint8_t &versionBuild,
        char (*storyVersion)[128],
        uint32_t &debugFlags);

    bool _ReadDIVObjects(COsiSmartBuf &buf);
    bool _ReadFunctions(COsiSmartBuf &buf);
    bool _ReadReteNodes(COsiSmartBuf &buf);
};

extern COsiris Osiris;
