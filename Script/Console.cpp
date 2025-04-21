#include "Console.h"
#include "ScriptEngine.h"
#include <iostream>
#include <algorithm>

namespace F2SE {
namespace Script {

Console& Console::GetInstance() {
    static Console instance;
    return instance;
}

bool Console::Initialize() {
    if (_initialized) {
        return true;
    }

    std::cout << "F2SE: Initializing Console..." << std::endl;
    
    // Initialize console state
    _visible = false;
    _currentCommand.clear();
    _cursorPosition = 0;
    _historyPosition = 0;
    _history.clear();

    _initialized = true;
    std::cout << "F2SE: Console initialized" << std::endl;
    return true;
}

void Console::Shutdown() {
    Hide();
    ClearHistory();
    _initialized = false;
}

void Console::Show() {
    if (!_initialized) {
        return;
    }
    _visible = true;
    UpdateDisplay();
}

void Console::Hide() {
    _visible = false;
}

void Console::Toggle() {
    if (_visible) {
        Hide();
    } else {
        Show();
    }
}

void Console::ExecuteCommand(const std::string& command) {
    if (!_initialized || command.empty()) {
        return;
    }

    // Create script context
    ScriptContext context;
    // TODO: Parse command parameters
    
    // Execute command
    auto result = ScriptEngine::GetInstance().ExecuteCommand(command, &context);
    
    // Add to history
    AddToHistory(command, result);
    
    // Display result
    if (result.success) {
        std::cout << "F2SE: Command executed successfully" << std::endl;
        if (result.result.type == ScriptParamType::String) {
            std::cout << "Result: " << result.result.stringValue << std::endl;
        }
    } else {
        std::cout << "F2SE: Command failed - " << result.error << std::endl;
    }

    // Clear current command
    _currentCommand.clear();
    _cursorPosition = 0;
    UpdateDisplay();
}

void Console::ProcessInput(char key) {
    if (!_visible) {
        return;
    }

    switch (key) {
        case '\r':  // Enter
        case '\n':
            HandleEnter();
            break;
        
        case '\b':  // Backspace
            HandleBackspace();
            break;
        
        case '\t':  // Tab
            HandleTab();
            break;
        
        case 127:   // Delete
            HandleDelete();
            break;
        
        // Arrow keys and special keys are handled as escape sequences
        case 27:    // Escape
            if (_currentCommand.empty()) {
                Hide();
            } else {
                _currentCommand.clear();
                _cursorPosition = 0;
                UpdateDisplay();
            }
            break;
        
        default:
            if (key >= 32 && key <= 126) {  // Printable characters
                _currentCommand.insert(_cursorPosition, 1, key);
                _cursorPosition++;
                UpdateDisplay();
            }
            break;
    }
}

void Console::Render() {
    if (!_visible) {
        return;
    }

    // TODO: Implement actual rendering using game's rendering system
    std::cout << "> " << _currentCommand;
    for (size_t i = 0; i < _cursorPosition; i++) {
        std::cout << " ";
    }
    std::cout << "_" << std::endl;
}

void Console::AddToHistory(const std::string& command, const CommandResult& result) {
    _history.push_front({command, result, result.success});
    if (_history.size() > MAX_HISTORY) {
        _history.pop_back();
    }
    _historyPosition = 0;
}

void Console::ClearHistory() {
    _history.clear();
    _historyPosition = 0;
}

std::vector<std::string> Console::GetCompletions(const std::string& partial) {
    std::vector<std::string> completions;
    
    // Get all commands
    auto commands = ScriptEngine::GetInstance().GetCommands();
    
    // Find matching commands
    for (const auto& cmd : commands) {
        if (cmd->name.substr(0, partial.length()) == partial) {
            completions.push_back(cmd->name);
        }
    }
    
    return completions;
}

void Console::CompleteCommand() {
    if (_currentCommand.empty()) {
        return;
    }

    auto completions = GetCompletions(_currentCommand);
    if (completions.empty()) {
        return;
    }

    if (completions.size() == 1) {
        // Single completion
        _currentCommand = completions[0];
        _cursorPosition = _currentCommand.length();
    } else {
        // Multiple completions - show all possibilities
        std::cout << "\nPossible completions:" << std::endl;
        for (const auto& completion : completions) {
            std::cout << completion << std::endl;
        }
    }

    UpdateDisplay();
}

void Console::ScrollHistory(bool up) {
    if (_history.empty()) {
        return;
    }

    if (up) {
        if (_historyPosition < _history.size()) {
            _historyPosition++;
            _currentCommand = _history[_historyPosition - 1].command;
            _cursorPosition = _currentCommand.length();
        }
    } else {
        if (_historyPosition > 0) {
            _historyPosition--;
            if (_historyPosition == 0) {
                _currentCommand.clear();
            } else {
                _currentCommand = _history[_historyPosition - 1].command;
            }
            _cursorPosition = _currentCommand.length();
        }
    }

    UpdateDisplay();
}

void Console::MoveCursor(bool left) {
    if (left) {
        if (_cursorPosition > 0) {
            _cursorPosition--;
        }
    } else {
        if (_cursorPosition < _currentCommand.length()) {
            _cursorPosition++;
        }
    }
    UpdateDisplay();
}

void Console::HandleBackspace() {
    if (_cursorPosition > 0) {
        _currentCommand.erase(_cursorPosition - 1, 1);
        _cursorPosition--;
        UpdateDisplay();
    }
}

void Console::HandleDelete() {
    if (_cursorPosition < _currentCommand.length()) {
        _currentCommand.erase(_cursorPosition, 1);
        UpdateDisplay();
    }
}

void Console::HandleEnter() {
    if (!_currentCommand.empty()) {
        ExecuteCommand(_currentCommand);
    }
}

void Console::HandleTab() {
    CompleteCommand();
}

void Console::UpdateDisplay() {
    // Clear current line
    std::cout << "\r";
    for (size_t i = 0; i < _currentCommand.length() + 2; i++) {
        std::cout << " ";
    }
    std::cout << "\r";
    
    // Display prompt and command
    Render();
}

} // namespace Script
} // namespace F2SE 