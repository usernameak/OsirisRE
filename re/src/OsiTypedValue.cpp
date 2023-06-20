#include "OsiTypedValue.h"
#include "OsiDIVObjectData.h"
#include "OsirisLogging.h"
#include "OsiGlobal.h"

COsiTypedValue::COsiTypedValue(COsiSmartBuf &buf) {
    uint8_t isReference;
    OsiCheckError(buf.ReadUint8(&isReference));
    if (isReference == '1') {
        uint32_t type;
        OsiCheckError(buf.ReadUint32(&type));
        SetType((TOsiValueType)type);
        OsiCheckError(buf.ReadUint32(&m_value.iVal));
    } else {
        uint32_t type;
        OsiCheckError(buf.ReadUint32(&type));
        if (g_OsiHadError) return;

        SetType((TOsiValueType)type);
        switch (type) {
        case OsiValueType_NONE:
            break;
        case OsiValueType_INT: {
            uint32_t val = 0;
            OsiCheckError(buf.ReadUint32(&val));
            SetValue(val);
            break;
        }
        case OsiValueType_REAL: {
            float val = 0;
            OsiCheckError(buf.ReadFloat(&val));
            SetValue(val);
            break;
        }
        case OsiValueType_STRING: {
            char str[1024];
            OsiCheckError(buf.ReadEncString((unsigned char *)str));
            SetValue(str);
            break;
        }
        default: {
            unsigned char objectName[1024];
            OsiCheckError(buf.ReadEncString(objectName));
            COsiHashedString key;
            if (objectName[0]) {
                key = COsiHashedString(objectName);
                if (g_OsiDIVObjects.exists(key)) {
                    if (type == g_OsiDIVObjects.lookupPtr(key)->GetType()) {
                        SetValue(key);
                    } else {
                        OSI_TRACEF("LOAD ERROR: object type saved type_%-d does not correspond to object '%s''s type_%-d.",
                            g_OsiDIVObjects.lookupPtr(key)->GetType(),
                            objectName,
                            type);
                        g_OsiHadError = true;
                    }
                } else {
                    OSI_TRACEF("LOAD ERROR: object '%s' is not defined.", objectName);
                    g_OsiHadError = true;
                }
            }
        }
        }
    }
}

bool COsiTypedValue::Write(COsiSmartBuf &buf) const {
    if (!IsReference()) {
        if (!buf.WriteUint8('0')) return false;
        if (!buf.WriteUint32(GetType())) return false;
        switch (GetType()) {
        case OsiValueType_NONE:
            break;
        case OsiValueType_INT:
            if (!buf.WriteUint32(GetValue().iVal)) return false;
            break;
        case OsiValueType_REAL:
            if (!buf.WriteFloat(GetValue().fVal)) return false;
            break;
        case OsiValueType_STRING: {
            const unsigned char *str = (const unsigned char *)GetValue().sVal;
            if (str) {
                if (!buf.WriteUint8(1)) return false;
                if (!buf.WriteEncString(str)) return false;
            } else {
                if (!buf.WriteUint8(0)) return false;
            }
            break;
        }
        default: {
            COsiHashedString key(GetValue().objectKey);
            COsiDIVObjectData *data = g_OsiDIVObjects.lookupPtr(key);
            if (data) {
                if (!buf.WriteEncString(data->GetName())) return false;
            } else {
                if (!buf.WriteEncString((const unsigned char *)"")) return false;
            }
            break;
        }
        }
    } else {
        if (!buf.WriteUint8('1')) return false;
        if (!buf.WriteUint32(GetType())) return false;
        if (!buf.WriteUint32(GetValue().iVal)) return false;
    }
    return true;
}

COsiTypedValue::~COsiTypedValue() {
    if (m_type == OsiValueType_STRING) {
        free((void *)m_value.sVal);
    }
}

void COsiTypedValue::SetType(TOsiValueType type) {
    if (m_type == OsiValueType_STRING) {
        free((void *) m_value.sVal);
        m_value.sVal = nullptr;
    }
    m_type = type;
    m_value.iVal = 0;
    m_value.objectKey = COsiHashedString(0, "");
}

void COsiTypedValue::Dump(char *buffer) const {
    if (IsReference()) {
        sprintf(buffer, "0x%08x", GetValue().iVal);
    } else {
        switch (GetType()) {
        case OsiValueType_NONE:
            sprintf(buffer, "(unknown)");
            break;
        case OsiValueType_INT:
            sprintf(buffer, "%-d", GetValue().iVal);
            break;
        case OsiValueType_REAL:
            sprintf(buffer, "%g", GetValue().fVal);
            if (strchr(buffer, '.') == nullptr) {
                sprintf(buffer, "%.1f", GetValue().fVal);
            }
            break;
        case OsiValueType_STRING:
            sprintf(buffer, "\"%s\"", GetValue().sVal);
            break;
        default: {
            COsiHashedString key(GetValue().objectKey);
            COsiDIVObjectData *obj = g_OsiDIVObjects.lookupPtr(key);
            if (obj) {
                sprintf(buffer, "%s", obj->GetName());
            } else {
                sprintf(buffer, "(undef key)");
            }
            break;
        }
        }
    }
}

void COsiTypedValue::SetValue(COsiTypedValue::Value value) {
    switch (GetType()) {
    case OsiValueType_NONE:
        OSI_TRACE("COsiTypedValue::SetValue() called and type == unknown.\n");
        assert(false); // OsiTypes.cpp:1228
        break;
    case OsiValueType_INT:
        m_value.iVal = value.iVal;
        break;
    case OsiValueType_REAL:
        m_value.fVal = value.fVal;
        break;
    case OsiValueType_STRING:
        free((void *) m_value.sVal);
        m_value.sVal = nullptr;
        if (value.sVal) {
            size_t len = strlen(value.sVal);
            char *newStr = (char *) OsiMemAlloc(len + 1);
            m_value.sVal = newStr;
            memcpy(newStr, value.sVal, len + 1);
        } else {
            m_value.sVal = nullptr;
        }
        break;
    default:
        m_value.objectKey = value.objectKey;
        break;
    }
}

COsiTypedValue::Value COsiTypedValue::GetValue() const {
    return m_value;
}

bool COsiTypedValue::IsReference() const {
    return GetType() >= OsiValueType_MAX && GetValue().iVal;
}
