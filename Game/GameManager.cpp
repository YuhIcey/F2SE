#include "GameManager.h"
#include <iostream>

namespace F2SE {
namespace Game {

GameManager& GameManager::GetInstance() {
    static GameManager instance;
    return instance;
}

bool GameManager::Initialize() {
    if (_initialized) {
        return true;
    }

    std::cout << "F2SE: Detecting game version..." << std::endl;
    _version = DetectGameVersion();
    
    if (_version == GameVersion::Unknown) {
        std::cout << "F2SE: Failed to detect game version" << std::endl;
        return false;
    }

    InitializeAddresses();
    
    if (!ValidateAddresses()) {
        std::cout << "F2SE: Failed to validate game addresses" << std::endl;
        return false;
    }

    _initialized = true;
    std::cout << "F2SE: Game Manager initialized for version " 
              << (_version == GameVersion::US_102 ? "US 1.02" : "US 1.02d") 
              << std::endl;
    return true;
}

void GameManager::Shutdown() {
    _initialized = false;
    _version = GameVersion::Unknown;
}

GameVersion GameManager::DetectGameVersion() {
    if (DetectVersion102()) {
        return GameVersion::US_102;
    }
    if (DetectVersion102d()) {
        return GameVersion::US_102d;
    }
    return GameVersion::Unknown;
}

bool GameManager::IsGameLoaded() const {
    // TODO: Implement game state detection
    return true;
}

bool GameManager::IsInCombat() const {
    // TODO: Implement combat state detection
    return false;
}

bool GameManager::IsInDialog() const {
    // TODO: Implement dialog state detection
    return false;
}

bool GameManager::IsInInventory() const {
    // TODO: Implement inventory state detection
    return false;
}

std::string GameManager::GetPlayerName() {
    char name[32];
    auto& memory = Memory::MemoryManager::GetInstance();
    memory.ReadMemory(_addresses.dat_name, name, sizeof(name));
    return std::string(name);
}

void GameManager::SetPlayerName(const std::string& name) {
    auto& memory = Memory::MemoryManager::GetInstance();
    memory.WriteMemory(_addresses.dat_name, name.c_str(), name.length() + 1);
}

void GameManager::GetWorldPosition(short& x, short& y) {
    auto& memory = Memory::MemoryManager::GetInstance();
    memory.ReadMemory(_addresses.world_x, &x, sizeof(short));
    memory.ReadMemory(_addresses.world_y, &y, sizeof(short));
}

void GameManager::SetWorldPosition(short x, short y) {
    auto& memory = Memory::MemoryManager::GetInstance();
    memory.WriteMemory(_addresses.world_x, &x, sizeof(short));
    memory.WriteMemory(_addresses.world_y, &y, sizeof(short));
}

void GameManager::GetGameDate(short& year, char& month, char& day) {
    auto& memory = Memory::MemoryManager::GetInstance();
    memory.ReadMemory(_addresses.start_year, &year, sizeof(short));
    memory.ReadMemory(_addresses.start_month, &month, sizeof(char));
    memory.ReadMemory(_addresses.start_day, &day, sizeof(char));
}

void GameManager::SetGameDate(short year, char month, char day) {
    auto& memory = Memory::MemoryManager::GetInstance();
    memory.WriteMemory(_addresses.start_year, &year, sizeof(short));
    memory.WriteMemory(_addresses.start_month, &month, sizeof(char));
    memory.WriteMemory(_addresses.start_day, &day, sizeof(char));
}

int GameManager::ComputeDamage(const AttackData& attack, const DamageComputation& computation) {
    // Store original computation data
    auto& memory = Memory::MemoryManager::GetInstance();
    memory.WriteMemory(_addresses.attack_data, &attack, sizeof(AttackData));
    
    // Call the original damage computation function
    DWORD result;
    __asm {
        push computation.nbhit
        push computation.ammo_dr_adj
        push computation.dmg_thresh
        push computation.cbt_diff_mod
        push computation.dam_div
        push computation.dam_mult
        push computation.bonus_ranged
        call [_addresses.damages_computation]
        mov result, eax
        add esp, 28  // Clean up stack (7 parameters * 4 bytes)
    }
    
    return result;
}

void GameManager::ModifyDamageComputation(const std::function<void(DamageComputation&)>& modifier) {
    // Hook the damage computation function to modify its behavior
    auto& hooks = Hooks::HookManager::GetInstance();
    
    auto damageHook = [](void* context) {
        auto comp = static_cast<DamageComputation*>(context);
        GameManager::GetInstance().ModifyDamageComputation([comp](DamageComputation& dc) {
            *comp = dc;
        });
    };
    
    hooks.AddPreHookCallback(_addresses.damages_computation, damageHook);
}

void GameManager::TogglePipBoy(bool enable) {
    auto& memory = Memory::MemoryManager::GetInstance();
    BYTE value = enable ? 1 : 0;
    memory.WriteMemory(_addresses.pipboy, &value, sizeof(BYTE));
}

void GameManager::ToggleMovies(bool enable) {
    auto& memory = Memory::MemoryManager::GetInstance();
    BYTE value = enable ? 1 : 0;
    memory.WriteMemory(_addresses.nomovie, &value, sizeof(BYTE));
}

void GameManager::SetStartingMap(const std::string& mapName) {
    auto& memory = Memory::MemoryManager::GetInstance();
    memory.WriteMemory(_addresses.startmap, mapName.c_str(), mapName.length() + 1);
}

void GameManager::SetModName(const std::string& modName) {
    auto& memory = Memory::MemoryManager::GetInstance();
    memory.WriteMemory(_addresses.mod_name, modName.c_str(), modName.length() + 1);
}

bool GameManager::DetectVersion102() {
    auto& memory = Memory::MemoryManager::GetInstance();
    
    // Check for specific patterns or values that identify version 1.02
    BYTE pattern[] = { 0x68, 0x11, 0x70, 0x49, 0x00 }; // Push instruction with pipboy address
    DWORD address = memory.FindPattern((char*)pattern, "xxxxx");
    
    return address == US_102_ADDRESSES.pipboy;
}

bool GameManager::DetectVersion102d() {
    auto& memory = Memory::MemoryManager::GetInstance();
    
    // Check for specific patterns or values that identify version 1.02d
    BYTE pattern[] = { 0x68, 0xC1, 0x71, 0x49, 0x00 }; // Push instruction with pipboy address
    DWORD address = memory.FindPattern((char*)pattern, "xxxxx");
    
    return address == US_102D_ADDRESSES.pipboy;
}

void GameManager::InitializeAddresses() {
    switch (_version) {
        case GameVersion::US_102:
            _addresses = US_102_ADDRESSES;
            break;
        case GameVersion::US_102d:
            _addresses = US_102D_ADDRESSES;
            break;
        default:
            break;
    }
}

bool GameManager::ValidateAddresses() {
    if (_version == GameVersion::Unknown) {
        return false;
    }

    auto& memory = Memory::MemoryManager::GetInstance();
    
    // Validate critical addresses
    if (!memory.ValidateAddress(_addresses.pipboy, 1) ||
        !memory.ValidateAddress(_addresses.damages_computation, 1) ||
        !memory.ValidateAddress(_addresses.world_x, 2) ||
        !memory.ValidateAddress(_addresses.world_y, 2)) {
        return false;
    }

    return true;
}

} // namespace Game
} // namespace F2SE 