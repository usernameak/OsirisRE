#pragma once

struct TOsirisInitFunction {
    void (*func0)();
    void (*func4)();
    void (*func8)();
    void (*pfnLogMessage)(const char *msg);
};

class COsirisFuncs {
    void (*m_pfnFunc0)();
    void (*m_pfnFunc4)();
    void (*m_pfnFunc8)();
    void (*m_pfnLogMessage)(const char *msg);

public:
    COsirisFuncs() = default;

    COsirisFuncs(TOsirisInitFunction *initFunc)
        : m_pfnFunc0(initFunc->func0),
          m_pfnFunc4(initFunc->func4),
          m_pfnFunc8(initFunc->func8),
          m_pfnLogMessage(initFunc->pfnLogMessage) {
    }

    void LogMessage(const char *message) {
        m_pfnLogMessage(message);
    }
};

extern COsirisFuncs g_OsiDIVFunctions;
