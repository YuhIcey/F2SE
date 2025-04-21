#pragma once

#include <string>
#include <memory>

namespace F2SE::Plugin {

struct Version {
    int major;
    int minor;
    int patch;

    std::string ToString() const {
        return std::to_string(major) + "." + 
               std::to_string(minor) + "." + 
               std::to_string(patch);
    }

    bool operator==(const Version& other) const {
        return major == other.major && 
               minor == other.minor && 
               patch == other.patch;
    }

    bool operator!=(const Version& other) const {
        return !(*this == other);
    }
};

struct PluginInfo {
    std::string name;
    std::string author;
    std::string description;
    Version version;
};

using GetPluginInfoFunc = PluginInfo(*)();
using InitializePluginFunc = bool(*)();
using ShutdownPluginFunc = void(*)();

class Plugin {
public:
    explicit Plugin(const std::string& path);
    ~Plugin();

    // Prevent copying
    Plugin(const Plugin&) = delete;
    Plugin& operator=(const Plugin&) = delete;

    // Allow moving
    Plugin(Plugin&&) = default;
    Plugin& operator=(Plugin&&) = default;

    bool Initialize();
    void Shutdown();

    // Getters
    const std::string& GetPath() const;
    const std::string& GetName() const;
    const Version& GetVersion() const;
    const std::string& GetAuthor() const;
    const std::string& GetDescription() const;
    bool IsInitialized() const;

    // Event handlers
    void OnGameStart();
    void OnGameEnd();
    void OnSave();
    void OnLoad();
    void OnUpdate(float deltaTime);

private:
    void LoadLibrary();

    std::string _path;
    void* _handle;
    bool _isInitialized;
    PluginInfo _info;
};

    // IPlugin interface implementation
    bool Initialize() override;
    void Shutdown() override;
    void OnGameStart() override;
    void OnGameEnd() override;
    void OnSave() override;
    void OnLoad() override;
    void OnUpdate(float deltaTime) override;

    // Plugin-specific methods
    const std::string& GetName() const { return _name; }
    void SetName(const std::string& name) { _name = name; }
    
    const PluginInfo& GetInfo() const { return _info; }
    void SetInfo(const PluginInfo& info) { _info = info; }

    bool IsInitialized() const { return _initialized; }

protected:
    std::string _name;
    PluginInfo _info;
    bool _initialized{false};
};

} // namespace F2SE::Plugin 