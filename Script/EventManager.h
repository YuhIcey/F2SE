#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <mutex>
#include <queue>
#include <variant>
#include <memory>

namespace F2SE::Script {

// Event data types specific to Fallout 2
using EventData = std::variant<
    int32_t,      // For game integers
    float,        // For coordinates and stats
    std::string,  // For text and names
    void*         // For raw game pointers
>;

// Event types specific to Fallout 2
enum class GameEventType {
    CombatStart,
    CombatEnd,
    ItemPickup,
    ItemDrop,
    NPCDialogStart,
    NPCDialogEnd,
    LocationChange,
    SkillCheck,
    StatCheck,
    QuestUpdate,
    PlayerDamage,
    PlayerHeal,
    GameLoad,
    GameSave,
    ScriptCall
};

struct QueuedEvent {
    GameEventType type;
    std::string eventName;
    EventData data;
    float delay;
};

using EventHandler = std::function<void(GameEventType, const EventData&)>;

class EventManager {
public:
    static EventManager& GetInstance();

    bool Initialize();
    void Shutdown();
    void Update();

    void RegisterEventHandler(GameEventType type, EventHandler handler);
    void UnregisterEventHandler(GameEventType type, size_t handlerId);
    
    void TriggerGameEvent(GameEventType type, const EventData& data);
    void QueueGameEvent(GameEventType type, const EventData& data, float delay = 0.0f);

    bool HasEventHandler(GameEventType type) const;
    size_t GetHandlerCount(GameEventType type) const;

private:
    EventManager() = default;
    ~EventManager() = default;
    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;

    void ProcessQueuedEvents();
    void CleanupEvents();

    bool _initialized{false};
    std::map<GameEventType, std::vector<EventHandler>> _eventHandlers;
    std::queue<QueuedEvent> _eventQueue;
    std::mutex _eventMutex;
};

} // namespace F2SE::Script 