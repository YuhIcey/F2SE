#pragma once

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

struct PlayerState {
    int x;
    int y;
    int elevation;
    int rotation;
};

__declspec(dllexport) void StartMultiplayer();
__declspec(dllexport) void StopMultiplayer();
__declspec(dllexport) const PlayerState* GetPlayerState();

#ifdef __cplusplus
}
#endif
