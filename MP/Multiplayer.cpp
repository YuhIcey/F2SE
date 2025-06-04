#include "Multiplayer.h"
#include <winsock2.h>

static WSADATA g_wsaData;
static SOCKET  g_socket = INVALID_SOCKET;

static PlayerState g_playerState = {0};
static HANDLE g_tickThread = NULL;
static volatile LONG g_running = 0;

#define PLAYER_X_ADDR        ((int*)0x0050ABC4)
#define PLAYER_Y_ADDR        ((int*)0x0050ABC8)
#define PLAYER_ELEV_ADDR     ((int*)0x0050ABCC)
#define PLAYER_ROT_ADDR      ((int*)0x0050ABD0)

static void MultiplayerTick()
{
    // Networking will be implemented later
}

static DWORD WINAPI TickThread(LPVOID)
{
    while (InterlockedCompareExchange(&g_running, 0, 0))
    {
        g_playerState.x = *PLAYER_X_ADDR;
        g_playerState.y = *PLAYER_Y_ADDR;
        g_playerState.elevation = *PLAYER_ELEV_ADDR;
        g_playerState.rotation = *PLAYER_ROT_ADDR;

        MultiplayerTick();
        Sleep(50);
    }
    return 0;
}

void StartMultiplayer()
{
    if (g_running)
        return;
    if (WSAStartup(MAKEWORD(2, 2), &g_wsaData) != 0)
        return;

    g_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (g_socket == INVALID_SOCKET)
    {
        WSACleanup();
        return;
    }

    g_running = 1;
    g_tickThread = CreateThread(NULL, 0, TickThread, NULL, 0, NULL);
}

void StopMultiplayer()
{
    if (!g_running)
        return;
    g_running = 0;
    WaitForSingleObject(g_tickThread, INFINITE);
    CloseHandle(g_tickThread);
    g_tickThread = NULL;

    if (g_socket != INVALID_SOCKET)
    {
        closesocket(g_socket);
        g_socket = INVALID_SOCKET;
    }

    WSACleanup();
}

const PlayerState* GetPlayerState()
{
    return &g_playerState;
}
