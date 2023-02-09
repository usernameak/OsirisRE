#pragma once

#include "Factory.h"
#include "OsiFunctionData.h"

class CReteDBase;

// TODO: seems like it should be something like a unique_ptr... in C++98?
//  might be auto_ptr, but we'll use something more like unique_ptr later
//  anyway; it compiles into the same bytecode
typedef CReteDBase *PCReteDBase;

class CReteDBaseFactory : public CReteFactory<CReteDBase, PCReteDBase> {};
extern CReteDBaseFactory g_ReteDBaseFactory;

class CReteDBase : public CFactoryProduct<CReteDBaseFactory, PCReteDBase> {
    // TODO:
};

class CReteDBaseRef {
    uint32_t m_id;
    CReteDBaseFactory *m_factory;
public:
    CReteDBaseRef() : m_id(0), m_factory(nullptr) {}

    bool Read(COsiSmartBuf &buf) {
        m_factory = &g_ReteDBaseFactory;
        return buf.ReadUint32(&m_id);
    }
};


