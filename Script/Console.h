#pragma once

#include "../Common/Types.h"
#include <string>
#include <vector>
#include <deque>

namespace F2SE {
namespace Script {

// Console command history entry
struct HistoryEntry {
    std::string command;
    CommandResult result;
    bool success;
};

class Console {
public:
    static Console& GetInstance();

    bool Initialize();
    void Shutdown();

    // Console state
    bool IsVisible() const { return _visible; }
    void Show();
    void Hide();
    void Toggle();

    // Command processing
    void ExecuteCommand(const std::string& command);
    void ProcessInput(char key);
    void Render();

    // History management
    void AddToHistory(const std::string& command, const CommandResult& result);
    const std::deque<HistoryEntry>& GetHistory() const { return _history; }
    void ClearHistory();

    // Auto-completion
    std::vector<std::string> GetCompletions(const std::string& partial);
    void CompleteCommand();

private:
    Console() = default;
    ~Console() = default;
    Console(const Console&) = delete;
    Console& operator=(const Console&) = delete;

    bool _visible = false;
    bool _initialized = false;
    std::string _currentCommand;
    size_t _cursorPosition = 0;
    std::deque<HistoryEntry> _history;
    size_t _historyPosition = 0;
    static constexpr size_t MAX_HISTORY = 100;

    // Internal helpers
    void ScrollHistory(bool up);
    void MoveCursor(bool left);
    void HandleBackspace();
    void HandleDelete();
    void HandleEnter();
    void HandleTab();
    void UpdateDisplay();
};

} // namespace Script
} // namespace F2SE 