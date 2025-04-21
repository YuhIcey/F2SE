#include "EventManager.h"
#include <algorithm>
#include <iostream>

namespace F2SE::Script {

EventManager& EventManager::GetInstance() {
    static EventManager instance;
    return instance;
}

bool EventManager::Initialize() {
    if (_initialized) {
        return false;
    }
    _initialized = true;
    return true;
}

void EventManager::Shutdown() {
    std::lock_guard<std::mutex> lock(_eventMutex);
    _eventHandlers.clear();
    while (!_eventQueue.empty()) {
        _eventQueue.pop();
    }
    _initialized = false;
}

void EventManager::Update() {
    if (!_initialized) {
        return;
    }
    ProcessQueuedEvents();
}

void EventManager::RegisterEventHandler(GameEventType type, EventHandler handler) {
    std::lock_guard<std::mutex> lock(_eventMutex);
    _eventHandlers[type].push_back(handler);
}

void EventManager::UnregisterEventHandler(GameEventType type, size_t handlerId) {
    std::lock_guard<std::mutex> lock(_eventMutex);
    auto it = _eventHandlers.find(type);
    if (it != _eventHandlers.end() && handlerId < it->second.size()) {
        it->second.erase(it->second.begin() + handlerId);
    }
}

void EventManager::TriggerGameEvent(GameEventType type, const EventData& data) {
    std::vector<EventHandler> handlers;
    {
        std::lock_guard<std::mutex> lock(_eventMutex);
        auto it = _eventHandlers.find(type);
        if (it != _eventHandlers.end()) {
            handlers = it->second;
        }
    }
    
    for (const auto& handler : handlers) {
        try {
            handler(type, data);
        } catch (const std::exception& e) {
            std::cerr << "Error in event handler for event type " 
                     << static_cast<int>(type) << ": " << e.what() << std::endl;
        }
    }
}

void EventManager::QueueGameEvent(GameEventType type, const EventData& data, float delay) {
    std::lock_guard<std::mutex> lock(_eventMutex);
    QueuedEvent event{type, "", data, delay};
    _eventQueue.push(event);
}

bool EventManager::HasEventHandler(GameEventType type) const {
    std::lock_guard<std::mutex> lock(_eventMutex);
    auto it = _eventHandlers.find(type);
    return it != _eventHandlers.end() && !it->second.empty();
}

size_t EventManager::GetHandlerCount(GameEventType type) const {
    std::lock_guard<std::mutex> lock(_eventMutex);
    auto it = _eventHandlers.find(type);
    return it != _eventHandlers.end() ? it->second.size() : 0;
}

void EventManager::ProcessQueuedEvents() {
    std::vector<QueuedEvent> eventsToTrigger;
    {
        std::lock_guard<std::mutex> lock(_eventMutex);
        while (!_eventQueue.empty()) {
            auto& event = _eventQueue.front();
            if (event.delay <= 0.0f) {
                eventsToTrigger.push_back(event);
                _eventQueue.pop();
            } else {
                event.delay -= 0.016f; // Assuming 60 FPS for now
                break;
            }
        }
    }

    for (const auto& event : eventsToTrigger) {
        TriggerGameEvent(event.type, event.data);
    }
}

void EventManager::CleanupEvents() {
    std::lock_guard<std::mutex> lock(_eventMutex);
    for (auto it = _eventHandlers.begin(); it != _eventHandlers.end();) {
        if (it->second.empty()) {
            it = _eventHandlers.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace F2SE::Script 