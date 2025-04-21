#include "ScriptExtender.h"
#include "../Memory/MemoryManager.h"
#include "../Hooks/HookManager.h"
#include "../Plugin/PluginManager.h"
#include "../Script/ScriptManager.h"
#include <iostream>

namespace F2SE {
namespace Core {

ScriptExtender& ScriptExtender::GetInstance() {
    static ScriptExtender instance;
    return instance;
}

bool ScriptExtender::Initialize() {
    std::cout << "F2SE: Initializing Script Extender v" 
              << VERSION_MAJOR << "." 
              << VERSION_MINOR << "." 
              << VERSION_PATCH << std::endl;

    // Initialize subsystems
    if (!Memory::MemoryManager::GetInstance().Initialize()) {
        std::cout << "F2SE: Failed to initialize Memory Manager" << std::endl;
        return false;
    }

    if (!Hooks::HookManager::GetInstance().Initialize()) {
        std::cout << "F2SE: Failed to initialize Hook Manager" << std::endl;
        return false;
    }

    if (!Plugin::PluginManager::GetInstance().Initialize()) {
        std::cout << "F2SE: Failed to initialize Plugin Manager" << std::endl;
        return false;
    }

    if (!Script::ScriptManager::GetInstance().Initialize()) {
        std::cout << "F2SE: Failed to initialize Script Manager" << std::endl;
        return false;
    }

    std::cout << "F2SE: Initialization complete" << std::endl;
    return true;
}

void ScriptExtender::Shutdown() {
    std::cout << "F2SE: Shutting down..." << std::endl;

    // Shutdown in reverse order
    Script::ScriptManager::GetInstance().Shutdown();
    Plugin::PluginManager::GetInstance().Shutdown();
    Hooks::HookManager::GetInstance().Shutdown();
    Memory::MemoryManager::GetInstance().Shutdown();

    std::cout << "F2SE: Shutdown complete" << std::endl;
}

bool ScriptExtender::LoadPlugin(const std::string& pluginPath) {
    return Plugin::PluginManager::GetInstance().LoadPlugin(pluginPath);
}

bool ScriptExtender::RegisterScriptFunction(const std::string& name, ScriptFunction func) {
    return Script::ScriptManager::GetInstance().RegisterFunction(name, func);
}

void* ScriptExtender::GetGameAddress(DWORD offset) {
    return Memory::MemoryManager::GetInstance().GetAddress(offset);
}

bool ScriptExtender::ReadGameMemory(DWORD address, void* buffer, size_t size) {
    return Memory::MemoryManager::GetInstance().ReadMemory(address, buffer, size);
}

bool ScriptExtender::WriteGameMemory(DWORD address, const void* buffer, size_t size) {
    return Memory::MemoryManager::GetInstance().WriteMemory(address, buffer, size);
}

PlayerData* ScriptExtender::GetPlayerData() {
    return Memory::MemoryManager::GetInstance().GetPlayerData();
}

} // namespace Core
} // namespace F2SE 