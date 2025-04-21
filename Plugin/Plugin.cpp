#include "Plugin.h"
#include <Windows.h>
#include <iostream>

namespace F2SE::Plugin {

Plugin::Plugin(const std::string& path)
    : _path(path), _handle(nullptr), _isInitialized(false) {
    LoadLibrary();
}

Plugin::~Plugin() {
    Shutdown();
}

bool Plugin::Initialize() {
    if (_isInitialized) {
        return true;
    }

    if (!_handle) {
        std::cerr << "Cannot initialize plugin: DLL not loaded" << std::endl;
        return false;
    }

    try {
        // Get plugin info
        auto getInfoFunc = reinterpret_cast<GetPluginInfoFunc>(
            GetProcAddress(static_cast<HMODULE>(_handle), "GetPluginInfo"));
        
        if (!getInfoFunc) {
            std::cerr << "Failed to get GetPluginInfo function" << std::endl;
            return false;
        }

        _info = getInfoFunc();
        
        // Initialize plugin
        auto initFunc = reinterpret_cast<InitializePluginFunc>(
            GetProcAddress(static_cast<HMODULE>(_handle), "InitializePlugin"));
        
        if (!initFunc) {
            std::cerr << "Failed to get InitializePlugin function" << std::endl;
            return false;
        }

        if (!initFunc()) {
            std::cerr << "Plugin initialization failed" << std::endl;
            return false;
        }

        _isInitialized = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error during plugin initialization: " << e.what() << std::endl;
        return false;
    }
}

void Plugin::Shutdown() {
    if (!_isInitialized) {
        return;
    }

    try {
        auto shutdownFunc = reinterpret_cast<ShutdownPluginFunc>(
            GetProcAddress(static_cast<HMODULE>(_handle), "ShutdownPlugin"));
        
        if (shutdownFunc) {
            shutdownFunc();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error during plugin shutdown: " << e.what() << std::endl;
    }

    if (_handle) {
        FreeLibrary(static_cast<HMODULE>(_handle));
        _handle = nullptr;
    }

    _isInitialized = false;
}

void Plugin::LoadLibrary() {
    _handle = LoadLibraryA(_path.c_str());
    if (!_handle) {
        DWORD error = GetLastError();
        std::cerr << "Failed to load plugin DLL: " << _path << " (Error: " << error << ")" << std::endl;
    }
}

const std::string& Plugin::GetPath() const {
    return _path;
}

const std::string& Plugin::GetName() const {
    return _info.name;
}

const Version& Plugin::GetVersion() const {
    return _info.version;
}

const std::string& Plugin::GetAuthor() const {
    return _info.author;
}

const std::string& Plugin::GetDescription() const {
    return _info.description;
}

bool Plugin::IsInitialized() const {
    return _isInitialized;
}

void Plugin::OnGameStart() {
    if (!_isInitialized) {
        return;
    }
    std::cout << "Game start event for plugin: " << _info.name << std::endl;
}

void Plugin::OnGameEnd() {
    if (!_isInitialized) {
        return;
    }
    std::cout << "Game end event for plugin: " << _info.name << std::endl;
}

void Plugin::OnSave() {
    if (!_isInitialized) {
        return;
    }
    std::cout << "Save event for plugin: " << _info.name << std::endl;
}

void Plugin::OnLoad() {
    if (!_isInitialized) {
        return;
    }
    std::cout << "Load event for plugin: " << _info.name << std::endl;
}

void Plugin::OnUpdate(float deltaTime) {
    if (!_isInitialized) {
        return;
    }
    // Regular update logic here
}

} // namespace F2SE::Plugin 