#include "OsiTypedValue.h"
#include "OsiDIVObjectData.h"
#include "OsirisLogging.h"

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
