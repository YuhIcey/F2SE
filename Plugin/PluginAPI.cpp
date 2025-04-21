#include "PluginAPI.h"
#include <iostream>

namespace F2SE::Plugin {

bool PluginAPI::Initialize() {
    if (_initialized) {
        return true;
    }

    std::cout << "Initializing PluginAPI..." << std::endl;
    _initialized = true;
    return true;
}

void PluginAPI::Shutdown() {
    if (!_initialized) {
        return;
    }

    std::cout << "Shutting down PluginAPI..." << std::endl;
    _functions.clear();
    _initialized = false;
}

void PluginAPI::RegisterFunction(const std::string& name, std::function<void*()> factory) {
    if (!_initialized) {
        return;
    }

    _functions[name] = factory;
    std::cout << "Registered API function: " << name << std::endl;
}

void* PluginAPI::GetFunction(const std::string& name) const {
    if (!_initialized) {
        return nullptr;
    }

    auto it = _functions.find(name);
    if (it == _functions.end()) {
        std::cerr << "API function not found: " << name << std::endl;
        return nullptr;
    }

    return it->second();
}

bool PluginAPI::HasFunction(const std::string& name) const {
    return _initialized && _functions.find(name) != _functions.end();
}

void PluginAPI::UnregisterFunction(const std::string& name) {
    if (!_initialized) {
        return;
    }

    auto it = _functions.find(name);
    if (it != _functions.end()) {
        _functions.erase(it);
        std::cout << "Unregistered API function: " << name << std::endl;
    }
}

std::vector<std::string> PluginAPI::GetRegisteredFunctions() const {
    std::vector<std::string> result;
    if (!_initialized) {
        return result;
    }

    result.reserve(_functions.size());
    for (const auto& [name, _] : _functions) {
        result.push_back(name);
    }
    return result;
}

} // namespace F2SE::Plugin 