#pragma once

#include <cstdio>
#include "OsirisFuncs.h"

extern FILE *g_OsiLogFile;

void OsiLogToFile(const char *str);

#define OSI_LOG(str)                   \
    g_OsiDIVFunctions.LogMessage(str); \
    OsiLogToFile(str "\n");

#define OSI_TRACE(str) /* TODO: */
#define OSI_TRACEF(...) /* TODO: */