#pragma once

#include <cstdint>
#include <vector>
#include <memory>

namespace F2SE::Game {

class Actor;

enum class CombatMode {
    None,
    Turn,
    RealTime
};

struct CombatAction {
    uint32_t actorId;
    uint32_t targetId;
    uint32_t actionType;
    uint32_t weaponId;
    int32_t actionPoints;
};

class CombatSystem {
public:
    CombatSystem() = default;
    ~CombatSystem() = default;

    bool Initialize();
    void Shutdown();
    void Update(float deltaTime);

    // Combat state
    bool IsInCombat() const { return _inCombat; }
    CombatMode GetCombatMode() const { return _mode; }
    void SetCombatMode(CombatMode mode);

    // Combat flow
    bool StartCombat(uint32_t initiatorId);
    void EndCombat();
    bool IsActorTurn(uint32_t actorId) const;
    void NextTurn();

    // Actions
    bool CanPerformAction(uint32_t actorId, const CombatAction& action) const;
    bool PerformAction(const CombatAction& action);
    void CancelAction(uint32_t actorId);

    // Combat participants
    void AddCombatant(uint32_t actorId);
    void RemoveCombatant(uint32_t actorId);
    bool IsCombatant(uint32_t actorId) const;
    std::vector<uint32_t> GetCombatants() const;

    // Combat stats
    int32_t GetActionPoints(uint32_t actorId) const;
    void SetActionPoints(uint32_t actorId, int32_t points);
    int32_t GetInitiative(uint32_t actorId) const;
    void SetInitiative(uint32_t actorId, int32_t initiative);

private:
    struct CombatantData {
        uint32_t actorId;
        int32_t actionPoints;
        int32_t initiative;
        bool hasActed;
    };

    bool _inCombat{false};
    CombatMode _mode{CombatMode::None};
    uint32_t _currentActorIndex{0};
    std::vector<CombatantData> _combatants;
    std::vector<CombatAction> _queuedActions;
};

} // namespace F2SE::Game 