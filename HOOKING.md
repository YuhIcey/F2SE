# F2MPx Hooking System Documentation

## Overview

The Fallout 2 Script Extender (F2MPx) uses a sophisticated hooking system to integrate with various game processes and extend their functionality. This document explains the key hooking mechanisms and their purposes.

## Save Game Integration

### Save Process Hook
F2MPx hooks into Fallout 2's save game process at two key points:
1. Pre-Save: Called before the game writes its save data
   - Allows F2MPx to prepare additional data that needs to be saved
   - Can prevent saving if script-extended features are in an unsafe state
   - Triggers the `OnPreSave` event for scripts to handle pre-save cleanup

2. Post-Save: Called after the game has written its save data
   - Writes F2MPx-specific data to a co-save file (.F2MPx)
   - Stores extended script data, custom forms, and plugin state
   - Triggers the `OnPostSave` event for scripts

### Load Process Hook
Similarly, F2MPx hooks the load process:
1. Pre-Load: Called before the game loads save data
   - Validates F2MPx co-save file existence and version compatibility
   - Prepares script engine for state restoration
   - Triggers the `OnPreLoad` event

2. Post-Load: Called after game data is loaded
   - Reads and restores F2MPx-specific data
   - Relinks script references and reconstructs plugin state
   - Triggers the `OnPostLoad` event

### Co-Save File Structure
The F2MPx co-save file (.F2MPx) contains:
```
Header:
- Magic number ('F2MPx')
- Version information
- Save timestamp

Script Data:
- Global variables
- Custom arrays
- Extended form data
- Plugin-specific data

State Information:
- Console history
- Active script states
- Extended reference data
```

## Memory Hooks

### Direct Memory Access
F2MPx uses several methods to access and modify game memory:
1. **Virtual Memory Hooks**
   - Patches specific memory addresses to redirect execution
   - Uses trampolines for safe function interception
   - Preserves original code for unhooking

2. **IAT Hooking**
   - Intercepts Windows API calls
   - Allows monitoring of file I/O and resource loading
   - Enables custom asset loading

### Critical Hook Points

1. **Main Game Loop**
   - Enables script execution during frame updates
   - Allows for custom rendering and UI updates
   - Monitors game state changes

2. **Dialog System**
   - Extends dialog options and responses
   - Allows script-driven dialog choices
   - Enables custom dialog UI elements

3. **Combat System**
   - Hooks combat calculations
   - Allows for custom damage types and effects
   - Enables script-driven combat events

## Safety and Stability

### Hook Management
- All hooks are version-specific and validated at runtime
- Hooks can be safely disabled/enabled during execution
- Failed hooks are logged and reported to the user

### Error Handling
- Memory access violations are caught and handled
- Corrupt save states are detected and reported
- Automatic backup of save data before modifications

## Plugin Integration

### Plugin Hook Points
Plugins can register for various events:
```cpp
// Example plugin hook registration
void RegisterPluginHooks() {
    F2MPx::HookManager::Register(HookType::PreSave, OnPreSave);
    F2MPx::HookManager::Register(HookType::PostLoad, OnPostLoad);
    F2MPx::HookManager::Register(HookType::GameLoop, OnFrame);
}
```

### Custom Hook Creation
Plugins can define custom hook points:
```cpp
// Example custom hook definition
F2MPx::HookPoint* customHook = F2MPx::HookManager::CreateHook(
    "MyPlugin::CustomEvent",
    0x004A5B60,  // Memory address
    HookType::Call,
    5  // Hook size
);
```

## Best Practices

1. **Version Checking**
   - Always verify game version before installing hooks
   - Include version information in co-save files
   - Handle version mismatches gracefully

2. **Save Data Safety**
   - Never modify original save files directly
   - Keep backup copies of save data
   - Validate data integrity before and after saves

3. **Memory Safety**
   - Use safe memory access patterns
   - Implement proper error handling
   - Clean up hooks when unloading

4. **Performance**
   - Minimize hook overhead
   - Batch operations when possible
   - Cache frequently accessed data

## Debugging

### Hook Debugging
F2MPx provides several debugging tools:
```cpp
// Enable hook debugging
F2MPx::HookManager::SetDebugMode(true);

// Log hook events
F2MPx::HookManager::LogHookEvent(
    "SaveGame",
    "Pre-save hook executed",
    LogLevel::Debug
);
```

### Memory Validation
Tools for validating memory operations:
```cpp
// Validate memory region
bool isValid = F2MPx::Memory::ValidateAddress(
    address,
    size,
    MemoryFlags::Read | MemoryFlags::Write
);
```

## Extended Features

### Input System Extension
F2MPx enhances the game's input system:
```cpp
// Register custom key bindings
F2MPx::InputManager::RegisterHotkey(
    "MyMod::SpecialAction",
    VK_F5,
    [](){ /* Custom action */ }
);

// Extended input events
F2MPx::InputManager::RegisterInputHandler(
    InputEventType::MouseWheel,
    [](const InputEvent& e) {
        // Handle mouse wheel for custom UI
    }
);
```

### UI Extensions
1. **Custom Menu System**
   - Create new menu interfaces
   - Extend existing game menus
   - Add custom HUD elements
   ```cpp
   // Create custom menu
   auto menu = F2MPx::UI::CreateMenu("MyMod::CustomMenu");
   menu->AddElement(
       "Button",
       {x: 100, y: 100, width: 200, height: 30},
       "Click Me"
   );
   ```

2. **Inventory Extensions**
   - Custom item categories
   - Extended item properties
   - Dynamic item modifications
   ```cpp
   // Add custom item property
   F2MPx::Items::RegisterProperty(
       "CustomDurability",
       PropertyType::Float,
       100.0f  // default value
   );
   ```

### Script Extensions

1. **Extended Script Functions**
   ```cpp
   // Register custom script function
   F2MPx::ScriptEngine::RegisterFunction(
       "GetCustomStat",
       [](ScriptContext& ctx) {
           auto statId = ctx.GetArg<int>(0);
           ctx.SetResult(GetStatValue(statId));
       }
   );
   ```

2. **Event System**
   ```cpp
   // Register custom event
   F2MPx::Events::Register(
       "OnCustomAction",
       [](const EventArgs& args) {
           // Handle custom event
       }
   );
   ```

### World Interaction

1. **Map Extensions**
   - Custom map markers
   - Extended location data
   - Dynamic world events
   ```cpp
   // Add custom map marker
   F2MPx::WorldMap::AddMarker(
       {x: 1500, y: 2000},
       "CustomLocation",
       MarkerType::Quest
   );
   ```

2. **NPC AI Enhancement**
   - Custom AI packages
   - Extended behavior trees
   - Dynamic NPC scheduling
   ```cpp
   // Register custom AI package
   F2MPx::AI::RegisterPackage(
       "CustomBehavior",
       [](NPC* npc) {
           // Implement custom behavior
       }
   );
   ```

### Multiplayer Support

1. **Network Synchronization**
   - State synchronization
   - Event broadcasting
   - Player management
   ```cpp
   // Broadcast custom event
   F2MPx::Network::Broadcast(
       "PlayerAction",
       {
           {"playerId", playerId},
           {"action", actionType},
           {"data", customData}
       }
   );
   ```

2. **Multiplayer Scripts**
   - Synchronized script execution
   - Network-aware functions
   - Player interaction handlers
   ```cpp
   // Register multiplayer-aware function
   F2MPx::MP::RegisterFunction(
       "SyncedAction",
       [](const NetworkContext& ctx) {
           // Handle synchronized action
       }
   );
   ```

### Graphics Extensions

1. **Shader System**
   - Custom shader effects
   - Post-processing
   - Dynamic lighting
   ```cpp
   // Register custom shader
   F2MPx::Graphics::RegisterShader(
       "CustomEffect",
       "shaders/custom.hlsl",
       ShaderType::PostProcess
   );
   ```

2. **Model Management**
   - Custom model loading
   - Animation extensions
   - Dynamic mesh modification
   ```cpp
   // Load custom model
   auto model = F2MPx::Models::Load(
       "models/custom.nif",
       ModelFlags::Dynamic | ModelFlags::Animated
   );
   ```

### Debug Tools

1. **Extended Console Commands**
   ```cpp
   // Register debug command
   F2MPx::Console::RegisterCommand(
       "ShowCustomDebug",
       [](const CommandArgs& args) {
           // Display debug information
       }
   );
   ```

2. **Performance Monitoring**
   ```cpp
   // Monitor performance
   F2MPx::Debug::StartPerfCounter("CustomFeature");
   // ... code to monitor ...
   auto metrics = F2MPx::Debug::StopPerfCounter("CustomFeature");
   ```

### Mod Management

1. **Plugin Dependencies**
   ```cpp
   // Define plugin requirements
   F2MPx::Plugins::RequirePlugin(
       "RequiredMod.esp",
       "1.2.0",
       VersionCompare::GreaterOrEqual
   );
   ```

2. **Resource Management**
   ```cpp
   // Register custom resource handler
   F2MPx::Resources::RegisterHandler(
       ".custom",
       [](const std::string& path) {
           // Handle custom resource loading
       }
   );
   ```

### Configuration System

1. **INI Settings**
   ```cpp
   // Register custom settings
   F2MPx::Config::RegisterSetting(
       "MyMod",
       "CustomFeature",
       true,  // default value
       "Enables custom feature"
   );
   ```

2. **User Preferences**
   ```cpp
   // Handle user preferences
   F2MPx::Preferences::Register(
       "MyMod::Settings",
       [](json& settings) {
           // Load/save user preferences
       }
   );
   ``` 