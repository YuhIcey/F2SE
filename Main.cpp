#include <Windows.h>
#include <iostream>
#include <string>
#include <TlHelp32.h>
#include "Game/GameAddresses.h"
#include "Game/SkillSystem.h"
#include <format>

// Global variables
HINSTANCE g_hInstance = nullptr;
bool g_isRunning = false;
HHOOK g_keyboardHook = nullptr;
bool g_consoleVisible = false;
const char* VERSION = "F2SE v0.01";
HANDLE g_gameProcess = nullptr;
DWORD g_processId = 0;
HWND g_consoleWindow = nullptr;

// Function declarations
bool InitializeF2SE();
void ShutdownF2SE();
void ShowError(const std::string& message);
bool LaunchGame();
void SetupConsole();
LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam);

// Windows entry point
int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow)
{
    g_hInstance = hInstance;

    try {
        // Set up console first
        SetupConsole();
        ShowWindow(g_consoleWindow, SW_HIDE); // Hide console initially

        // Set up keyboard hook for console toggle
        g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHook, hInstance, 0);
        if (!g_keyboardHook) {
            ShowError("Failed to set keyboard hook");
            return 1;
        }

        // Initialize F2SE
        if (!InitializeF2SE()) {
            ShowError("Failed to initialize F2SE");
            return 1;
        }

        // Launch Fallout 2
        if (!LaunchGame()) {
            ShowError("Failed to launch Fallout 2");
            return 1;
        }

        std::cout << "Fallout 2 launched successfully!\n";
        std::cout << "Press ` to toggle console\n";

        // Main message loop
        MSG msg = {};
        g_isRunning = true;

        while (g_isRunning) {
            // Check if Fallout 2 is still running
            DWORD exitCode;
            if (GetExitCodeProcess(g_gameProcess, &exitCode) && exitCode != STILL_ACTIVE) {
                g_isRunning = false;
                break;
            }

            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    g_isRunning = false;
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            Sleep(10); // Prevent high CPU usage
        }

        ShutdownF2SE();
        return static_cast<int>(msg.wParam);
    }
    catch (const std::exception& e) {
        ShowError(std::format("Unhandled exception: {}", e.what()));
        return 1;
    }
}

bool InitializeF2SE() {
    // Initialize systems
    if (!F2SE::Game::SkillSystem::Initialize()) {
        ShowError("Failed to initialize Skill System");
        return false;
    }

    // TODO: Initialize other systems
    std::cout << "F2SE initialized successfully\n";
    return true;
}

void ShutdownF2SE() {
    // Unhook keyboard
    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }

    // Close game process handle
    if (g_gameProcess) {
        CloseHandle(g_gameProcess);
        g_gameProcess = nullptr;
    }

    // Free console
    if (g_consoleWindow) {
        FreeConsole();
        g_consoleWindow = nullptr;
    }
}

void ShowError(const std::string& message) {
    MessageBoxA(nullptr, message.c_str(), "F2SE Error", MB_OK | MB_ICONERROR);
}

void SetupConsole() {
    // Allocate console
    AllocConsole();
    g_consoleWindow = GetConsoleWindow();

    // Set console title
    SetConsoleTitleA(VERSION);

    // Redirect standard streams
    FILE* dummy;
    freopen_s(&dummy, "CONOUT$", "w", stdout);
    freopen_s(&dummy, "CONOUT$", "w", stderr);
    freopen_s(&dummy, "CONIN$", "r", stdin);

    // Set console properties
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    csbi.dwSize.X = 100;
    csbi.dwSize.Y = 1000;
    SetConsoleScreenBufferSize(hConsole, csbi.dwSize);

    // Set console colors
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);

    // Print version
    std::cout << VERSION << " initialized\n";
}

bool LaunchGame() {
    // Get current directory
    char currentDir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, currentDir);

    // Construct path to Fallout2.exe
    std::string gamePath = std::string(currentDir) + "\\Fallout2.exe";

    // Launch process
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    // Create process in suspended state
    if (!CreateProcessA(
        gamePath.c_str(),    // Application name
        nullptr,             // Command line
        nullptr,             // Process handle not inheritable
        nullptr,             // Thread handle not inheritable
        FALSE,              // Set handle inheritance to FALSE
        CREATE_SUSPENDED,    // Create suspended
        nullptr,            // Use parent's environment block
        nullptr,            // Use parent's starting directory 
        &si,                // Pointer to STARTUPINFO structure
        &pi                 // Pointer to PROCESS_INFORMATION structure
    )) {
        return false;
    }

    // Store process handle and ID
    g_gameProcess = pi.hProcess;
    g_processId = pi.dwProcessId;

    // Resume the process
    ResumeThread(pi.hThread);

    // Close thread handle
    CloseHandle(pi.hThread);

    return true;
}

LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_KEYDOWN) {
            KBDLLHOOKSTRUCT* kbdStruct = (KBDLLHOOKSTRUCT*)lParam;
            
            // Check for backtick key
            if (kbdStruct->vkCode == VK_OEM_3) {  // VK_OEM_3 is the backtick/tilde key
                g_consoleVisible = !g_consoleVisible;
                ShowWindow(g_consoleWindow, g_consoleVisible ? SW_SHOW : SW_HIDE);
                
                if (g_consoleVisible) {
                    SetForegroundWindow(g_consoleWindow);
                }
                
                return 1;  // Prevent the key from being passed to the game
            }
        }
    }
    
    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
} 