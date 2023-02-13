#include "OsiFunctionData.h"

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

COsiFunctionData::COsiFunctionData(COsiSmartBuf &buf) {
    OsiCheckError(buf.ReadUint32(&m_unk4));
    OsiCheckError(buf.ReadUint32(&m_unk8));
    OsiCheckError(buf.ReadUint32(&m_unkc));
    OsiCheckError(m_nodeRef.Read(buf));
    OsiCheckError(buf.ReadUint8(&m_unk1c));
    OsiCheckError(m_keyList.Read(buf));
    m_pFunctionDef = new COsiFunctionDef(buf);
}

COsiFunctionData::~COsiFunctionData() {
    delete m_pFunctionDef;
}

COsiSearchIndex<COsiFunctionData *> g_OsiFunctions;
