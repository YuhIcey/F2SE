#include "CombatSystem.h"
#include <algorithm>
#include <stdexcept>

namespace F2SE::Game {

bool CombatSystem::Initialize() {
    if (_inCombat) {
        return false;
    }
    _mode = CombatMode::None;
    _currentActorIndex = 0;
    _combatants.clear();
    _queuedActions.clear();
    return true;
}

void CombatSystem::Shutdown() {
    EndCombat();
}

void CombatSystem::Update(float deltaTime) {
    if (!_inCombat) {
        return;
    }

    // Process queued actions
    for (const auto& action : _queuedActions) {
        if (CanPerformAction(action.actorId, action)) {
            PerformAction(action);
        }
    }
    _queuedActions.clear();

    // Auto-advance turns in real-time mode
    if (_mode == CombatMode::RealTime) {
        for (auto& combatant : _combatants) {
            if (!combatant.hasActed) {
                combatant.actionPoints += static_cast<int32_t>(deltaTime * 10.0f);
            }
        }
    }
}

void CombatSystem::SetCombatMode(CombatMode mode) {
    _mode = mode;
    if (mode == CombatMode::Turn) {
        // Reset action points for turn-based mode
        for (auto& combatant : _combatants) {
            combatant.actionPoints = 10;
            combatant.hasActed = false;
        }
    }
}

bool CombatSystem::StartCombat(uint32_t initiatorId) {
    if (_inCombat) {
        return false;
    }

    _inCombat = true;
    _currentActorIndex = 0;
    AddCombatant(initiatorId);
    
    // Sort combatants by initiative
    std::sort(_combatants.begin(), _combatants.end(),
        [](const CombatantData& a, const CombatantData& b) {
            return a.initiative > b.initiative;
        });

    return true;
}

void CombatSystem::EndCombat() {
    _inCombat = false;
    _mode = CombatMode::None;
    _combatants.clear();
    _queuedActions.clear();
}

bool CombatSystem::IsActorTurn(uint32_t actorId) const {
    if (!_inCombat || _combatants.empty()) {
        return false;
    }
    return _combatants[_currentActorIndex].actorId == actorId;
}

void CombatSystem::NextTurn() {
    if (!_inCombat || _combatants.empty()) {
        return;
    }

    _currentActorIndex = (_currentActorIndex + 1) % _combatants.size();
    
    // Reset action points for the new actor
    if (_mode == CombatMode::Turn) {
        _combatants[_currentActorIndex].actionPoints = 10;
        _combatants[_currentActorIndex].hasActed = false;
    }
}

bool CombatSystem::CanPerformAction(uint32_t actorId, const CombatAction& action) const {
    if (!_inCombat) {
        return false;
    }

    auto it = std::find_if(_combatants.begin(), _combatants.end(),
        [actorId](const CombatantData& data) { return data.actorId == actorId; });

    if (it == _combatants.end()) {
        return false;
    }

    return it->actionPoints >= action.actionPoints;
}

bool CombatSystem::PerformAction(const CombatAction& action) {
    if (!CanPerformAction(action.actorId, action)) {
        return false;
    }

    auto it = std::find_if(_combatants.begin(), _combatants.end(),
        [action](const CombatantData& data) { return data.actorId == action.actorId; });

    it->actionPoints -= action.actionPoints;
    it->hasActed = true;

    if (_mode == CombatMode::Turn && it->actionPoints <= 0) {
        NextTurn();
    }

    return true;
}

void CombatSystem::CancelAction(uint32_t actorId) {
    _queuedActions.erase(
        std::remove_if(_queuedActions.begin(), _queuedActions.end(),
            [actorId](const CombatAction& action) { return action.actorId == actorId; }),
        _queuedActions.end());
}

void CombatSystem::AddCombatant(uint32_t actorId) {
    if (std::find_if(_combatants.begin(), _combatants.end(),
        [actorId](const CombatantData& data) { return data.actorId == actorId; }) != _combatants.end()) {
        return;
    }

    CombatantData data{
        actorId,
        10, // Initial action points
        0,  // Initiative will be set separately
        false
    };
    _combatants.push_back(data);
}

void CombatSystem::RemoveCombatant(uint32_t actorId) {
    _combatants.erase(
        std::remove_if(_combatants.begin(), _combatants.end(),
            [actorId](const CombatantData& data) { return data.actorId == actorId; }),
        _combatants.end());

    if (_combatants.empty()) {
        EndCombat();
    }
}

bool CombatSystem::IsCombatant(uint32_t actorId) const {
    return std::find_if(_combatants.begin(), _combatants.end(),
        [actorId](const CombatantData& data) { return data.actorId == actorId; }) != _combatants.end();
}

std::vector<uint32_t> CombatSystem::GetCombatants() const {
    std::vector<uint32_t> result;
    result.reserve(_combatants.size());
    for (const auto& combatant : _combatants) {
        result.push_back(combatant.actorId);
    }
    return result;
}

int32_t CombatSystem::GetActionPoints(uint32_t actorId) const {
    auto it = std::find_if(_combatants.begin(), _combatants.end(),
        [actorId](const CombatantData& data) { return data.actorId == actorId; });
    return it != _combatants.end() ? it->actionPoints : 0;
}

void CombatSystem::SetActionPoints(uint32_t actorId, int32_t points) {
    auto it = std::find_if(_combatants.begin(), _combatants.end(),
        [actorId](const CombatantData& data) { return data.actorId == actorId; });
    if (it != _combatants.end()) {
        it->actionPoints = points;
    }
}

int32_t CombatSystem::GetInitiative(uint32_t actorId) const {
    auto it = std::find_if(_combatants.begin(), _combatants.end(),
        [actorId](const CombatantData& data) { return data.actorId == actorId; });
    return it != _combatants.end() ? it->initiative : 0;
}

void CombatSystem::SetInitiative(uint32_t actorId, int32_t initiative) {
    auto it = std::find_if(_combatants.begin(), _combatants.end(),
        [actorId](const CombatantData& data) { return data.actorId == actorId; });
    if (it != _combatants.end()) {
        it->initiative = initiative;
    }
}

} // namespace F2SE::Game 