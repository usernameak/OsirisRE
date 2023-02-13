#pragma once

#include <cstdint>
#include <vector>
#include "OsiSmartBuf.h"

enum TOsiReteNodeType : uint8_t {
    OSI_RETE_FACT           = 1,
    OSI_RETE_EVENT          = 2,
    OSI_RETE_DIV_QUERY      = 3,
    OSI_RETE_AND            = 4,
    OSI_RETE_NAND           = 5,
    OSI_RETE_REL_CONDITION  = 6,
    OSI_RETE_RULE           = 7,
    OSI_RETE_INTERNAL_QUERY = 8,
};

template <typename Self, typename T>
class CReteFactory;

template <typename Factory, typename T>
class CFactoryProductRef {
    uint32_t m_id;
    Factory *m_factory;

public:
    CFactoryProductRef() : m_id(0), m_factory(nullptr) {}

    CFactoryProductRef(uint32_t id, Factory *factory) : m_id(id), m_factory(factory) {}

    bool Read(COsiSmartBuf &buf) {
        m_factory = &Factory::instance;
        return buf.ReadUint32(&m_id);
    }

    bool Write(COsiSmartBuf &buf) const {
        return buf.WriteUint32(m_id);
    }
};

template <typename Self, typename T>
class CReteFactory {
    uint32_t m_size;
    std::vector<T> m_registeredList;

public:
    CReteFactory() : m_size(0) {
        // wtf Larian

        for (auto &item : m_registeredList) {
            item = nullptr;
        }
    }

    CFactoryProductRef<Self, T> Register(T item, uint32_t id) {
        while (id >= m_size) {
            m_registeredList.push_back(nullptr);
            ++m_size;
        }

        if (id - 1 < m_size) {
            if (m_registeredList[id]) {
                assert(false); // d:\develop_old\divinedivinity\osiris\source\Factory.h:158
            }

            m_registeredList[id] = item;
            item->m_id           = id;
            item->m_factory      = static_cast<Self *>(this);

            return { m_size, static_cast<Self *>(this) };
        } else {
            // unreachable, just seems to appear in original code

            m_registeredList.push_back(item);
            item->m_id      = ++m_size;
            item->m_factory = static_cast<Self *>(this);

            return { m_size, static_cast<Self *>(this) };
        }
    }

    template <typename ItemType>
    CFactoryProductRef<Self, T> Produce(COsiSmartBuf &buf) {
        uint32_t id;
        OsiCheckError(buf.ReadUint32(&id));
        T obj = new std::remove_pointer_t<T>(buf);
        return Register(obj, id);
    }
};

template <typename FactoryType, typename ObjectType>
class CFactoryProduct {
    uint32_t m_id;
    FactoryType *m_factory;

    friend class CReteFactory<FactoryType, ObjectType>;
};
