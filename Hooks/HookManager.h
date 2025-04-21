#pragma once

#include "../Common/Types.h"
#include <windows.h>
#include <vector>
#include <unordered_map>
#include <functional>

namespace F2SE {
namespace Hooks {

// Hook types
enum class HookType {
    Function,    // Regular function hook
    IAT,        // Import Address Table hook
    VTable,     // Virtual table hook
    Trampoline  // Trampoline hook
};

// Hook information
struct HookInfo {
    DWORD address;
    void* hook;
    void* original;
    HookType type;
    bool enabled;
};

// Hook callback types
using PreHookCallback = std::function<void(void*)>;
using PostHookCallback = std::function<void(void*)>;

class HookManager {
public:
    static HookManager& GetInstance();

    bool Initialize();
    void Shutdown();

    // Hook management
    bool CreateHook(DWORD address, void* hook, void** original, HookType type = HookType::Function);
    bool RemoveHook(DWORD address);
    bool EnableHook(DWORD address);
    bool DisableHook(DWORD address);

    // Callback management
    bool AddPreHookCallback(DWORD address, PreHookCallback callback);
    bool AddPostHookCallback(DWORD address, PostHookCallback callback);
    bool RemovePreHookCallback(DWORD address);
    bool RemovePostHookCallback(DWORD address);

    // Hook information
    HookInfo* GetHookInfo(DWORD address);
    std::vector<HookInfo*> GetAllHooks();

private:
    HookManager() = default;
    ~HookManager() = default;
    HookManager(const HookManager&) = delete;
    HookManager& operator=(const HookManager&) = delete;

    std::unordered_map<DWORD, HookInfo> _hooks;
    std::unordered_map<DWORD, PreHookCallback> _preCallbacks;
    std::unordered_map<DWORD, PostHookCallback> _postCallbacks;
    bool _initialized;

    bool ValidateHook(DWORD address);
    bool CreateFunctionHook(DWORD address, void* hook, void** original);
    bool CreateIATHook(DWORD address, void* hook, void** original);
    bool CreateVTableHook(DWORD address, void* hook, void** original);
    bool CreateTrampolineHook(DWORD address, void* hook, void** original);
};

} // namespace Hooks
} // namespace F2SE 