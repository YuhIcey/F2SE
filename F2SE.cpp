#include <windows.h>
#include <detours.h>
#include <iostream>
#include <vector>
#include <string>

// Base address of Fallout2.exe (you'll need to adjust this based on your version)
#define FALLOUT2_BASE 0x00400000

// Function pointers for original functions we'll hook
typedef void (__stdcall *tMainLoop)(void);
typedef int (__stdcall *tScriptProc)(void* script, int param);
typedef void (__stdcall *tGameInit)(void);

// Original function pointers
tMainLoop orig_MainLoop = nullptr;
tScriptProc orig_ScriptProc = nullptr;
tGameInit orig_GameInit = nullptr;

// Global state
bool g_gameInitialized = false;
std::vector<std::string> loadedPlugins;

// Hooked functions
void __stdcall Hook_MainLoop() {
    // Pre main loop processing
    if (g_gameInitialized) {
        // Process any queued script commands
        // Update extended memory regions
        // Process plugin updates
    }

    // Call original main loop
    orig_MainLoop();

    // Post main loop processing
    // Update UI overlays
    // Process delayed scripts
}

int __stdcall Hook_ScriptProc(void* script, int param) {
    // Pre script processing
    // Add new script commands
    // Process script extensions

    int result = orig_ScriptProc(script, param);

    // Post script processing
    // Handle custom return values
    // Process script callbacks

    return result;
}

void __stdcall Hook_GameInit() {
    // Pre game init
    std::cout << "F2SE: Initializing..." << std::endl;

    // Call original init
    orig_GameInit();

    // Post game init
    g_gameInitialized = true;
    std::cout << "F2SE: Initialization complete" << std::endl;
}

// Plugin system
struct Plugin {
    HMODULE handle;
    std::string name;
    bool (__cdecl *Initialize)(void);
    void (__cdecl *Shutdown)(void);
};

std::vector<Plugin> plugins;

bool LoadPlugin(const char* pluginPath) {
    Plugin plugin = {};
    plugin.handle = LoadLibraryA(pluginPath);
    
    if (!plugin.handle) {
        std::cout << "F2SE: Failed to load plugin: " << pluginPath << std::endl;
        return false;
    }

    plugin.Initialize = (bool (__cdecl *)(void))GetProcAddress(plugin.handle, "Initialize");
    plugin.Shutdown = (void (__cdecl *)(void))GetProcAddress(plugin.handle, "Shutdown");
    
    if (!plugin.Initialize || !plugin.Shutdown) {
        std::cout << "F2SE: Plugin missing required functions: " << pluginPath << std::endl;
        FreeLibrary(plugin.handle);
        return false;
    }

    if (!plugin.Initialize()) {
        std::cout << "F2SE: Plugin initialization failed: " << pluginPath << std::endl;
        FreeLibrary(plugin.handle);
        return false;
    }

    plugin.name = pluginPath;
    plugins.push_back(plugin);
    std::cout << "F2SE: Loaded plugin: " << pluginPath << std::endl;
    return true;
}

void UnloadPlugins() {
    for (auto& plugin : plugins) {
        if (plugin.Shutdown) {
            plugin.Shutdown();
        }
        FreeLibrary(plugin.handle);
    }
    plugins.clear();
}

// Memory utilities
void* GetFallout2Function(DWORD offset) {
    return (void*)(FALLOUT2_BASE + offset);
}

// Main DLL entry point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    switch (reason) {
        case DLL_PROCESS_ATTACH: {
            DisableThreadLibraryCalls(hModule);

            // Create console for debug output
            AllocConsole();
            FILE* dummy;
            freopen_s(&dummy, "CONOUT$", "w", stdout);

            std::cout << "F2SE: Initializing Script Extender..." << std::endl;

            // Get original function addresses
            orig_MainLoop = (tMainLoop)GetFallout2Function(0x1234);  // Replace with actual offset
            orig_ScriptProc = (tScriptProc)GetFallout2Function(0x5678);  // Replace with actual offset
            orig_GameInit = (tGameInit)GetFallout2Function(0x9ABC);  // Replace with actual offset

            // Install hooks
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            
            DetourAttach(&(PVOID&)orig_MainLoop, Hook_MainLoop);
            DetourAttach(&(PVOID&)orig_ScriptProc, Hook_ScriptProc);
            DetourAttach(&(PVOID&)orig_GameInit, Hook_GameInit);
            
            LONG error = DetourTransactionCommit();
            if (error != NO_ERROR) {
                std::cout << "F2SE: Failed to install hooks" << std::endl;
                return FALSE;
            }

            std::cout << "F2SE: Hooks installed successfully" << std::endl;

            // Load plugins
            LoadPlugin("plugins/example.dll");
            break;
        }
        
        case DLL_PROCESS_DETACH: {
            if (lpReserved != nullptr) {
                break;
            }

            // Uninstall hooks
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            
            DetourDetach(&(PVOID&)orig_MainLoop, Hook_MainLoop);
            DetourDetach(&(PVOID&)orig_ScriptProc, Hook_ScriptProc);
            DetourDetach(&(PVOID&)orig_GameInit, Hook_GameInit);
            
            DetourTransactionCommit();

            // Unload plugins
            UnloadPlugins();

            // Clean up console
            FreeConsole();
            break;
        }
    }
    return TRUE;
} 