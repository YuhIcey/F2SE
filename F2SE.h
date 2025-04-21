#pragma once

#include <windows.h>
#include <string>

namespace F2SE {

// Version information
constexpr int VERSION_MAJOR = 0;
constexpr int VERSION_MINOR = 1;
constexpr int VERSION_PATCH = 0;

// Game memory addresses (you'll need to update these for your Fallout 2 version)
namespace Memory {
    constexpr DWORD MAIN_LOOP = 0x1234;
    constexpr DWORD SCRIPT_PROC = 0x5678;
    constexpr DWORD GAME_INIT = 0x9ABC;
    
    // Player data
    constexpr DWORD PLAYER_NAME = 0x0000;
    constexpr DWORD PLAYER_LEVEL = 0x0000;
    constexpr DWORD PLAYER_XP = 0x0000;
    constexpr DWORD PLAYER_HP = 0x0000;
    constexpr DWORD PLAYER_AP = 0x0000;
    
    // SPECIAL stats
    constexpr DWORD PLAYER_STRENGTH = 0x0000;
    constexpr DWORD PLAYER_PERCEPTION = 0x0000;
    constexpr DWORD PLAYER_ENDURANCE = 0x0000;
    constexpr DWORD PLAYER_CHARISMA = 0x0000;
    constexpr DWORD PLAYER_INTELLIGENCE = 0x0000;
    constexpr DWORD PLAYER_AGILITY = 0x0000;
    constexpr DWORD PLAYER_LUCK = 0x0000;
}

// Game structures
struct PlayerData {
    char name[32];
    int level;
    int experience;
    int currentHP;
    int maxHP;
    int currentAP;
    int maxAP;
    
    // SPECIAL stats
    int strength;
    int perception;
    int endurance;
    int charisma;
    int intelligence;
    int agility;
    int luck;
};

// Script extensions
struct ScriptExtension {
    const char* name;
    const char* description;
    void* function;
};

// Plugin interface
class IPlugin {
public:
    virtual ~IPlugin() = default;
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    virtual const char* GetName() = 0;
    virtual const char* GetAuthor() = 0;
    virtual const char* GetVersion() = 0;
};

// API Functions
extern "C" {
    bool __declspec(dllexport) RegisterScriptExtension(const ScriptExtension* extension);
    bool __declspec(dllexport) UnregisterScriptExtension(const char* name);
    void* __declspec(dllexport) GetGameAddress(DWORD offset);
    bool __declspec(dllexport) ReadGameMemory(DWORD address, void* buffer, size_t size);
    bool __declspec(dllexport) WriteGameMemory(DWORD address, const void* buffer, size_t size);
    PlayerData* __declspec(dllexport) GetPlayerData();
}

} // namespace F2SE 