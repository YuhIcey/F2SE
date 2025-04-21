#pragma once

namespace F2SE {
namespace Game {

// Memory patterns to find addresses dynamically
struct MemoryPattern {
    const unsigned char* pattern;
    const char* mask;
    size_t size;
};

// Game version information
struct GameVersion {
    const char* version;
    DWORD launcherCheckAddress;
    DWORD mainMenuSkipAddress;
    DWORD gameStartAddress;
};

// Known game versions and their addresses
const GameVersion GAME_VERSIONS[] = {
    // Version 1.02d US
    {
        "1.02d US",
        0x4A1B20,  // Launcher check
        0x4A2C30,  // Main menu
        0x4A3D40   // Game start
    },
    // Version 1.02d UK
    {
        "1.02d UK",
        0x4A1B30,  // Launcher check
        0x4A2C40,  // Main menu
        0x4A3D50   // Game start
    }
    // Add more versions as needed
};

// Memory patterns for dynamic address finding
const MemoryPattern PATTERNS[] = {
    // Launcher check pattern
    {
        (const unsigned char*)"\x83\xEC\x08\x56\x68\x00\x00\x00\x00",
        "xxxxx????",
        9
    },
    // Main menu pattern
    {
        (const unsigned char*)"\x6A\x00\x68\x00\x00\x00\x00\xE8",
        "xx????xx",
        8
    }
};

// Patch data
struct PatchData {
    // Skip launcher check
    const unsigned char LAUNCHER_SKIP[5] = { 0x90, 0x90, 0x90, 0x90, 0x90 };  // NOP

    // Skip main menu (jump to game start)
    const unsigned char MENU_SKIP[5] = { 0xE9, 0x00, 0x00, 0x00, 0x00 };  // JMP

    // Original bytes (for restoration)
    unsigned char originalLauncher[5];
    unsigned char originalMenu[5];
};

class GamePatcher {
public:
    static GamePatcher& GetInstance();

    bool Initialize(HANDLE processHandle);
    bool PatchGame();
    bool RestoreGame();
    const char* GetGameVersion() const;

private:
    GamePatcher() = default;
    ~GamePatcher() = default;
    GamePatcher(const GamePatcher&) = delete;
    GamePatcher& operator=(const GamePatcher&) = delete;

    bool FindGameAddresses();
    bool VerifyGameVersion();
    DWORD FindPattern(const unsigned char* pattern, const char* mask, size_t size);
    bool WriteMemory(DWORD address, const void* buffer, size_t size);
    bool ReadMemory(DWORD address, void* buffer, size_t size);

    HANDLE _processHandle;
    const GameVersion* _currentVersion;
    PatchData _patches;
    bool _initialized;
    bool _patched;
};

} // namespace Game
} // namespace F2SE 