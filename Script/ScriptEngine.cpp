#include "ScriptEngine.h"
#include "../Game/GameManager.h"
#include <iostream>
#include <sstream>

namespace F2SE {
namespace Script {

ScriptEngine& ScriptEngine::GetInstance() {
    static ScriptEngine instance;
    return instance;
}

bool ScriptEngine::Initialize() {
    if (_initialized) {
        return true;
    }

    std::cout << "F2SE: Initializing Script Engine..." << std::endl;

    // Register default commands
    RegisterDefaultCommands();

    _initialized = true;
    std::cout << "F2SE: Script Engine initialized" << std::endl;
    return true;
}

void ScriptEngine::Shutdown() {
    _commands.clear();
    _globals.clear();
    _arrays.clear();
    _functions.clear();
    _initialized = false;
}

bool ScriptEngine::RegisterCommand(const CommandInfo& cmd) {
    if (_commands.find(cmd.name) != _commands.end()) {
        std::cout << "F2SE: Command '" << cmd.name << "' already registered" << std::endl;
        return false;
    }

    _commands[cmd.name] = cmd;
    std::cout << "F2SE: Registered command '" << cmd.name << "'" << std::endl;
    return true;
}

bool ScriptEngine::UnregisterCommand(const std::string& name) {
    auto it = _commands.find(name);
    if (it == _commands.end()) {
        return false;
    }

    _commands.erase(it);
    return true;
}

CommandInfo* ScriptEngine::GetCommand(const std::string& name) {
    auto it = _commands.find(name);
    if (it == _commands.end()) {
        return nullptr;
    }
    return &it->second;
}

std::vector<CommandInfo*> ScriptEngine::GetCommands() {
    std::vector<CommandInfo*> commands;
    commands.reserve(_commands.size());
    for (auto& pair : _commands) {
        commands.push_back(&pair.second);
    }
    return commands;
}

CommandResult ScriptEngine::ExecuteCommand(const std::string& name, ScriptContext* context) {
    auto cmd = GetCommand(name);
    if (!cmd) {
        return {false, {}, "Command not found"};
    }

    // Validate command flags
    auto& game = Game::GameManager::GetInstance();
    if ((static_cast<DWORD>(cmd->flags) & static_cast<DWORD>(CommandFlags::RequiresPlayer)) && 
        !game.IsGameLoaded()) {
        return {false, {}, "Command requires player"};
    }

    if ((static_cast<DWORD>(cmd->flags) & static_cast<DWORD>(CommandFlags::RequiresCombat)) && 
        !game.IsInCombat()) {
        return {false, {}, "Command requires combat"};
    }

    if ((static_cast<DWORD>(cmd->flags) & static_cast<DWORD>(CommandFlags::RequiresDialog)) && 
        !game.IsInDialog()) {
        return {false, {}, "Command requires dialog"};
    }

    // Validate parameters
    if (!ValidateParams(*cmd, context)) {
        return {false, {}, "Invalid parameters"};
    }

    // Execute command
    try {
        return cmd->execute(context);
    }
    catch (const std::exception& e) {
        return {false, {}, std::string("Command execution failed: ") + e.what()};
    }
}

CommandResult ScriptEngine::ExecuteScript(const std::string& script) {
    std::vector<std::string> commands;
    if (!ParseScript(script, commands)) {
        return {false, {}, "Failed to parse script"};
    }

    CommandResult lastResult{true, {}, ""};
    for (const auto& cmd : commands) {
        // Create context for command
        ScriptContext context;
        // TODO: Parse command parameters
        
        lastResult = ExecuteCommand(cmd, &context);
        if (!lastResult.success) {
            break;
        }
    }

    return lastResult;
}

bool ScriptEngine::SetGlobalVariable(const std::string& name, const ScriptParam& value) {
    _globals[name] = value;
    return true;
}

bool ScriptEngine::GetGlobalVariable(const std::string& name, ScriptParam& value) {
    auto it = _globals.find(name);
    if (it == _globals.end()) {
        return false;
    }
    value = it->second;
    return true;
}

bool ScriptEngine::CreateArray(const std::string& name, const std::vector<ScriptParam>& values) {
    _arrays[name] = values;
    return true;
}

bool ScriptEngine::GetArrayElement(const std::string& name, size_t index, ScriptParam& value) {
    auto it = _arrays.find(name);
    if (it == _arrays.end() || index >= it->second.size()) {
        return false;
    }
    value = it->second[index];
    return true;
}

bool ScriptEngine::SetArrayElement(const std::string& name, size_t index, const ScriptParam& value) {
    auto it = _arrays.find(name);
    if (it == _arrays.end() || index >= it->second.size()) {
        return false;
    }
    it->second[index] = value;
    return true;
}

bool ScriptEngine::RegisterFunction(const std::string& name, ScriptFunction func) {
    if (_functions.find(name) != _functions.end()) {
        return false;
    }
    _functions[name] = func;
    return true;
}

bool ScriptEngine::UnregisterFunction(const std::string& name) {
    return _functions.erase(name) > 0;
}

bool ScriptEngine::ValidateParams(const CommandInfo& cmd, ScriptContext* context) {
    if (!context || !context->params) {
        return cmd.params.empty();
    }

    if (context->paramCount < static_cast<int>(cmd.params.size())) {
        // Check if remaining parameters are optional
        for (size_t i = context->paramCount; i < cmd.params.size(); i++) {
            if (!cmd.params[i].optional) {
                return false;
            }
        }
    }

    // Validate parameter types
    for (int i = 0; i < context->paramCount && i < static_cast<int>(cmd.params.size()); i++) {
        switch (cmd.params[i].type) {
            case ParamType::Number:
                if (context->params[i].type != ScriptParamType::Integer && 
                    context->params[i].type != ScriptParamType::Float) {
                    return false;
                }
                break;
            case ParamType::String:
                if (context->params[i].type != ScriptParamType::String) {
                    return false;
                }
                break;
            // Add other parameter type validations
        }
    }

    return true;
}

bool ScriptEngine::ParseScript(const std::string& script, std::vector<std::string>& commands) {
    std::istringstream stream(script);
    std::string line;

    while (std::getline(stream, line)) {
        // Remove comments
        size_t commentPos = line.find(';');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }

        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (!line.empty()) {
            commands.push_back(line);
        }
    }

    return true;
}

void ScriptEngine::RegisterDefaultCommands() {
    // Player commands
    REGISTER_COMMAND("GetPlayerName", 
        "Gets the player's name",
        "GetPlayerName",
        CommandFlags::RequiresPlayer)
    {
        auto& game = Game::GameManager::GetInstance();
        std::string name = game.GetPlayerName();
        
        ScriptParam result;
        result.type = ScriptParamType::String;
        result.stringValue = _strdup(name.c_str());
        
        return {true, result, ""};
    }
    END_COMMAND;

    REGISTER_COMMAND("SetPlayerName",
        "Sets the player's name",
        "SetPlayerName <name>",
        CommandFlags::RequiresPlayer,
        ParamInfo{ParamType::String, false, "name", "New player name"})
    {
        auto& game = Game::GameManager::GetInstance();
        game.SetPlayerName(context->params[0].stringValue);
        return {true, {}, ""};
    }
    END_COMMAND;

    // World commands
    REGISTER_COMMAND("GetWorldPosition",
        "Gets the player's world position",
        "GetWorldPosition",
        CommandFlags::RequiresPlayer)
    {
        auto& game = Game::GameManager::GetInstance();
        short x, y;
        game.GetWorldPosition(x, y);
        
        // Create array result with x,y coordinates
        std::vector<ScriptParam> coords = {
            {ScriptParamType::Integer, x},
            {ScriptParamType::Integer, y}
        };
        CreateArray("position", coords);
        
        ScriptParam result;
        result.type = ScriptParamType::Object;
        result.objectValue = &_arrays["position"];
        
        return {true, result, ""};
    }
    END_COMMAND;

    // More default commands...
}

} // namespace Script
} // namespace F2SE 