#include "OsiFunctionData.h"

uint32_t g_unknown1;

COsiFunctionDef::COsiFunctionDef(COsiSmartBuf &buf)
    : m_name(nullptr) {
    OsiCheckError(buf.ReadEncString(&m_name));
    m_parameterMask = COsiOutParameterMask(buf);

    m_parameterTypes = new COsiValueTypeList;
    m_parameterTypes->Read(buf);
}

COsiFunctionDef::~COsiFunctionDef() {
    free(m_name);
}

COsiFunctionData::COsiFunctionData(COsiSmartBuf &buf)
    : m_unk14(0),
      m_unk18(nullptr) {
    OsiCheckError(buf.ReadUint32(&m_unk4));
    OsiCheckError(buf.ReadUint32(&m_unk8));
    OsiCheckError(buf.ReadUint32(&m_unkc));
    OsiCheckError(buf.ReadUint32(&m_unk14));
    m_unk18 = &g_unknown1;
    OsiCheckError(buf.ReadUint8(&m_unk1c));
    OsiCheckError(m_keyList.Read(buf));
    m_pFunctionDef = new COsiFunctionDef(buf);
}

COsiFunctionData::~COsiFunctionData() {
    delete m_pFunctionDef;
}

COsiSearchIndex<COsiFunctionData *> g_OsiFunctions;
