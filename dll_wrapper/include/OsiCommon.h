#pragma once

extern bool g_OsiHadError;

inline void OsiCheckError(bool status) {
    if (!status) {
        g_OsiHadError = true;
    }
}
