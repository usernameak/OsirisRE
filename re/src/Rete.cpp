#include "Rete.h"

CReteNodeFactory g_ReteNodeFactory;

CReteConnection::CReteConnection(COsiSmartBuf &buf)
    : m_unk04(0),
      m_nodeFactory(nullptr) {

    OsiCheckError(Read(buf));
}

bool CReteConnection::Write(COsiSmartBuf &buf) const {
    if (!buf.WriteUint32(m_unk04)) return false;
    if (!buf.WriteUint32(m_unk0C)) return false;
    if (!buf.WriteUint32(m_unk10)) return false;

    return true;
}

bool CReteConnection::Read(COsiSmartBuf &buf) {
    // TODO: unk04 and factory might need to be moved outside this class
    uint32_t unk04_tmp;
    if (!buf.ReadUint32(&unk04_tmp)) {
        return false;
    }

    m_nodeFactory = &g_ReteNodeFactory;
    m_unk04 = unk04_tmp;

    if (!buf.ReadUint32(&m_unk0C)) {
        return false;
    }

    if (!buf.ReadUint32(&m_unk10)) {
        return false;
    }

    return false;
}

// === //

CReteNode::CReteNode(COsiSmartBuf &buf) {
    OsiCheckError(m_dbRef.Read(buf));

    unsigned char name[1024];
    OsiCheckError(buf.ReadEncString(name));

    if (name[0]) {
        uint8_t numArgs;
        OsiCheckError(buf.ReadUint8(&numArgs));

        COsiHashedString key(name, numArgs);
        m_ownerFunc = g_OsiFunctions.lookupPtr(key);
    } else {
        m_ownerFunc = nullptr;
    }
}

// === //

CReteStartNode::CReteStartNode(COsiSmartBuf &buf) : CReteNode(buf) {
    uint32_t numConnections;
    OsiCheckError(buf.ReadUint32(&numConnections));

    for (uint32_t i = 0; i < numConnections; i++) {
        if (g_OsiHadError) return;

        m_connections.emplace_back(buf);
    }
}
