#include "OsiDIVObjectData.h"

COsiDIVObjectData::COsiDIVObjectData(COsiSmartBuf &buf)
    : m_name(nullptr) {
    OsiCheckError(buf.ReadEncString(&m_name));
    OsiCheckError(buf.ReadUint8(&m_type));
    OsiCheckError(m_keyList.Read(buf));
}

COsiDIVObjectData::~COsiDIVObjectData() {
    free(m_name);
}

COsiSearchIndex<COsiDIVObjectData *> g_OsiDIVObjects;
