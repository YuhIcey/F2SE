#pragma once

#include "GameAddresses.h"
#include "../Memory/MemoryManager.h"
#include <string>

namespace F2SE {
namespace Game {

class GameManager {
public:
    static GameManager& GetInstance();

    bool Initialize();
    void Shutdown();

    // Game version detection
    GameVersion DetectGameVersion();
    const GameAddresses& GetAddresses() const { return _addresses; }

    // Game state
    bool IsGameLoaded() const;
    bool IsInCombat() const;
    bool IsInDialog() const;
    bool IsInInventory() const;

    // Character functions
    std::string GetPlayerName();
    void SetPlayerName(const std::string& name);
    int GetPlayerLevel();
    void SetPlayerLevel(int level);
    
    // World position
    void GetWorldPosition(short& x, short& y);
    void SetWorldPosition(short x, short y);
    
    // Date/Time
    void GetGameDate(short& year, char& month, char& day);
    void SetGameDate(short year, char month, char day);

    // Combat functions
    int ComputeDamage(const AttackData& attack, const DamageComputation& computation);
    void ModifyDamageComputation(const std::function<void(DamageComputation&)>& modifier);

    // Game state modification
    void TogglePipBoy(bool enable);
    void ToggleMovies(bool enable);
    void SetStartingMap(const std::string& mapName);
    void SetModName(const std::string& modName);

private:
    GameManager() = default;
    ~GameManager() = default;
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    GameVersion _version = GameVersion::Unknown;
    GameAddresses _addresses = {};
    bool _initialized = false;

    // Internal helper functions
    bool DetectVersion102();
    bool DetectVersion102d();
    void InitializeAddresses();
    bool ValidateAddresses();
};

} // namespace Game
} // namespace F2SE 