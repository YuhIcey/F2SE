#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <filesystem>

namespace F2SE::Plugin {

// Forward declarations
class Plugin;
class PluginAPI;

// Version information
struct Version {
    uint16_t major;
    uint16_t minor;
    uint16_t patch;
    std::string build;

    bool operator<(const Version& other) const;
    bool operator==(const Version& other) const;
    std::string ToString() const;
};

// Plugin metadata
struct PluginInfo {
    std::string name;
    std::string author;
    std::string description;
    Version version;
    Version minF2SEVersion;
    std::vector<std::string> dependencies;
    std::vector<std::string> incompatibilities;
    bool hasScripts;
    bool hasAssets;
    bool requiresNewGame;
};

// Plugin states
enum class PluginState {
    Unloaded,
    Loading,
    Active,
    Error,
    Disabled
};

// Plugin interface
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    virtual void OnGameStart() = 0;
    virtual void OnGameEnd() = 0;
    virtual void OnSave() = 0;
    virtual void OnLoad() = 0;
    virtual void OnUpdate(float deltaTime) = 0;
};

class PluginManager {
public:
    static PluginManager& GetInstance();

    bool Initialize();
    void Shutdown();
    void Update(float deltaTime);

    // Plugin loading
    bool LoadPlugin(const std::filesystem::path& path);
    bool UnloadPlugin(const std::string& name);
    bool ReloadPlugin(const std::string& name);
    void UnloadAllPlugins();

    // Plugin queries
    bool IsPluginLoaded(const std::string& name) const;
    PluginState GetPluginState(const std::string& name) const;
    const PluginInfo* GetPluginInfo(const std::string& name) const;
    std::vector<std::string> GetLoadedPlugins() const;
    std::vector<std::string> GetPluginsInState(PluginState state) const;

    // Plugin management
    bool EnablePlugin(const std::string& name);
    bool DisablePlugin(const std::string& name);
    bool SetPluginLoadOrder(const std::string& name, int order);
    int GetPluginLoadOrder(const std::string& name) const;

    // Plugin data
    template<typename T>
    T* GetPluginData(const std::string& name) const;
    
    template<typename T>
    void SetPluginData(const std::string& name, std::unique_ptr<T> data);

    // Plugin API
    PluginAPI* GetPluginAPI() const;
    void RegisterAPIFunction(const std::string& name, std::function<void*()> factory);
    void* GetAPIFunction(const std::string& name) const;

    // Plugin events
    using PluginEventCallback = std::function<void(const std::string&)>;
    void AddPluginEventHandler(const std::string& event, PluginEventCallback callback);
    void RemovePluginEventHandler(const std::string& event);

    // Plugin configuration
    void SetPluginConfigPath(const std::string& path);
    std::string GetPluginConfigPath() const;
    bool SavePluginConfig(const std::string& name);
    bool LoadPluginConfig(const std::string& name);

    // Dependency management
    bool CheckDependencies(const std::string& name) const;
    bool ResolveDependencies(const std::string& name);
    std::vector<std::string> GetMissingDependencies(const std::string& name) const;

    // Asset management
    bool RegisterAssetDirectory(const std::string& plugin, const std::string& path);
    std::string GetAssetPath(const std::string& plugin, const std::string& asset) const;
    bool HasAsset(const std::string& plugin, const std::string& asset) const;

    // Script management
    bool LoadPluginScripts(const std::string& name);
    bool UnloadPluginScripts(const std::string& name);
    bool ReloadPluginScripts(const std::string& name);

private:
    PluginManager() = default;
    ~PluginManager() = default;
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;

    struct PluginData {
        std::unique_ptr<IPlugin> instance;
        PluginInfo info;
        PluginState state;
        int loadOrder;
        std::string path;
        std::vector<std::string> assetDirs;
        std::unordered_map<std::string, std::unique_ptr<void, void(*)(void*)>> data;
    };

    std::unordered_map<std::string, PluginData> _plugins;
    std::unique_ptr<PluginAPI> _api;
    std::string _configPath;
    bool _initialized{false};

    // Internal methods
    bool ValidatePlugin(const PluginInfo& info) const;
    bool LoadPluginLibrary(const std::filesystem::path& path, PluginData& data);
    void UnloadPluginLibrary(PluginData& data);
    void SortPluginsByLoadOrder();
    void NotifyPluginEvent(const std::string& event, const std::string& plugin);
};

} // namespace F2SE::Plugin 