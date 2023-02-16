#include <Windows.h>
#include <cstdio>
#include <cstdint>
#include <Osiris.h>
#include "cpptoml.h"

bool g_didLoadConfig = false;
bool g_useLogFile    = false;
bool g_useForceInit  = true;
bool g_isLogFileOpen = false;

class CFakeOsiris : public COsiris {
public:
    bool __thiscall FakeLoad(const char *filename) {
        if (!g_didLoadConfig) {
            try {
                auto config = cpptoml::parse_file("plugins/osi_extender.ini");

                g_useLogFile   = config->get_as<bool>("enable_osiris_logging").value_or(g_useLogFile);
                g_useForceInit = config->get_as<bool>("enable_force_init").value_or(g_useForceInit);
            } catch (cpptoml::parse_exception &) {
                MessageBoxW(
                    nullptr,
                    L"plugins/osi_extender.ini cannot be loaded",
                    L"DD Osiris Extender",
                    MB_OK | MB_ICONERROR);
            }
            g_didLoadConfig = true;
        }

        if (g_useLogFile && !g_isLogFileOpen) {
            Osiris.OpenLogFile("Osiris.log", "w");
            g_isLogFileOpen = true;
        }

        if (Load(filename)) {
            if (g_useForceInit && strcmp(filename, "main\\startup\\story.000") == 0) {
                return InitGame();
            } else {
                return true;
            }
        }
        return false;
    }
};

void initialize(DWORD major, DWORD minor, DWORD patch, DWORD build) {

    if (major == 1 && minor == 0 && patch == 0 && build == 62) {
        // 1.0.0.62
        DWORD oldProtect;
        VirtualProtect((void *)(uintptr_t)0x517000, 0x1000, PAGE_EXECUTE_READWRITE, &oldProtect);

        // some extremely dangerous shit
        uintptr_t fakeLoad;
        __asm {
            mov eax, dword ptr CFakeOsiris::FakeLoad
            mov fakeLoad, eax
        }
        ;
        const uintptr_t INSN_ADDR       = 0x5170E1;
        *((uint8_t *)(INSN_ADDR))       = 0x90;
        *((uint8_t *)(INSN_ADDR + 1))   = 0xE8;
        *((uintptr_t *)(INSN_ADDR + 2)) = fakeLoad - INSN_ADDR - 0x6;
    } else if (major == 1 && minor == 2 && patch == 0 && build == 0) {
        // ConfigTool, ignore it
    } else {
        MessageBoxW(NULL, L"Borderless Mod: Unsupported Divine Divinity version", L"div_slashopt", MB_OK);
    }
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        // get version number
        HMODULE exeMod = GetModuleHandleW(nullptr);
        WCHAR szVersionFile[MAX_PATH];
        GetModuleFileNameW(exeMod, szVersionFile, MAX_PATH);

        DWORD verHandle = 0;
        UINT size       = 0;
        LPBYTE lpBuffer = NULL;
        DWORD verSize   = GetFileVersionInfoSizeW(szVersionFile, &verHandle);

        if (verSize != NULL) {
            auto verData = new char[verSize];

            if (GetFileVersionInfoW(szVersionFile, verHandle, verSize, verData)) {
                if (VerQueryValueW(verData, L"\\", (VOID FAR * FAR *)&lpBuffer, &size)) {
                    if (size) {
                        auto *verInfo = (VS_FIXEDFILEINFO *)lpBuffer;
                        if (verInfo->dwSignature == 0xfeef04bd) {
                            DWORD major = (verInfo->dwFileVersionMS >> 16) & 0xffff;
                            DWORD minor = (verInfo->dwFileVersionMS >> 0) & 0xffff;
                            DWORD patch = (verInfo->dwFileVersionLS >> 16) & 0xffff;
                            DWORD build = (verInfo->dwFileVersionLS >> 0) & 0xffff;

                            initialize(major, minor, patch, build);

                            delete[] verData;
                            return TRUE;
                        }
                    }
                }
            }
            delete[] verData;

            initialize(0, 0, 0, 0);
        }
    }

    return TRUE;
}