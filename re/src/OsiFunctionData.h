#pragma once

#include "OsiSmartBuf.h"
#include "KeyList.h"
#include "List.h"
#include "SearchIndex.h"

enum TOsiValueType {

};

class COsiValueTypeList : public CList<TOsiValueType> {
public:
    virtual bool Write(COsiSmartBuf &buf) {
        if (!buf.WriteUint8(size())) return false;

        for (TOsiValueType type : *this) {
            if (!buf.WriteUint32(type)) return false;
        }

        return true;
    }

    virtual bool Read(COsiSmartBuf &buf) {
        uint8_t numTypes;
        if (!buf.ReadUint8(&numTypes)) return false;

        for (int i = 0; i < numTypes; i++) {
            uint32_t type;
            if (!buf.ReadUint32(&type)) return false;

            push_back((TOsiValueType) type);
        }

        return true;
    }
};

class COsiOutParameterMask {
    uint8_t *m_maskBits;
    int m_numMaskBytes;

public:
    COsiOutParameterMask() : m_maskBits(nullptr), m_numMaskBytes(0) {}

    explicit COsiOutParameterMask(COsiSmartBuf &buf) {
        OsiCheckError(buf.ReadInt(&m_numMaskBytes));
        m_maskBits = new uint8_t[m_numMaskBytes];
        for (int i = 0; i < m_numMaskBytes; i++) {
            OsiCheckError(buf.ReadUint8(&m_maskBits[i]));
        }
    }

    COsiOutParameterMask(const COsiOutParameterMask &other)
        : m_maskBits(new uint8_t[other.m_numMaskBytes]),
          m_numMaskBytes(other.m_numMaskBytes) {}

    COsiOutParameterMask &operator=(const COsiOutParameterMask &other) {
        if (GetNumParameters() != other.GetNumParameters()) {
            delete[] m_maskBits;
            m_numMaskBytes = other.GetNumParameters();
            m_maskBits = new uint8_t[m_numMaskBytes];
        }
        for (int i = 0; i < m_numMaskBytes; i++) {
            m_maskBits[i] = other.m_maskBits[i];
        }

        return *this;
    }

    ~COsiOutParameterMask() {
        delete[] m_maskBits;
    }

    [[nodiscard]] int GetNumParameters() const {
        return m_numMaskBytes * 8;
    }
};

class COsiFunctionDef {
    unsigned char *m_name;
    COsiValueTypeList *m_parameterTypes;
    COsiOutParameterMask m_parameterMask;

public:
    explicit COsiFunctionDef(COsiSmartBuf &buf);

    virtual ~COsiFunctionDef();

    [[nodiscard]] size_t GetNumParameters() const {
        return m_parameterTypes->size();
    }

    const unsigned char *GetName() const {
        return m_name;
    }
};

class CReteNodeFactory;

class COsiFunctionData {
    uint32_t m_unk4;
    uint32_t m_unk8;
    uint32_t m_unkc;
    COsiFunctionDef *m_pFunctionDef;
    uint32_t m_unk14;
    CReteNodeFactory *m_nodeFactory;
    uint8_t m_unk1c;
    COsiKeyList m_keyList;
    uint32_t m_unk30;

    unsigned char *m_name;

public:
    explicit COsiFunctionData(COsiSmartBuf &buf);

    virtual ~COsiFunctionData();

    [[nodiscard]] COsiFunctionDef *GetDefinition() const {
        return m_pFunctionDef;
    }
};

extern COsiSearchIndex<COsiFunctionData *> g_OsiFunctions;
