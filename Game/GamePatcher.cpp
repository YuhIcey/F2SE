#include <Windows.h>
#include <vector>
#include "GamePatches.h"

namespace F2SE {
namespace Game {

GamePatcher& GamePatcher::GetInstance() {
    static GamePatcher instance;
    return instance;
}

bool GamePatcher::Initialize(HANDLE processHandle) {
    if (_initialized) {
        return true;
    }

    _processHandle = processHandle;
    _currentVersion = nullptr;
    _patched = false;
    
    if (!FindGameAddresses()) {
        return false;
    }

    _initialized = true;
    return true;
}

bool GamePatcher::PatchGame() {
    if (!_initialized || _patched) {
        return false;
    }

    // Backup original bytes
    if (!ReadMemory(_currentVersion->launcherCheckAddress, _patches.originalLauncher, sizeof(_patches.originalLauncher)) ||
        !ReadMemory(_currentVersion->mainMenuSkipAddress, _patches.originalMenu, sizeof(_patches.originalMenu))) {
        return false;
    }

    // Apply launcher check patch
    if (!WriteMemory(_currentVersion->launcherCheckAddress, _patches.LAUNCHER_SKIP, sizeof(_patches.LAUNCHER_SKIP))) {
        return false;
    }

    // Calculate jump offset for menu skip
    DWORD jumpOffset = _currentVersion->gameStartAddress - (_currentVersion->mainMenuSkipAddress + 5);
    unsigned char menuSkip[5] = { 0xE9, 0x00, 0x00, 0x00, 0x00 };
    memcpy(&menuSkip[1], &jumpOffset, sizeof(DWORD));

    // Apply menu skip patch
    if (!WriteMemory(_currentVersion->mainMenuSkipAddress, menuSkip, sizeof(menuSkip))) {
        // Try to restore launcher patch if menu patch fails
        WriteMemory(_currentVersion->launcherCheckAddress, _patches.originalLauncher, sizeof(_patches.originalLauncher));
        return false;
    }

    _patched = true;
    return true;
}

bool GamePatcher::RestoreGame() {
    if (!_initialized || !_patched) {
        return false;
    }

    // Restore original bytes
    if (!WriteMemory(_currentVersion->launcherCheckAddress, _patches.originalLauncher, sizeof(_patches.originalLauncher)) ||
        !WriteMemory(_currentVersion->mainMenuSkipAddress, _patches.originalMenu, sizeof(_patches.originalMenu))) {
        return false;
    }

    _patched = false;
    return true;
}

const char* GamePatcher::GetGameVersion() const {
    return _currentVersion ? _currentVersion->version : "Unknown";
}

bool GamePatcher::FindGameAddresses() {
    // Read game's PE header to get base address
    DWORD baseAddress = 0;
    MEMORY_BASIC_INFORMATION mbi;
    
    if (VirtualQueryEx(_processHandle, (LPCVOID)0x400000, &mbi, sizeof(mbi))) {
        baseAddress = (DWORD)mbi.AllocationBase;
    }

    if (!baseAddress) {
        return false;
    }

    // Read game version information
    char versionBuffer[MAX_PATH];
    DWORD versionSize = GetFileVersionInfoSizeA("Fallout2.exe", nullptr);
    
    if (versionSize > 0) {
        std::vector<BYTE> versionData(versionSize);
        if (GetFileVersionInfoA("Fallout2.exe", 0, versionSize, versionData.data())) {
            UINT len;
            LPVOID version;
            if (VerQueryValueA(versionData.data(), "\\StringFileInfo\\040904B0\\ProductVersion", &version, &len)) {
                strncpy_s(versionBuffer, (const char*)version, len);
                
                // Find matching version in our database
                for (const auto& ver : GAME_VERSIONS) {
                    if (strcmp(versionBuffer, ver.version) == 0) {
                        _currentVersion = &ver;
                        return true;
                    }
                }
            }
        }
    }

    // If version not found, try pattern scanning
    DWORD launcherAddr = FindPattern(PATTERNS[0].pattern, PATTERNS[0].mask, PATTERNS[0].size);
    DWORD menuAddr = FindPattern(PATTERNS[1].pattern, PATTERNS[1].mask, PATTERNS[1].size);

    if (launcherAddr && menuAddr) {
        // Create a temporary version entry
        static GameVersion dynamicVersion = {
            "Dynamic",
            launcherAddr,
            menuAddr,
            menuAddr + 0x1000  // Estimated game start address
        };
        _currentVersion = &dynamicVersion;
        return true;
    }

    return false;
}

DWORD GamePatcher::FindPattern(const unsigned char* pattern, const char* mask, size_t size) {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    DWORD_PTR start = (DWORD_PTR)sysInfo.lpMinimumApplicationAddress;
    DWORD_PTR end = (DWORD_PTR)sysInfo.lpMaximumApplicationAddress;

    while (start < end) {
        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQueryEx(_processHandle, (LPCVOID)start, &mbi, sizeof(mbi))) {
            if (mbi.State == MEM_COMMIT && mbi.Protect != PAGE_NOACCESS) {
                std::vector<unsigned char> buffer(mbi.RegionSize);
                SIZE_T bytesRead;

                if (ReadProcessMemory(_processHandle, mbi.BaseAddress, buffer.data(), mbi.RegionSize, &bytesRead)) {
                    for (size_t i = 0; i < bytesRead - size; i++) {
                        bool found = true;
                        for (size_t j = 0; j < size; j++) {
                            if (mask[j] == 'x' && buffer[i + j] != pattern[j]) {
                                found = false;
                                break;
                            }
                        }
                        if (found) {
                            return (DWORD)mbi.BaseAddress + i;
                        }
                    }
                }
            }
            start += mbi.RegionSize;
        }
        else {
            start += 4096;
        }
    }

    return 0;
}

bool GamePatcher::WriteMemory(DWORD address, const void* buffer, size_t size) {
    SIZE_T bytesWritten;
    DWORD oldProtect;

    if (!VirtualProtectEx(_processHandle, (LPVOID)address, size, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        return false;
    }

    bool result = WriteProcessMemory(_processHandle, (LPVOID)address, buffer, size, &bytesWritten) 
                 && bytesWritten == size;

    VirtualProtectEx(_processHandle, (LPVOID)address, size, oldProtect, &oldProtect);
    return result;
}

bool GamePatcher::ReadMemory(DWORD address, void* buffer, size_t size) {
    SIZE_T bytesRead;
    return ReadProcessMemory(_processHandle, (LPVOID)address, buffer, size, &bytesRead) 
           && bytesRead == size;
}

} // namespace Game
} // namespace F2SE 