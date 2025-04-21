#pragma once

#include "../Common/Types.h"
#include <string>
#include <windows.h>

namespace F2SE {
namespace Core {

class ScriptExtender {
public:
    static ScriptExtender& GetInstance();

    bool Initialize();
    void Shutdown();

    // Plugin management
    bool LoadPlugin(const std::string& pluginPath);

    // Script functions
    bool RegisterScriptFunction(const std::string& name, ScriptFunction func);

    // Memory access
    void* GetGameAddress(DWORD offset);
    bool ReadGameMemory(DWORD address, void* buffer, size_t size);
    bool WriteGameMemory(DWORD address, const void* buffer, size_t size);
    PlayerData* GetPlayerData();

private:
    ScriptExtender() = default;
    ~ScriptExtender() = default;
    ScriptExtender(const ScriptExtender&) = delete;
    ScriptExtender& operator=(const ScriptExtender&) = delete;
};

} // namespace Core
} // namespace F2SE 