#include "pch.h"
#include "patches.h"
#include "utils.h"

using namespace Memory::VP;

bool DisableSignatureCheck(const char* pSigCheck) {
    HMODULE moduleHandle = GetModuleHandleA(NULL);
    uint64_t* lpSigCheckPattern = FindPattern(moduleHandle, pSigCheck);
    std::cout << "lpSigCheckPattern " << (lpSigCheckPattern ? "Patched" : "FAILED") << "\n";
    if (!lpSigCheckPattern) return false;
    uint64_t hook_address = (uint64_t)lpSigCheckPattern;
    uint8_t* patchAddr = (uint8_t*)(GetGameAddr(hook_address) - 0x14);

    if (*patchAddr != 0xC3) { // Check if already patched
        Patch<uint8_t>(patchAddr, 0xC3);
        Patch<uint32_t>(patchAddr + 1, 0x90909090);
    }
    return true;
}

bool DisableSignatureWarn(const char* pSigWarn) {
    HMODULE moduleHandle = GetModuleHandleA(NULL);
    uint64_t* lpSigWarnPattern = FindPattern(moduleHandle, pSigWarn);
    std::cout << "lpSigWarnPattern " << (lpSigWarnPattern ? "Patched" : "FAILED") << "\n";
    if (!lpSigWarnPattern) return false;
    ConditionalJumpToJump((uint64_t)lpSigWarnPattern, 0xA);
    return true;
}

bool DisableChunkSigCheck(const char* pChunkSigCheck, const char* pChunkSigCheckFunc) {
    HMODULE moduleHandle = GetModuleHandleA(NULL);
    uint64_t* lpChunkSigCheckPattern = FindPattern(moduleHandle, pChunkSigCheck);
    uint64_t* lpChunkSigCheckFuncPattern = FindPattern(moduleHandle, pChunkSigCheckFunc);
    std::cout << "lpChunkSigCheckPattern " << (lpChunkSigCheckPattern ? "Patched" : "FAILED") << "\n";
    std::cout << "lpChunkSigCheckFuncPattern " << (lpChunkSigCheckFuncPattern ? "Patched" : "FAILED") << "\n";
    if (!lpChunkSigCheckPattern || !lpChunkSigCheckFuncPattern) return false;

    uint64_t relativeAddress = ((uint64_t)lpChunkSigCheckFuncPattern) - (((uint64_t)lpChunkSigCheckPattern) + 0xE + 5);
    if (relativeAddress > std::numeric_limits<uint32_t>::max()) {
        std::cout << "Error: Address out of range for 32-bit patching." << std::endl;
        return false;
    }

    Patch<uint32_t>(((uint64_t)lpChunkSigCheckPattern) + 0xF, static_cast<uint32_t>(relativeAddress));
    return true;
}
