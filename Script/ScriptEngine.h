#pragma once

#include "../Common/Types.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <functional>

namespace F2SE {
namespace Script {

// Command flags
enum class CommandFlags : DWORD {
    None            = 0,
    RequiresPlayer  = 1 << 0,
    RequiresCombat  = 1 << 1,
    RequiresDialog  = 1 << 2,
    Dangerous       = 1 << 3,
    NoWait         = 1 << 4,
    ForScripts     = 1 << 5,
    ForConsole     = 1 << 6
};

// Command parameter types
enum class ParamType {
    None,
    Number,
    String,
    Form,
    Array,
    Object,
    Reference
};

// Command parameter info
struct ParamInfo {
    ParamType type;
    bool optional;
    std::string name;
    std::string desc;
};

// Command result
struct CommandResult {
    bool success;
    ScriptParam result;
    std::string error;
};

// Command definition
struct CommandInfo {
    std::string name;
    std::string description;
    std::string usage;
    std::vector<ParamInfo> params;
    CommandFlags flags;
    std::function<CommandResult(ScriptContext*)> execute;
};

class ScriptEngine {
public:
    static ScriptEngine& GetInstance();

    bool Initialize();
    void Shutdown();

    // Command registration
    bool RegisterCommand(const CommandInfo& cmd);
    bool UnregisterCommand(const std::string& name);
    CommandInfo* GetCommand(const std::string& name);
    std::vector<CommandInfo*> GetCommands();

    // Script execution
    CommandResult ExecuteCommand(const std::string& name, ScriptContext* context);
    CommandResult ExecuteScript(const std::string& script);
    
    // Variable management
    bool SetGlobalVariable(const std::string& name, const ScriptParam& value);
    bool GetGlobalVariable(const std::string& name, ScriptParam& value);
    
    // Array operations
    bool CreateArray(const std::string& name, const std::vector<ScriptParam>& values);
    bool GetArrayElement(const std::string& name, size_t index, ScriptParam& value);
    bool SetArrayElement(const std::string& name, size_t index, const ScriptParam& value);

    // Function registration
    bool RegisterFunction(const std::string& name, ScriptFunction func);
    bool UnregisterFunction(const std::string& name);

private:
    ScriptEngine() = default;
    ~ScriptEngine() = default;
    ScriptEngine(const ScriptEngine&) = delete;
    ScriptEngine& operator=(const ScriptEngine&) = delete;

    std::unordered_map<std::string, CommandInfo> _commands;
    std::unordered_map<std::string, ScriptParam> _globals;
    std::unordered_map<std::string, std::vector<ScriptParam>> _arrays;
    std::unordered_map<std::string, ScriptFunction> _functions;
    bool _initialized = false;

    // Internal helpers
    bool ValidateParams(const CommandInfo& cmd, ScriptContext* context);
    bool ParseScript(const std::string& script, std::vector<std::string>& commands);
    void RegisterDefaultCommands();
};

// Helper macros for command registration
#define REGISTER_COMMAND(name, desc, usage, flags, ...) \
    RegisterCommand({ name, desc, usage, {__VA_ARGS__}, flags, \
        [](ScriptContext* context) -> CommandResult

#define END_COMMAND })

// Example command registration:
/*
REGISTER_COMMAND("SetPlayerHealth", 
    "Sets the player's health", 
    "SetPlayerHealth <amount>",
    CommandFlags::RequiresPlayer,
    ParamInfo{ParamType::Number, false, "amount", "Health amount"})
{
    int health = context->params[0].intValue;
    // Implementation...
    return {true, {}, ""};
}
END_COMMAND;
*/

} // namespace Script
} // namespace F2SE 