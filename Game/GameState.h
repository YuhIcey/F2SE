#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace F2SE {
namespace Game {

// Forward declarations
class Player;
class WorldMap;
class LocalMap;
class Inventory;
class QuestSystem;
class DialogSystem;
class CombatSystem;

struct GameTime {
    uint32_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
};

class GameState {
public:
    static GameState& GetInstance();

    bool Initialize();
    void Shutdown();
    void Update(float deltaTime);

    // Game time management
    GameTime GetGameTime() const;
    void SetGameTime(const GameTime& time);
    void AdvanceTime(uint32_t minutes);
    float GetTimeScale() const;
    void SetTimeScale(float scale);

    // Player state
    Player* GetPlayer() const;
    void SetPlayerLocation(uint32_t worldX, uint32_t worldY);
    void SetPlayerHealth(float health);
    void SetPlayerRadiation(float rads);
    void SetPlayerPoison(float poison);
    
    // World state
    WorldMap* GetWorldMap() const;
    LocalMap* GetCurrentMap() const;
    void LoadMap(uint32_t mapId);
    void TransitionToMap(uint32_t mapId, uint32_t entrance);
    
    // Quest & Dialog
    QuestSystem* GetQuestSystem() const;
    DialogSystem* GetDialogSystem() const;
    void StartDialog(uint32_t npcId);
    void EndDialog();

    // Combat
    CombatSystem* GetCombatSystem() const;
    bool IsInCombat() const;
    void StartCombat(uint32_t targetId);
    void EndCombat();

    // Weather & Environment
    enum class Weather {
        Clear,
        Cloudy,
        Rainy,
        Stormy,
        Sandstorm
    };
    Weather GetCurrentWeather() const;
    void SetWeather(Weather weather);
    float GetRadiationLevel() const;
    void SetRadiationLevel(float level);

    // Game flags & variables
    bool GetGameFlag(const std::string& flag) const;
    void SetGameFlag(const std::string& flag, bool value);
    int32_t GetGameVariable(const std::string& name) const;
    void SetGameVariable(const std::string& name, int32_t value);

    // Extended state management
    template<typename T>
    void SetExtendedState(const std::string& key, const T& value);
    
    template<typename T>
    T GetExtendedState(const std::string& key) const;

    // Save/Load state
    bool SaveState(const std::string& filename);
    bool LoadState(const std::string& filename);

private:
    GameState() = default;
    ~GameState() = default;
    GameState(const GameState&) = delete;
    GameState& operator=(const GameState&) = delete;

    // Internal state
    std::unique_ptr<Player> _player;
    std::unique_ptr<WorldMap> _worldMap;
    std::unique_ptr<LocalMap> _currentMap;
    std::unique_ptr<QuestSystem> _questSystem;
    std::unique_ptr<DialogSystem> _dialogSystem;
    std::unique_ptr<CombatSystem> _combatSystem;

    GameTime _currentTime;
    float _timeScale;
    Weather _currentWeather;
    float _radiationLevel;

    std::unordered_map<std::string, bool> _gameFlags;
    std::unordered_map<std::string, int32_t> _gameVariables;
    std::unordered_map<std::string, std::vector<uint8_t>> _extendedState;

    bool _inCombat;
    bool _inDialog;
    bool _initialized;
};

} // namespace Game
} // namespace F2SE 