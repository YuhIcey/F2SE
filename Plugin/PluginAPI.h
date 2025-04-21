#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include <memory>

namespace F2SE::Plugin {

class PluginAPI {
public:
    PluginAPI() = default;
    ~PluginAPI() = default;

    // Function registration
    void RegisterFunction(const std::string& name, std::function<void*()> factory);
    void UnregisterFunction(const std::string& name);
    void* GetFunction(const std::string& name) const;

    // API version
    static constexpr int API_VERSION_MAJOR = 1;
    static constexpr int API_VERSION_MINOR = 0;

private:
    std::unordered_map<std::string, std::function<void*()>> _functions;
};

} // namespace F2SE::Plugin 