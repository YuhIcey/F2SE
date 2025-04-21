#include "PluginManager.h"
#include <iostream>
#include <filesystem>
#include <fstream>

namespace F2SE::Plugin {

// Fallout 2 supported file extensions
const std::vector<std::string> SUPPORTED_EXTENSIONS = {
    ".dat",  // Main game data files
    ".frm",  // Frame animation files
    ".pro",  // Proto item/critter files
    ".map",  // Map files
    ".gam",  // Saved game files
    ".acm",  // Sound files
    ".txt",  // Text/script files
    ".msg",  // Message/dialog files
    ".int",  // Interface files
    ".lst",  // List files
    ".gcd",  // Character files
    ".sve",  // Save files
    ".sav"   // Alternative save files
};

PluginManager& PluginManager::GetInstance() {
    static PluginManager instance;
    return instance;
}

bool PluginManager::Initialize() {
    if (_isInitialized) {
        return true;
    }

    try {
        LoadPlugins();
        _isInitialized = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize PluginManager: " << e.what() << std::endl;
        return false;
    }
}

void PluginManager::Shutdown() {
    if (!_isInitialized) {
        return;
    }

    for (auto& plugin : _plugins) {
        plugin->Shutdown();
    }
    _plugins.clear();
    _isInitialized = false;
}

void PluginManager::LoadPlugins() {
    namespace fs = std::filesystem;
    const std::string pluginDir = "plugins";

    if (!fs::exists(pluginDir)) {
        fs::create_directory(pluginDir);
        return;
    }

    for (const auto& entry : fs::directory_iterator(pluginDir)) {
        if (entry.path().extension() != ".dll") {
            continue;
        }

        try {
            auto plugin = std::make_unique<Plugin>(entry.path().string());
            if (plugin->Initialize()) {
                _plugins.push_back(std::move(plugin));
                std::cout << "Loaded plugin: " << entry.path().filename().string() << std::endl;
            } else {
                std::cerr << "Failed to initialize plugin: " << entry.path().filename().string() << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error loading plugin " << entry.path().filename().string() << ": " << e.what() << std::endl;
        }
    }
}

bool PluginManager::IsInitialized() const {
    return _isInitialized;
}

const std::vector<std::unique_ptr<Plugin>>& PluginManager::GetPlugins() const {
    return _plugins;
}

Plugin* PluginManager::GetPlugin(const std::string& name) const {
    auto it = std::find_if(_plugins.begin(), _plugins.end(),
        [&name](const auto& plugin) { return plugin->GetName() == name; });
    
    return it != _plugins.end() ? it->get() : nullptr;
}

void PluginManager::Update(float deltaTime) {
    if (!_initialized) {
        return;
    }

    // Update all active plugins
    for (auto& [name, data] : _plugins) {
        if (data.state == PluginState::Active && data.instance) {
            data.instance->OnUpdate(deltaTime);
        }
    }
}

bool PluginManager::LoadPlugin(const std::filesystem::path& path) {
    if (!_initialized) {
        return false;
    }

    // Check if plugin is already loaded
    for (const auto& [name, data] : _plugins) {
        if (data.path == path.string()) {
            return true;
        }
    }

    PluginData data;
    data.path = path.string();
    data.state = PluginState::Loading;

    // Load plugin library
    if (!LoadPluginLibrary(path, data)) {
        std::cerr << "Failed to load plugin: " << path << std::endl;
        return false;
    }

    // Validate plugin
    if (!ValidatePlugin(data.info)) {
        UnloadPluginLibrary(data);
        return false;
    }

    // Initialize plugin
    if (!data.instance->Initialize()) {
        UnloadPluginLibrary(data);
        return false;
    }

    data.state = PluginState::Active;
    _plugins[data.info.name] = std::move(data);

    std::cout << "Loaded plugin: " << data.info.name << " v" << data.info.version.ToString() << std::endl;
    return true;
}

bool PluginManager::UnloadPlugin(const std::string& name) {
    auto it = _plugins.find(name);
    if (it == _plugins.end()) {
        return false;
    }

    auto& data = it->second;
    if (data.instance) {
        data.instance->Shutdown();
    }

    UnloadPluginLibrary(data);
    _plugins.erase(it);

    std::cout << "Unloaded plugin: " << name << std::endl;
    return true;
}

bool PluginManager::ReloadPlugin(const std::string& name) {
    auto it = _plugins.find(name);
    if (it == _plugins.end()) {
        return false;
    }

    std::string path = it->second.path;
    UnloadPlugin(name);
    return LoadPlugin(std::filesystem::path(path));
}

void PluginManager::UnloadAllPlugins() {
    std::vector<std::string> pluginNames;
    for (const auto& [name, data] : _plugins) {
        pluginNames.push_back(name);
    }

    for (const auto& name : pluginNames) {
        UnloadPlugin(name);
    }
}

bool PluginManager::IsPluginLoaded(const std::string& name) const {
    return _plugins.find(name) != _plugins.end();
}

PluginState PluginManager::GetPluginState(const std::string& name) const {
    auto it = _plugins.find(name);
    return it != _plugins.end() ? it->second.state : PluginState::Unloaded;
}

const PluginInfo* PluginManager::GetPluginInfo(const std::string& name) const {
    auto it = _plugins.find(name);
    return it != _plugins.end() ? &it->second.info : nullptr;
}

std::vector<std::string> PluginManager::GetLoadedPlugins() const {
    std::vector<std::string> result;
    result.reserve(_plugins.size());
    for (const auto& [name, data] : _plugins) {
        result.push_back(name);
    }
    return result;
}

std::vector<std::string> PluginManager::GetPluginsInState(PluginState state) const {
    std::vector<std::string> result;
    for (const auto& [name, data] : _plugins) {
        if (data.state == state) {
            result.push_back(name);
        }
    }
    return result;
}

bool PluginManager::EnablePlugin(const std::string& name) {
    auto it = _plugins.find(name);
    if (it == _plugins.end() || it->second.state == PluginState::Active) {
        return false;
    }

    auto& data = it->second;
    if (!data.instance->Initialize()) {
        return false;
    }

    data.state = PluginState::Active;
    return true;
}

bool PluginManager::DisablePlugin(const std::string& name) {
    auto it = _plugins.find(name);
    if (it == _plugins.end() || it->second.state != PluginState::Active) {
        return false;
    }

    auto& data = it->second;
    data.instance->Shutdown();
    data.state = PluginState::Disabled;
    return true;
}

bool PluginManager::SetPluginLoadOrder(const std::string& name, int order) {
    auto it = _plugins.find(name);
    if (it == _plugins.end()) {
        return false;
    }

    it->second.loadOrder = order;
    return true;
}

int PluginManager::GetPluginLoadOrder(const std::string& name) const {
    auto it = _plugins.find(name);
    return it != _plugins.end() ? it->second.loadOrder : -1;
}

PluginAPI* PluginManager::GetPluginAPI() const {
    return _api.get();
}

void PluginManager::RegisterAPIFunction(const std::string& name, std::function<void*()> factory) {
    if (_api) {
        _api->RegisterFunction(name, factory);
    }
}

void* PluginManager::GetAPIFunction(const std::string& name) const {
    return _api ? _api->GetFunction(name) : nullptr;
}

void PluginManager::SetPluginConfigPath(const std::string& path) {
    _configPath = path;
}

std::string PluginManager::GetPluginConfigPath() const {
    return _configPath;
}

bool PluginManager::SavePluginConfig(const std::string& name) {
    auto it = _plugins.find(name);
    if (it == _plugins.end()) {
        return false;
    }

    std::string configPath = _configPath + "/Plugins/" + name + ".cfg";
    std::ofstream file(configPath);
    if (!file.is_open()) {
        return false;
    }

    // Save plugin configuration
    const auto& info = it->second.info;
    file << "name=" << info.name << "\n";
    file << "version=" << info.version.ToString() << "\n";
    file << "state=" << static_cast<int>(it->second.state) << "\n";
    file << "loadOrder=" << it->second.loadOrder << "\n";

    return true;
}

bool PluginManager::LoadPluginConfig(const std::string& name) {
    std::string configPath = _configPath + "/Plugins/" + name + ".cfg";
    std::ifstream file(configPath);
    if (!file.is_open()) {
        return false;
    }

    // Load plugin configuration
    std::string line;
    while (std::getline(file, line)) {
        // Parse configuration lines
        // This is a simplified version - you would need to implement proper parsing
    }

    return true;
}

bool PluginManager::CheckDependencies(const std::string& name) const {
    auto it = _plugins.find(name);
    if (it == _plugins.end()) {
        return false;
    }

    const auto& dependencies = it->second.info.dependencies;
    for (const auto& dep : dependencies) {
        if (!IsPluginLoaded(dep)) {
            return false;
        }
    }

    return true;
}

bool PluginManager::ResolveDependencies(const std::string& name) {
    auto it = _plugins.find(name);
    if (it == _plugins.end()) {
        return false;
    }

    const auto& dependencies = it->second.info.dependencies;
    for (const auto& dep : dependencies) {
        if (!IsPluginLoaded(dep)) {
            // Try to load the dependency
            std::filesystem::path depPath = std::filesystem::path(_configPath) / "Plugins" / (dep + ".dll");
            if (!LoadPlugin(depPath)) {
                return false;
            }
        }
    }

    return true;
}

std::vector<std::string> PluginManager::GetMissingDependencies(const std::string& name) const {
    std::vector<std::string> missing;
    
    auto it = _plugins.find(name);
    if (it == _plugins.end()) {
        return missing;
    }

    const auto& dependencies = it->second.info.dependencies;
    for (const auto& dep : dependencies) {
        if (!IsPluginLoaded(dep)) {
            missing.push_back(dep);
        }
    }

    return missing;
}

bool PluginManager::RegisterAssetDirectory(const std::string& plugin, const std::string& path) {
    auto it = _plugins.find(plugin);
    if (it == _plugins.end()) {
        return false;
    }

    it->second.assetDirs.push_back(path);
    return true;
}

std::string PluginManager::GetAssetPath(const std::string& plugin, const std::string& asset) const {
    auto it = _plugins.find(plugin);
    if (it == _plugins.end()) {
        return "";
    }

    for (const auto& dir : it->second.assetDirs) {
        std::filesystem::path assetPath = std::filesystem::path(dir) / asset;
        if (std::filesystem::exists(assetPath)) {
            return assetPath.string();
        }
    }

    return "";
}

bool PluginManager::HasAsset(const std::string& plugin, const std::string& asset) const {
    return !GetAssetPath(plugin, asset).empty();
}

bool PluginManager::LoadPluginScripts(const std::string& name) {
    auto it = _plugins.find(name);
    if (it == _plugins.end() || !it->second.info.hasScripts) {
        return false;
    }

    // Load scripts from the plugin's script directory
    std::filesystem::path scriptPath = std::filesystem::path(_configPath) / "Scripts" / name;
    if (!std::filesystem::exists(scriptPath)) {
        return false;
    }

    // Implement script loading logic here
    return true;
}

bool PluginManager::UnloadPluginScripts(const std::string& name) {
    auto it = _plugins.find(name);
    if (it == _plugins.end() || !it->second.info.hasScripts) {
        return false;
    }

    // Implement script unloading logic here
    return true;
}

bool PluginManager::ReloadPluginScripts(const std::string& name) {
    if (!UnloadPluginScripts(name)) {
        return false;
    }
    return LoadPluginScripts(name);
}

bool PluginManager::ValidatePlugin(const PluginInfo& info) const {
    // Check plugin name
    if (info.name.empty()) {
        std::cerr << "Plugin validation failed: Empty name" << std::endl;
        return false;
    }

    // Check version compatibility
    if (info.version.major == 0 && info.version.minor == 0) {
        std::cerr << "Plugin validation failed: Invalid version" << std::endl;
        return false;
    }

    // Check F2SE version compatibility
    // You would need to implement proper version comparison here

    return true;
}

bool PluginManager::LoadPluginLibrary(const std::filesystem::path& path, PluginData& data) {
    if (!std::filesystem::exists(path)) {
        std::cerr << "Plugin file not found: " << path << std::endl;
        return false;
    }

    // Load the plugin DLL and create the plugin instance
    // This is a placeholder - you would need to implement proper DLL loading
    
    return true;
}

void PluginManager::UnloadPluginLibrary(PluginData& data) {
    // Unload the plugin DLL
    // This is a placeholder - you would need to implement proper DLL unloading
    data.instance.reset();
}

void PluginManager::SortPluginsByLoadOrder() {
    // Sort plugins based on their load order
    // This would be called when loading plugins or changing load order
}

void PluginManager::NotifyPluginEvent(const std::string& event, const std::string& plugin) {
    // Notify plugins of system events
    // This would be called for various system events that plugins might want to handle
}

} // namespace F2SE::Plugin 