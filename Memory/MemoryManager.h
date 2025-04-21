#pragma once

#include "../Common/Types.h"
#include <windows.h>
#include <vector>

namespace F2SE {
namespace Memory {

class MemoryManager {
public:
    static MemoryManager& GetInstance();

    bool Initialize();
    void Shutdown();

    // Memory access functions
    void* GetAddress(DWORD offset);
    bool ReadMemory(DWORD address, void* buffer, size_t size);
    bool WriteMemory(DWORD address, const void* buffer, size_t size);
    
    // Game data access
    PlayerData* GetPlayerData();

    // Memory protection
    bool ProtectMemory(DWORD address, size_t size, DWORD protection);
    bool UnprotectMemory(DWORD address, size_t size);

    // Pattern scanning
    DWORD FindPattern(const char* pattern, const char* mask);
    std::vector<DWORD> FindPatterns(const char* pattern, const char* mask);

    // Hook management
    bool CreateHook(DWORD address, void* hook, void** original);
    bool RemoveHook(DWORD address);

private:
    MemoryManager() = default;
    ~MemoryManager() = default;
    MemoryManager(const MemoryManager&) = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;

    struct MemoryRegion {
        DWORD address;
        size_t size;
        DWORD originalProtection;
    };

    std::vector<MemoryRegion> _protectedRegions;
    DWORD _baseAddress;
    bool _initialized;

    bool GetModuleInfo();
    bool ValidateAddress(DWORD address, size_t size);
};

} // namespace Memory
} // namespace F2SE 