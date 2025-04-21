#include "MemoryManager.h"
#include <detours.h>
#include <iostream>
#include <psapi.h>

namespace F2SE {
namespace Memory {

MemoryManager& MemoryManager::GetInstance() {
    static MemoryManager instance;
    return instance;
}

bool MemoryManager::Initialize() {
    if (_initialized) {
        return true;
    }

    if (!GetModuleInfo()) {
        std::cout << "F2SE: Failed to get module information" << std::endl;
        return false;
    }

    _initialized = true;
    std::cout << "F2SE: Memory Manager initialized" << std::endl;
    return true;
}

void MemoryManager::Shutdown() {
    // Restore all protected memory regions
    for (const auto& region : _protectedRegions) {
        DWORD oldProtection;
        VirtualProtect((LPVOID)region.address, region.size, 
                      region.originalProtection, &oldProtection);
    }
    _protectedRegions.clear();
    _initialized = false;
}

void* MemoryManager::GetAddress(DWORD offset) {
    return (void*)(_baseAddress + offset);
}

bool MemoryManager::ReadMemory(DWORD address, void* buffer, size_t size) {
    if (!ValidateAddress(address, size)) {
        return false;
    }

    __try {
        memcpy(buffer, (void*)address, size);
        return true;
    }
    __except(EXCEPTION_EXECUTE_HANDLER) {
        std::cout << "F2SE: Memory read failed at " << std::hex << address << std::endl;
        return false;
    }
}

bool MemoryManager::WriteMemory(DWORD address, const void* buffer, size_t size) {
    if (!ValidateAddress(address, size)) {
        return false;
    }

    DWORD oldProtection;
    if (!VirtualProtect((LPVOID)address, size, PAGE_EXECUTE_READWRITE, &oldProtection)) {
        std::cout << "F2SE: Failed to unprotect memory at " << std::hex << address << std::endl;
        return false;
    }

    __try {
        memcpy((void*)address, buffer, size);
        VirtualProtect((LPVOID)address, size, oldProtection, &oldProtection);
        return true;
    }
    __except(EXCEPTION_EXECUTE_HANDLER) {
        VirtualProtect((LPVOID)address, size, oldProtection, &oldProtection);
        std::cout << "F2SE: Memory write failed at " << std::hex << address << std::endl;
        return false;
    }
}

PlayerData* MemoryManager::GetPlayerData() {
    static PlayerData data;
    
    // Read player name
    ReadMemory(_baseAddress + Memory::PLAYER_NAME, data.name, sizeof(data.name));
    
    // Read stats
    ReadMemory(_baseAddress + Memory::PLAYER_LEVEL, &data.level, sizeof(int));
    ReadMemory(_baseAddress + Memory::PLAYER_XP, &data.experience, sizeof(int));
    ReadMemory(_baseAddress + Memory::PLAYER_HP, &data.currentHP, sizeof(int));
    ReadMemory(_baseAddress + Memory::PLAYER_AP, &data.currentAP, sizeof(int));
    
    // Read SPECIAL stats
    ReadMemory(_baseAddress + Memory::PLAYER_STRENGTH, &data.strength, sizeof(int));
    ReadMemory(_baseAddress + Memory::PLAYER_PERCEPTION, &data.perception, sizeof(int));
    ReadMemory(_baseAddress + Memory::PLAYER_ENDURANCE, &data.endurance, sizeof(int));
    ReadMemory(_baseAddress + Memory::PLAYER_CHARISMA, &data.charisma, sizeof(int));
    ReadMemory(_baseAddress + Memory::PLAYER_INTELLIGENCE, &data.intelligence, sizeof(int));
    ReadMemory(_baseAddress + Memory::PLAYER_AGILITY, &data.agility, sizeof(int));
    ReadMemory(_baseAddress + Memory::PLAYER_LUCK, &data.luck, sizeof(int));
    
    return &data;
}

bool MemoryManager::ProtectMemory(DWORD address, size_t size, DWORD protection) {
    if (!ValidateAddress(address, size)) {
        return false;
    }

    DWORD oldProtection;
    if (!VirtualProtect((LPVOID)address, size, protection, &oldProtection)) {
        return false;
    }

    _protectedRegions.push_back({address, size, oldProtection});
    return true;
}

bool MemoryManager::UnprotectMemory(DWORD address, size_t size) {
    auto it = std::find_if(_protectedRegions.begin(), _protectedRegions.end(),
        [address](const MemoryRegion& region) { return region.address == address; });

    if (it == _protectedRegions.end()) {
        return false;
    }

    DWORD oldProtection;
    if (!VirtualProtect((LPVOID)address, size, it->originalProtection, &oldProtection)) {
        return false;
    }

    _protectedRegions.erase(it);
    return true;
}

DWORD MemoryManager::FindPattern(const char* pattern, const char* mask) {
    MODULEINFO moduleInfo;
    if (!GetModuleInformation(GetCurrentProcess(), 
                             GetModuleHandle(NULL), 
                             &moduleInfo, 
                             sizeof(moduleInfo))) {
        return 0;
    }

    DWORD start = (DWORD)moduleInfo.lpBaseOfDll;
    DWORD size = moduleInfo.SizeOfImage;
    DWORD pos = 0;
    size_t patternLen = strlen(mask);

    for (DWORD i = 0; i < size - patternLen; i++) {
        bool found = true;
        for (size_t j = 0; j < patternLen; j++) {
            if (mask[j] != '?' && pattern[j] != *(char*)(start + i + j)) {
                found = false;
                break;
            }
        }
        if (found) {
            return start + i;
        }
    }

    return 0;
}

std::vector<DWORD> MemoryManager::FindPatterns(const char* pattern, const char* mask) {
    std::vector<DWORD> results;
    MODULEINFO moduleInfo;
    if (!GetModuleInformation(GetCurrentProcess(), 
                             GetModuleHandle(NULL), 
                             &moduleInfo, 
                             sizeof(moduleInfo))) {
        return results;
    }

    DWORD start = (DWORD)moduleInfo.lpBaseOfDll;
    DWORD size = moduleInfo.SizeOfImage;
    size_t patternLen = strlen(mask);

    for (DWORD i = 0; i < size - patternLen; i++) {
        bool found = true;
        for (size_t j = 0; j < patternLen; j++) {
            if (mask[j] != '?' && pattern[j] != *(char*)(start + i + j)) {
                found = false;
                break;
            }
        }
        if (found) {
            results.push_back(start + i);
        }
    }

    return results;
}

bool MemoryManager::CreateHook(DWORD address, void* hook, void** original) {
    if (!ValidateAddress(address, 5)) { // Minimum size for a jump instruction
        return false;
    }

    LONG error = DetourTransactionBegin();
    if (error != NO_ERROR) {
        return false;
    }

    error = DetourUpdateThread(GetCurrentThread());
    if (error != NO_ERROR) {
        DetourTransactionAbort();
        return false;
    }

    error = DetourAttach(&(PVOID&)address, hook);
    if (error != NO_ERROR) {
        DetourTransactionAbort();
        return false;
    }

    *original = (void*)address;
    error = DetourTransactionCommit();
    return (error == NO_ERROR);
}

bool MemoryManager::RemoveHook(DWORD address) {
    if (!ValidateAddress(address, 5)) {
        return false;
    }

    LONG error = DetourTransactionBegin();
    if (error != NO_ERROR) {
        return false;
    }

    error = DetourUpdateThread(GetCurrentThread());
    if (error != NO_ERROR) {
        DetourTransactionAbort();
        return false;
    }

    error = DetourDetach(&(PVOID&)address, (PVOID)address);
    if (error != NO_ERROR) {
        DetourTransactionAbort();
        return false;
    }

    error = DetourTransactionCommit();
    return (error == NO_ERROR);
}

bool MemoryManager::GetModuleInfo() {
    MODULEINFO moduleInfo;
    HMODULE module = GetModuleHandle(NULL);
    
    if (!module || !GetModuleInformation(GetCurrentProcess(), 
                                       module, 
                                       &moduleInfo, 
                                       sizeof(moduleInfo))) {
        return false;
    }

    _baseAddress = (DWORD)moduleInfo.lpBaseOfDll;
    return true;
}

bool MemoryManager::ValidateAddress(DWORD address, size_t size) {
    if (!_initialized) {
        return false;
    }

    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery((LPCVOID)address, &mbi, sizeof(mbi)) == 0) {
        return false;
    }

    return (mbi.State == MEM_COMMIT && 
            !(mbi.Protect & PAGE_GUARD) && 
            ((mbi.Protect & PAGE_EXECUTE_READ) || 
             (mbi.Protect & PAGE_EXECUTE_READWRITE) || 
             (mbi.Protect & PAGE_READONLY) || 
             (mbi.Protect & PAGE_READWRITE)));
}

} // namespace Memory
} // namespace F2SE 