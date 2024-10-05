// PirateSense.

#pragma once

#include <d3d11.h>
#include <DiscordRPC/DiscordPresence.h>
#include <imgui.h>
#include <Psapi.h>
#include <vector>

#include <SDK/SDK.h>

struct FCrewColor
{
	FGuid Id;
	ImVec4 Color;
};

class PirateSense
{
public:
	static inline HMODULE CoreModuleHandle = nullptr;

	static inline bool bInitialisedGraphics = false;
	static inline HWND WindowHandle = nullptr;

	static inline ID3D11Device* Device = nullptr;
	static inline ID3D11DeviceContext* DeviceContext = nullptr;
	static inline ID3D11RenderTargetView* MainRenderTargetView = nullptr;

	static inline ImFont* MenuFont = nullptr;
	static inline ImFont* OverlayFont = nullptr;

	static inline MODULEINFO SoTBaseModule;

	static inline uintptr_t GWorldAddress = 0;
	static inline uintptr_t GObjectsAddress = 0;
	static inline uintptr_t GNamesAddress = 0;

	static inline DiscordRPC* DiscordPresence = nullptr;

	static inline UAthenaGameViewportClient* AthenaGameViewportClient = nullptr;
	static inline ULocalPlayer* LocalPlayer = nullptr;
	static inline APlayerController* PlayerController = nullptr;
	static inline AAthenaPlayerCharacter* LocalPlayerActor = nullptr;
	static inline UWorld* World = nullptr;
	static inline UGameInstance* GameInstance = nullptr;
	static inline AAthenaGameState* GameState = nullptr;
	static inline TArray<ULevel*> Levels;
	
	static inline bool bUIActive = true;

	static inline std::vector<FCrewColor> CrewColors;

	static void Init();
	static void MainLoop();
	static void Shutdown();

	static void BindHooks();
	static void InitCoreObjects();

	static bool ShouldUpdateGameVars();
	static bool UpdateGameVars();
	
	static bool InitGraphics(IDXGISwapChain* SwapChain);
	static void RenderGraphics();

	static HRESULT WINAPI OnPresent_Hook(IDXGISwapChain* SwapChain, UINT SyncInterval, UINT Flags);
	static HRESULT WINAPI OnResizeBuffers_Hook(IDXGISwapChain* SwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);

	static LRESULT WINAPI WindowProcedure_Hook(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam);

	static HCURSOR WINAPI SetCursor_Hook(HCURSOR Cursor);
	static BOOL WINAPI SetCursorPos_Hook(int X, int Y);
};
