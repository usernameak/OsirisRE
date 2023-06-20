#pragma once

#include <cstdint>
#include "OsiFunctionData.h"
#include "SearchIndex.h"

class COsiTypedValue {
public:
    struct Value {
        union {
            uint32_t iVal;
            float fVal;
            const char *sVal;
        };
        COsiHashedString objectKey;

        Value() : iVal(0), objectKey(0, "") {}
        Value(uint32_t val) : iVal(0) {}
        Value(float val) : fVal(0) {}
        Value(const char *val) : sVal(val) {}
        Value(const COsiHashedString &objectKey) {
            this->objectKey = objectKey;
        }
    };

private:
    TOsiValueType m_type;
    Value m_value;

public:
    COsiTypedValue() : m_type(OsiValueType_NONE) {}
    explicit COsiTypedValue(COsiSmartBuf &buf);
    virtual bool Write(COsiSmartBuf &buf) const;

    virtual ~COsiTypedValue();

    [[nodiscard]] bool IsReference() const;

    virtual void SetType(TOsiValueType type);
    [[nodiscard]] virtual TOsiValueType GetType() const {
        return m_type;
    }
    [[nodiscard]] virtual bool IsValid() const {
        return GetType() != OsiValueType_NONE;
    }
    virtual void Dump(char *buffer) const;
    virtual void SetValue(Value value);
    virtual Value GetValue() const;
};
