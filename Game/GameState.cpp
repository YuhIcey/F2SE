#include "GameState.h"
#include <iostream>

namespace F2SE {
namespace Game {

GameState& GameState::GetInstance() {
    static GameState instance;
    return instance;
}

bool GameState::Initialize() {
    if (_initialized) {
        return true;
    }

    std::cout << "Initializing GameState..." << std::endl;
    
    _currentTime = {2241, 7, 25, 8, 0};  // Default game start time
    _timeScale = 1.0f;
    _currentWeather = Weather::Clear;
    _radiationLevel = 0.0f;
    _inCombat = false;
    _inDialog = false;
    
    _initialized = true;
    std::cout << "GameState initialized" << std::endl;
    return true;
}

void GameState::Shutdown() {
    if (!_initialized) {
        return;
    }

    std::cout << "Shutting down GameState..." << std::endl;
    
    // Clean up systems
    _player.reset();
    _worldMap.reset();
    _currentMap.reset();
    _questSystem.reset();
    _dialogSystem.reset();
    _combatSystem.reset();
    
    _initialized = false;
    std::cout << "GameState shutdown complete" << std::endl;
}

void GameState::Update(float deltaTime) {
    if (!_initialized) {
        return;
    }

    // Update all systems
    if (_player) {
        // Update player state
    }

    if (_currentMap) {
        // Update current map
    }

    if (_inCombat && _combatSystem) {
        // Update combat
    }

    if (_inDialog && _dialogSystem) {
        // Update dialog
    }
}

GameTime GameState::GetGameTime() const {
    return _currentTime;
}

void GameState::SetGameTime(const GameTime& time) {
    _currentTime = time;
}

void GameState::AdvanceTime(uint32_t minutes) {
    _currentTime.minute += minutes;
    
    // Handle time overflow
    while (_currentTime.minute >= 60) {
        _currentTime.minute -= 60;
        _currentTime.hour++;
        
        if (_currentTime.hour >= 24) {
            _currentTime.hour = 0;
            _currentTime.day++;
            
            if (_currentTime.day > 30) {
                _currentTime.day = 1;
                _currentTime.month++;
                
                if (_currentTime.month > 12) {
                    _currentTime.month = 1;
                    _currentTime.year++;
                }
            }
        }
    }
}

float GameState::GetTimeScale() const {
    return _timeScale;
}

void GameState::SetTimeScale(float scale) {
    _timeScale = scale;
}

Player* GameState::GetPlayer() const {
    return _player.get();
}

void GameState::SetPlayerLocation(uint32_t worldX, uint32_t worldY) {
    if (_player) {
        // Update player location
    }
}

WorldMap* GameState::GetWorldMap() const {
    return _worldMap.get();
}

LocalMap* GameState::GetCurrentMap() const {
    return _currentMap.get();
}

void GameState::LoadMap(uint32_t mapId) {
    // Load new map
}

void GameState::TransitionToMap(uint32_t mapId, uint32_t entrance) {
    // Handle map transition
}

QuestSystem* GameState::GetQuestSystem() const {
    return _questSystem.get();
}

DialogSystem* GameState::GetDialogSystem() const {
    return _dialogSystem.get();
}

void GameState::StartDialog(uint32_t npcId) {
    if (_dialogSystem) {
        _inDialog = true;
        // Start dialog
    }
}

void GameState::EndDialog() {
    _inDialog = false;
}

CombatSystem* GameState::GetCombatSystem() const {
    return _combatSystem.get();
}

bool GameState::IsInCombat() const {
    return _inCombat;
}

void GameState::StartCombat(uint32_t targetId) {
    if (_combatSystem) {
        _inCombat = true;
        // Start combat
    }
}

void GameState::EndCombat() {
    _inCombat = false;
}

GameState::Weather GameState::GetCurrentWeather() const {
    return _currentWeather;
}

void GameState::SetWeather(Weather weather) {
    _currentWeather = weather;
}

float GameState::GetRadiationLevel() const {
    return _radiationLevel;
}

void GameState::SetRadiationLevel(float level) {
    _radiationLevel = level;
}

bool GameState::GetGameFlag(const std::string& flag) const {
    auto it = _gameFlags.find(flag);
    return it != _gameFlags.end() ? it->second : false;
}

void GameState::SetGameFlag(const std::string& flag, bool value) {
    _gameFlags[flag] = value;
}

int32_t GameState::GetGameVariable(const std::string& name) const {
    auto it = _gameVariables.find(name);
    return it != _gameVariables.end() ? it->second : 0;
}

void GameState::SetGameVariable(const std::string& name, int32_t value) {
    _gameVariables[name] = value;
}

bool GameState::SaveState(const std::string& filename) {
    // Save game state
    return true;
}

bool GameState::LoadState(const std::string& filename) {
    // Load game state
    return true;
}

} // namespace Game
} // namespace F2SE 