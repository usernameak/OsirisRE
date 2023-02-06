#pragma once

#include <cstdint>
#include "OsiSmartBuf.h"

struct COsiKeyList {
    uint32_t keys[4];

    COsiKeyList() : keys{} {}

    bool Read(COsiSmartBuf &buf) {
        return buf.ReadUint32(&keys[0]) &&
               buf.ReadUint32(&keys[1]) &&
               buf.ReadUint32(&keys[2]) &&
               buf.ReadUint32(&keys[3]);
    }
};
