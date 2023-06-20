#pragma once

#include <cstdlib>
#include <cassert>
#include <cstring>

inline void *OsiMemAlloc(size_t size) {
    void *pMem = malloc(size);
    assert(pMem != NULL); // OsiGlobal.h:286
    memset(pMem, 0, size);
    return pMem;
}