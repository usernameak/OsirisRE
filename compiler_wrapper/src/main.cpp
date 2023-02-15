#include <cstdio>
#include <Osiris.h>
#include <functional>
#include <vector>

enum OsiCCErrorCode : int {
    OSICCERR_OK               = 0,
    OSICCERR_INVALID_ARGUMENT = 1,
    OSICCERR_CANT_OPEN_LOG    = 2,
    OSICCERR_CANT_SAVE        = 3,
    OSICCERR_CANT_INIT_GAME   = 4,
};

uint32_t getDIVHandle(uint8_t a, uint16_t b, uint16_t c) {
    return a & 0xF | (16 * (b | ((c & 0x3FF) << 16)));
}

bool invokeCall(uint32_t handle, void *argsdata) {
    return false;
}

bool invokeQuery(uint32_t handle, void *argsdata) {
    return false;
}

void osirisError(const char *msg) {
    fprintf(stderr, "Error from Osiris: %s\n", msg);
}

int main(int argc, char **argv) {
    // Osiris.DeleteAllData();

    TOsirisInitFunction initFunctions{};
    initFunctions.getDIVHandle = getDIVHandle;
    initFunctions.invokeCall   = invokeCall;
    initFunctions.invokeQuery  = invokeQuery;
    initFunctions.osirisError  = osirisError;

    Osiris.RegisterDIVFunctions(&initFunctions);

    std::vector<std::function<int()>> buildTargets;

    if (argc == 1) {
        fprintf(stderr, "No build targets specified.\n");
        return OSICCERR_INVALID_ARGUMENT;
    }

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-log") == 0) {
            if (++i >= argc) {
                fprintf(stderr, "-log requires an argument\n");
                return OSICCERR_INVALID_ARGUMENT;
            }

            buildTargets.emplace_back([filename = argv[i]]() {
                return Osiris.OpenLogFile(filename, "w") ? OSICCERR_OK : OSICCERR_CANT_OPEN_LOG;
            });
        } else if (strcmp(argv[i], "-compile") == 0) {
            if (++i >= argc) {
                fprintf(stderr, "-compile requires an argument\n");
                return OSICCERR_INVALID_ARGUMENT;
            }

            buildTargets.emplace_back([filename = argv[i]]() {
                if (!Osiris.Compile(filename, "r")) {
                    fprintf(stderr,
                        "compilation finished with errors/warnings,\n"
                        "refer to log file for more information\n");
                    return OSICCERR_OK;
                }
                return OSICCERR_OK;
            });
        } else if (strcmp(argv[i], "-init") == 0) {
            buildTargets.emplace_back([filename = argv[i]]() {
                if (!Osiris.InitGame()) {
                    fprintf(stderr,
                        "osiris init game failed\n");
                    return OSICCERR_CANT_INIT_GAME;
                }
                return OSICCERR_OK;
            });
        } else if (strcmp(argv[i], "-save") == 0) {
            if (++i >= argc) {
                fprintf(stderr, "-save requires an argument\n");
                return OSICCERR_INVALID_ARGUMENT;
            }

            buildTargets.emplace_back([filename = argv[i]]() {
                if (Osiris.Save(filename)) {
                    return OSICCERR_CANT_SAVE;
                }
                return OSICCERR_OK;
            });
        }
    }

    for (auto &target : buildTargets) {
        int code = target();
        if (code != OSICCERR_OK) {
            return code;
        }
    }

    fprintf(stderr, "build finished\n");

    return OSICCERR_OK;
}