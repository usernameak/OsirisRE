#include "OsirisLogging.h"

FILE *g_OsiLogFile;

void OsiLogToFile(const char *str) {
    if (g_OsiLogFile) {
        fprintf(g_OsiLogFile, str);
        fflush(g_OsiLogFile);
    }
}
