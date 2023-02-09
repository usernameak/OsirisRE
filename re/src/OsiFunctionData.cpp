#include "OsiFunctionData.h"

#include "Rete.h"

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
      m_nodeFactory(nullptr) {
    OsiCheckError(buf.ReadUint32(&m_unk4));
    OsiCheckError(buf.ReadUint32(&m_unk8));
    OsiCheckError(buf.ReadUint32(&m_unkc));
    OsiCheckError(buf.ReadUint32(&m_unk14));
    m_nodeFactory = &g_ReteNodeFactory;
    OsiCheckError(buf.ReadUint8(&m_unk1c));
    OsiCheckError(m_keyList.Read(buf));
    m_pFunctionDef = new COsiFunctionDef(buf);
}

COsiFunctionData::~COsiFunctionData() {
    delete m_pFunctionDef;
}

COsiSearchIndex<COsiFunctionData *> g_OsiFunctions;
