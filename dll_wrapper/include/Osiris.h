#pragma once

#include "OsiSmartBuf.h"
#include "OsirisFuncs.h"
#include <cstdint>
#include <Windows.h>

class CpOsiFunctionParseDataList;
class COsiArgumentDesc;

class __declspec(dllimport) COsiris {
    CpOsiFunctionParseDataList *m_globalActionList;
    void *unknown04;

public:
    COsiris();
    ~COsiris();
    COsiris &operator=(COsiris const &);

    bool ApplyPatchFile(const char *filename, const char *mode);
    void CheckOrphanFunctions();

    void ClearGlobalActionList();

    void CloseLogFile();

    bool Compile(const char *filename, const char *mode);

    void DeleteAllData();

    bool Event(unsigned int eventHandle, COsiArgumentDesc *arguments);

    void GenerateFunctionList(); // writes to log file
    void GenerateNodeList(); // writes to log file

    bool GetFileStoryVersion(const char *filename, DWORD *version);
    bool GetFileStoryVersionString(const char *filename, char *version);

    bool GetStoryVersion(DWORD *version);
    bool GetStoryVersionString(char *version);

    bool InitGame();

    bool Load(const char *filename);
    bool Merge();

    void Minilog_Clear();
    bool Minilog_Create(DWORD size);
    DWORD Minilog_Expand(unsigned char *, DWORD,char *, DWORD); // wtf
    bool Minilog_GetBufferData(unsigned char * *,unsigned char * *,unsigned long *);

    bool NoStoryLoaded();

    bool OpenLogFile(const char *FileName, const char *Mode);

    bool PrepareMerge();

    void RegisterDIVFunctions(TOsirisInitFunction *funcs);

    bool Save(const char *filename);

    void SetOption(unsigned int option);
private:
    /* let's not include private functions here to avoid some funky shit */
};

extern __declspec(dllimport) COsiris Osiris;
