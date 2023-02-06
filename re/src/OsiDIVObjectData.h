#pragma once

#include <cstdlib>
#include <cstdint>

#include "OsiSmartBuf.h"
#include "KeyList.h"
#include "SearchIndex.h"

class COsiDIVObjectData {
    COsiKeyList m_keyList;
    uint32_t m_unk14;
    uint8_t m_type;

    unsigned char *m_name;

public:
    COsiDIVObjectData(COsiSmartBuf &buf);

    [[nodiscard]] const unsigned char *GetName() const {
        return m_name;
    }

    virtual ~COsiDIVObjectData();
};

extern COsiSearchIndex<COsiDIVObjectData *> g_OsiDIVObjects;
