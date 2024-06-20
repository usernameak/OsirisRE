#pragma once

struct TOsirisInitFunction {
    uint32_t (*getDIVHandle)(uint8_t a, uint16_t b, uint16_t c);
    bool (*invokeCall)(uint32_t handle, void *argsdata);
    bool (*invokeQuery)(uint32_t handle, void *argsdata);
    void (*osirisError)(const char *msg);
};

class COsirisFuncs {
    uint32_t (*m_getDIVHandle)(uint8_t a, uint16_t b, uint16_t c);
    bool (*m_invokeCall)(uint32_t handle, void *argsdata);
    bool (*m_invokeQuery)(uint32_t handle, void *argsdata);
    void (*m_osirisError)(const char *msg);

public:
    COsirisFuncs() = default;

    COsirisFuncs(TOsirisInitFunction *initFunc)
        : m_getDIVHandle(initFunc->getDIVHandle),
          m_invokeCall(initFunc->invokeCall),
          m_invokeQuery(initFunc->invokeQuery),
          m_osirisError(initFunc->osirisError) {
    }

    void Error(const char *message) {
        m_osirisError(message);
    }
};

extern COsirisFuncs g_OsiDIVFunctions;
