#pragma once

#include "Factory.h"
#include "ReteDBase.h"

class CReteNode;
class CReteDBase;
class CReteAdaptor;

class COsiFunctionData;

// === ADAPTORS === //

// TODO: seems like it should be something like a unique_ptr... in C++98?
//  might be auto_ptr, but we'll use something more like unique_ptr later
//  anyway; it compiles into the same bytecode
typedef CReteAdaptor *PCReteAdaptor;

class CReteAdaptorFactory : public CReteFactory<CReteAdaptorFactory, PCReteAdaptor> {
public:
    static CReteAdaptorFactory instance;
};

class CReteAdaptor;

typedef CFactoryProductRef<CReteAdaptorFactory, PCReteAdaptor> CReteAdaptorRef;

// === NODES === //

// TODO: seems like it should be something like a unique_ptr... in C++98?
//  might be auto_ptr, but we'll use something more like unique_ptr later
//  anyway; it compiles into the same bytecode
typedef CReteNode *PCReteNode;

class CReteNodeFactory : public CReteFactory<CReteNodeFactory, PCReteNode> {
public:
    static CReteNodeFactory instance;
};

class CReteNode : public CFactoryProduct<CReteNodeFactory, PCReteNode> {
    COsiFunctionData *m_ownerFunc;
    CReteDBaseRef m_dbRef;
public:
    explicit CReteNode(COsiSmartBuf &buf);

    virtual ~CReteNode() = default;
};

typedef CFactoryProductRef<CReteNodeFactory, PCReteNode> CReteNodeRef;

class CReteConnection {
    CReteNodeRef m_nodeRef;
    uint32_t m_unk0C;
    uint32_t m_unk10;
public:
    CReteConnection();
    explicit CReteConnection(COsiSmartBuf &buf);

    virtual bool Write(COsiSmartBuf &buf) const;
    virtual bool Read(COsiSmartBuf &buf);
};

class CReteStartNode : public CReteNode {
    std::list<CReteConnection> m_connections;

public:
    explicit CReteStartNode(COsiSmartBuf &buf);

    virtual ~CReteStartNode() override = default;
};

class CReteFact : public CReteStartNode {
public:
    using CReteStartNode::CReteStartNode;
};

class CReteEvent : public CReteStartNode {
public:
    using CReteStartNode::CReteStartNode;
};

class CReteQuery : public CReteNode {
public:
    using CReteNode::CReteNode;
};

class CReteDIVQuery : public CReteQuery {
public:
    using CReteQuery::CReteQuery;
};

class CReteInternalQuery : public CReteQuery {
public:
    using CReteQuery::CReteQuery;
};

class CReteChildNode : public CReteNode {
    CReteConnection m_parent;

public:
    explicit CReteChildNode(COsiSmartBuf &buf);
};

class CReteBinaryNode : public CReteChildNode {
    CReteNodeRef m_unk2C;
    CReteNodeRef m_unk34;
    CReteAdaptorRef m_unk3C;
    CReteAdaptorRef m_unk44;
    uint32_t m_unk4C;
    CReteNodeRef m_unk50;
    CReteConnection m_left;
    uint8_t m_unk6C;
    CReteNodeRef m_unk70;
    CReteConnection m_right;
    uint8_t m_unk8C;

public:
    explicit CReteBinaryNode(COsiSmartBuf &buf);
};

class CReteAnd : public CReteBinaryNode {
public:
    using CReteBinaryNode::CReteBinaryNode;
};

class CReteNAnd : public CReteBinaryNode {
public:
    using CReteBinaryNode::CReteBinaryNode;
};
