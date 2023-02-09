#include "Osiris.h"

#include "OsirisLogging.h"
#include "SearchIndex.h"
#include "OsiDIVObjectData.h"
#include "OsiFunctionData.h"
#include "Factory.h"
#include "Rete.h"

COsiris::COsiris() {
    m_globalActionList = new CpOsiFunctionParseDataList;
}

COsiris::~COsiris() {
    delete m_globalActionList;
}

void COsiris::ClearGlobalActionList() {
    m_globalActionList->ClearPointerList();
}

void COsiris::CloseLogFile() {
    if (g_OsiLogFile) {
        fclose(g_OsiLogFile);
        g_OsiLogFile = nullptr;
    }
}

void COsiris::RegisterDIVFunctions(TOsirisInitFunction *funcs) {
    g_OsiDIVFunctions = funcs;
}

bool COsiris::OpenLogFile(const char *FileName, const char *Mode) {
    g_OsiLogFile = fopen(FileName, Mode);
    if (!g_OsiLogFile) {
        g_OsiDIVFunctions.LogMessage("Log file could not be opened.\n");
        return false;
    }

    return true;
}

bool COsiris::_ReadHeader(
    COsiSmartBuf &buf,
    uint8_t &versionMajor,
    uint8_t &versionMinor,
    uint8_t &versionPatch,
    uint8_t &versionBuild,
    char (*storyVersion)[128],
    uint32_t &debugFlags) {

    unsigned char magic[1024]; // [esp+1Ch] [ebp-404h] BYREF

    if (!buf.ReadEncString(magic) ||
        !buf.ReadUint8(&versionMajor) ||
        !buf.ReadUint8(&versionMinor) ||
        !buf.ReadUint8(&versionPatch) ||
        !buf.ReadUint8(&versionBuild)) {
        return false;
    }
    if (versionMajor <= 1u && versionMinor < 2u) {
        memset(storyVersion, 0, sizeof(char[128]));
    } else if (!buf.ReadBytes(storyVersion, 0x80u)) {
        OSI_LOG("Error reading story file version.");

        return false;
    }

    if (versionMajor > 1 || versionMinor >= 4) {
        if (!buf.ReadBytes(&debugFlags, sizeof(uint32_t))) {
            OSI_LOG("Error reading debug flags.");
            return false;
        }

        buf.m_xorValue = 0xAD;
    }
    return true;
}

bool COsiris::_ReadDIVObjects(COsiSmartBuf &buf) {
    uint32_t numObjects;
    if (!buf.ReadUint32(&numObjects)) {
        OSI_TRACE("SaveLoadError: cannot read number of DIV object definitions.");
        return false;
    }

    for (int i = 0; i < numObjects; i++) {
        auto *object = new COsiDIVObjectData(buf);
        if (g_OsiHadError) {
            OSI_TRACEF("SaveLoadError: error reading object definition %-d", i);
            return false;
        }

        g_OsiDIVObjects.set(object->GetName(), object);
    }
    return true;
}

bool COsiris::_ReadFunctions(COsiSmartBuf &buf) {
    uint32_t numFuncDefs;
    if (!buf.ReadUint32(&numFuncDefs)) {
        OSI_TRACE("SaveLoadError: cannot read number of function definitions.");
        return false;
    }

    for (int i = 0; i < numFuncDefs; i++) {
        auto *func = new COsiFunctionData(buf);
        if (g_OsiHadError) {
            OSI_TRACEF("SaveLoadError: error reading function definition %-d", i);
            return false;
        }

        g_OsiFunctions.set(
            COsiHashedString(
                func->GetDefinition()->GetName(),
                func->GetDefinition()->GetNumParameters()),
            func);
    }

    return true;
}

bool COsiris::_ReadReteNodes(COsiSmartBuf &buf) {
    uint32_t numNodes;
    if (!buf.ReadUint32(&numNodes)) {
        OSI_TRACE("SaveLoadError: cannot read number of rete nodes.");
        return false;
    }

    for (int i = 0; i < numNodes; i++) {
        uint8_t nodeType;
        if (!buf.ReadUint8(&nodeType)) {
            OSI_TRACEF("SaveLoadError: cannot read type of node %-d", i);
            return false;
        }

        switch (nodeType) {
        case OSI_RETE_FACT:
            g_ReteNodeFactory.Produce<CReteFact>(buf);
            break;
        case OSI_RETE_EVENT:
            g_ReteNodeFactory.Produce<CReteEvent>(buf);
            break;
        case OSI_RETE_DIV_QUERY:
            g_ReteNodeFactory.Produce<CReteDIVQuery>(buf);
            break;
        case OSI_RETE_AND:
            // TODO:
            break;
        case OSI_RETE_NAND:
            // TODO:
            break;
        case OSI_RETE_REL_CONDITION:
            // TODO:
            break;
        case OSI_RETE_RULE:
            // TODO:
            break;
        case OSI_RETE_INTERNAL_QUERY:
            g_ReteNodeFactory.Produce<CReteInternalQuery>(buf);
            break;
        default:
            OSI_TRACEF("SaveLoadError: 0x%02x: invalid rete node type trying to read node %-d.", nodeType, i);
            return false;
        }

        if (g_OsiHadError) {
            OSI_TRACEF("SaveLoadError: error reading node %-d", i);
        }
    }

    return true;
}

COsiris Osiris;
