#include "GameAddresses.h"
#include <Windows.h>
#include <stdexcept>

namespace F2SE::Game {

int8_t GameAddresses::GetStat(uintptr_t address) {
    int8_t value = 0;
    SIZE_T bytesRead;
    
    if (!ReadProcessMemory(GetCurrentProcess(), 
                          reinterpret_cast<LPCVOID>(address),
                          &value, 
                          sizeof(value), 
                          &bytesRead) || 
        bytesRead != sizeof(value)) {
        throw std::runtime_error("Failed to read stat value from memory");
    }
    
    return value;
}

void GameAddresses::SetStat(uintptr_t address, int8_t value) {
    if (!IsValidStatValue(value)) {
        throw std::invalid_argument("Invalid stat value");
    }

    SIZE_T bytesWritten;
    if (!WriteProcessMemory(GetCurrentProcess(),
                           reinterpret_cast<LPVOID>(address),
                           &value,
                           sizeof(value),
                           &bytesWritten) ||
        bytesWritten != sizeof(value)) {
        throw std::runtime_error("Failed to write stat value to memory");
    }
}

bool GameAddresses::IsValidStatValue(int8_t value, int8_t min, int8_t max) {
    return value >= min && value <= max;
}

} // namespace F2SE::Game 