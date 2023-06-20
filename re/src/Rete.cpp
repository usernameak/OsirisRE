#include "Rete.h"

CReteNodeFactory CReteNodeFactory::instance;

CReteConnection::CReteConnection() {}

CReteConnection::CReteConnection(COsiSmartBuf &buf) {
    OsiCheckError(Read(buf));
}

bool CReteConnection::Write(COsiSmartBuf &buf) const {
    if (!m_nodeRef.Write(buf)) return false;
    if (!buf.WriteUint32(m_unk0C)) return false;
    if (!buf.WriteUint32(m_unk10)) return false;

    return true;
}

bool CReteConnection::Read(COsiSmartBuf &buf) {
    if (!m_nodeRef.Read(buf)) return false;

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

// === //

CReteChildNode::CReteChildNode(COsiSmartBuf &buf) : CReteNode(buf) {
    OsiCheckError(m_next.Read(buf));
}

// === //
CReteBinaryNode::CReteBinaryNode(COsiSmartBuf &buf)
    : CReteChildNode(buf),
      m_unk4C(0) {
    OsiCheckError(m_leftParentRef.Read(buf));
    OsiCheckError(m_rightParentRef.Read(buf));
    OsiCheckError(m_leftAdaptorRef.Read(buf));
    OsiCheckError(m_rightAdaptorRef.Read(buf));
    OsiCheckError(m_leftFactRef.Read(buf));
    OsiCheckError(m_left.Read(buf));
    OsiCheckError(buf.ReadUint8(&m_leftIndirection));
    OsiCheckError(m_rightFactRef.Read(buf));
    OsiCheckError(m_right.Read(buf));
    OsiCheckError(buf.ReadUint8(&m_rightIndirection));
}

// === //

CReteUnaryNode::CReteUnaryNode(COsiSmartBuf &buf) : CReteChildNode(buf) {
    OsiCheckError(m_parent.Read(buf));
    OsiCheckError(m_adaptor.Read(buf));
    OsiCheckError(m_fact.Read(buf));
    OsiCheckError(m_join.Read(buf));
    OsiCheckError(buf.ReadUint8(&m_indirection));
}

// === //

CReteRelCondition::CReteRelCondition(COsiSmartBuf &buf) : CReteUnaryNode(buf) {
    OsiCheckError(buf.ReadUint8(&m_leftIdx));
    OsiCheckError(buf.ReadUint8(&m_rightIdx));
    m_left = COsiTypedValue(buf);
    m_right = COsiTypedValue(buf);
    OsiCheckError(buf.ReadUint32(&m_type));
}
