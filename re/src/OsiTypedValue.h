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
    explicit COsiTypedValue(COsiSmartBuf &buf);

    void SetType(TOsiValueType type);
    void SetValue(Value value);
};
