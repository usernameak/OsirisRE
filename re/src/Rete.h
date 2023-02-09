#pragma once

#include "Factory.h"
#include "OsiFunctionData.h"
#include "ReteDBase.h"

class CReteNode;

// TODO: seems like it should be something like a unique_ptr... in C++98?
//  might be auto_ptr, but we'll use something more like unique_ptr later
//  anyway; it compiles into the same bytecode
typedef CReteNode *PCReteNode;

class CReteNodeFactory : public CReteFactory<CReteNodeFactory, PCReteNode> {};
extern CReteNodeFactory g_ReteNodeFactory;

class CReteNode : public CFactoryProduct<CReteNodeFactory, PCReteNode> {
    COsiFunctionData *m_ownerFunc;
    CReteDBaseRef m_dbRef; // TODO:
public:
    CReteNode(COsiSmartBuf &buf);

    virtual ~CReteNode() = default;
};

class CReteConnection {
    uint32_t m_unk04;
    CReteNodeFactory *m_nodeFactory;
    uint32_t m_unk0C;
    uint32_t m_unk10;
public:
    CReteConnection(COsiSmartBuf &buf);

    virtual bool Write(COsiSmartBuf &buf) const;
    virtual bool Read(COsiSmartBuf &buf);
};

class CReteStartNode : public CReteNode {
    std::list<CReteConnection> m_connections;

public:
    CReteStartNode(COsiSmartBuf &buf);

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
