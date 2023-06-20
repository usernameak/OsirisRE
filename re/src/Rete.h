#pragma once

#include "Factory.h"
#include "ReteDBase.h"
#include "OsiTypedValue.h"

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
    CReteConnection m_next;

public:
    explicit CReteChildNode(COsiSmartBuf &buf);
};

class CReteBinaryNode : public CReteChildNode {
    CReteNodeRef m_leftParentRef;
    CReteNodeRef m_rightParentRef;
    CReteAdaptorRef m_leftAdaptorRef;
    CReteAdaptorRef m_rightAdaptorRef;
    uint32_t m_unk4C;
    CReteNodeRef m_leftFactRef;
    CReteConnection m_left;
    uint8_t m_leftIndirection;
    CReteNodeRef m_rightFactRef;
    CReteConnection m_right;
    uint8_t m_rightIndirection;

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

struct CReteUnaryNode : public CReteChildNode {
    CReteNodeRef m_parent;
    CReteAdaptorRef m_adaptor;
    CReteNodeRef m_fact;
    CReteConnection m_join;
    uint8_t m_indirection;

public:
    explicit CReteUnaryNode(COsiSmartBuf &buf);
};

struct CReteRelCondition : public CReteUnaryNode {
    uint8_t m_leftIdx;
    uint8_t m_rightIdx;
    COsiTypedValue m_left;
    COsiTypedValue m_right;
    uint32_t m_type;

public:
    explicit CReteRelCondition(COsiSmartBuf &buf);
};
