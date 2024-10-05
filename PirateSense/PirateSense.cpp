// PirateSense.

#include "PirateSense.h"

#include <format>
#include <imgui_internal.h>
#include <kiero.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include <MinHook.h>
#include <ThirdParty/tslib.h>
#include <ThirdParty/XORSTR.h>
#include <variant>

#include "Hooks.h"
#include "Module/AimbotModule.h"
#include "Module/DevModule.h"
#include "Module/ESPModule.h"
#include "Module/ExploitsModule.h"
#include "Module/MiscModule.h"
#include "Module/ModuleInterface.h"
#include "Module/ViewModule.h"
#include "Utility/EnumStringConversions.h"
#include "Utility/Logging.h"

void PirateSense::Init()
{
	PIRATESENSE_DEVLOG(LOG_Core, LogType::Info, xorstr_(L"Initialising..."));

	// Init kiero and pass on to the binding hooks.
	if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
	{
		PIRATESENSE_LOG(LOG_Core, LogType::Info, xorstr_(L"Welcome to PirateSense"));
		PIRATESENSE_DEVLOGARGS(LOG_Core, LogType::Info, xorstr_(L"Renderer: {0}"), std::vector<std::variant<std::wstring>>{ Utility::RenderTypeToString(kiero::getRenderType()) });

		BindHooks();
		InitCoreObjects();

		//ModuleInterface::AddModule<AimbotModule>();
		//ModuleInterface::AddModule<DevModule>();
		ModuleInterface::AddModule<ESPModule>();
		//ModuleInterface::AddModule<ExploitsModule>();
		//ModuleInterface::AddModule<MiscModule>();
		//ModuleInterface::AddModule<ViewModule>();

		DiscordPresence->Initialize();
	}
	else
	{
		PIRATESENSE_LOG(LOG_Core, LogType::Error, xorstr_(L"Failed to initialise kiero!"));
	}
}

void PirateSense::MainLoop()
{
	while (!GetAsyncKeyState(VK_END))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
}

void PirateSense::Shutdown()
{
	PIRATESENSE_LOG(LOG_Core, LogType::Info, xorstr_(L"Shutting down..."));

	ModuleInterface::ShutdownModules();
	
	ImGui_ImplDX11_Shutdown();
	ImGui::DestroyContext();

	kiero::unbind(8);
	kiero::unbind(13);
	kiero::shutdown();
}

void PirateSense::BindHooks()
{
	PIRATESENSE_DEVLOG(LOG_Core, LogType::Info, xorstr_(L"Binding hooks..."));

	if (!K32GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(nullptr), &SoTBaseModule, sizeof(MODULEINFO)))
	{
		PIRATESENSE_LOG(LOG_Core, LogType::Error, xorstr_(L"Failed to get SoT Base Module!"));
	}

	if (kiero::bind(Hooks::MethodIndex_OnPresent, reinterpret_cast<void**>(&Hooks::OnPresent_Original), OnPresent_Hook) != kiero::Status::Success)
	{
		PIRATESENSE_LOG(LOG_Core, LogType::Error, xorstr_(L"Failed to bind to OnPresent!"));
	}
	if (kiero::bind(Hooks::MethodIndex_OnResizeBuffers, reinterpret_cast<void**>(&Hooks::OnResizeBuffers_Original), OnResizeBuffers_Hook) != kiero::Status::Success)
	{
		PIRATESENSE_LOG(LOG_Core, LogType::Error, xorstr_(L"Failed to bind to OnResizeBuffers!"));
	}

	if (MH_CreateHook(&::SetCursor, &PirateSense::SetCursor_Hook, reinterpret_cast<LPVOID*>(&Hooks::SetCursor_Original)) != MH_OK)
	{
		PIRATESENSE_LOG(LOG_Core, LogType::Error, xorstr_(L"Failed to bind to SetCursor!"));
	}
	if (MH_CreateHook(&::SetCursorPos, &PirateSense::SetCursorPos_Hook, reinterpret_cast<LPVOID*>(&Hooks::SetCursorPos_Original)) != MH_OK)
	{
		PIRATESENSE_LOG(LOG_Core, LogType::Error, xorstr_(L"Failed to bind to SetCursorPos!"));
	}

	PIRATESENSE_DEVLOG(LOG_Core, LogType::Success, xorstr_(L"Finished binding hooks."));
}

void PirateSense::InitCoreObjects()
{
	GWorldAddress = Hooks::FindGWorld(SoTBaseModule);
	if (!GWorldAddress)
	{
		PIRATESENSE_LOG(LOG_Core, LogType::Error, xorstr_(L"Failed to find GWorld!"));
	}
	PIRATESENSE_DEVLOGARGS(LOG_Core, LogType::Info, xorstr_(L"GWorld offset (Decimal): {0}"), std::vector<std::variant<uintptr_t>>{ (GWorldAddress - reinterpret_cast<uintptr_t>(SoTBaseModule.lpBaseOfDll)) });

	GObjectsAddress = Hooks::FindGObjects(SoTBaseModule);
	if (!GObjectsAddress)
	{
		PIRATESENSE_LOG(LOG_Core, LogType::Error, xorstr_(L"Failed to find GObjects!"));
	}
	PIRATESENSE_DEVLOGARGS(LOG_Core, LogType::Info, xorstr_(L"GObjects offset (Decimal): {0}"), std::vector<std::variant<uintptr_t>>{ (GObjectsAddress - reinterpret_cast<uintptr_t>(SoTBaseModule.lpBaseOfDll)) });

	GNamesAddress = Hooks::FindGNames(SoTBaseModule);
	if (!GNamesAddress)
	{
		PIRATESENSE_LOG(LOG_Core, LogType::Error, xorstr_(L"Failed to find GNames!"));
	}
	PIRATESENSE_DEVLOGARGS(LOG_Core, LogType::Info, xorstr_(L"GNames offset (Decimal): {0}"), std::vector<std::variant<uintptr_t>>{ (GNamesAddress - reinterpret_cast<uintptr_t>(SoTBaseModule.lpBaseOfDll)) });

	UWorld::GWorld = reinterpret_cast<decltype(UWorld::GWorld)>(GWorldAddress);
	UObject::GObjects = reinterpret_cast<decltype(UObject::GObjects)>(GObjectsAddress);
	FName::GNames = *reinterpret_cast<decltype(FName::GNames)*>(GNamesAddress);
	AthenaGameViewportClient = FindObject<UAthenaGameViewportClient>("AthenaGameViewportClient Transient.AthenaGameEngine_1.AthenaGameViewportClient_1");
	if (!AthenaGameViewportClient)
	{
		PIRATESENSE_LOG(LOG_Core, LogType::Error, xorstr_(L"Failed to find GameViewportClient!"));
	}

	PIRATESENSE_DEVLOG(LOG_Core, LogType::Success, xorstr_(L"Finished initializing core objects."));
}

bool PirateSense::InitGraphics(IDXGISwapChain* SwapChain)
{
	if (SUCCEEDED(SwapChain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&Device))))
	{
		Device->GetImmediateContext(&DeviceContext);

		DXGI_SWAP_CHAIN_DESC SwapChainDescription;
		SwapChain->GetDesc(&SwapChainDescription);

		WindowHandle = SwapChainDescription.OutputWindow;

		ID3D11Texture2D* BackBuffer = nullptr;
		if (SUCCEEDED(SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&BackBuffer))))
		{
			Device->CreateRenderTargetView(BackBuffer, nullptr, &MainRenderTargetView);
			BackBuffer->Release();
		}
		else
		{
			PIRATESENSE_LOG(LOG_Core, LogType::Error, xorstr_(L"Failed to make Render Target!"));
		}

		// Add hook for WndProc (Window Procedure)
		Hooks::WindowProcedure_Original = reinterpret_cast<WNDPROC>(SetWindowLongPtr(WindowHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProcedure_Hook)));

		ImGui::CreateContext();
		ImGuiIO& IO = ImGui::GetIO();

		IO.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

		MenuFont = IO.Fonts->AddFontFromFileTTF(xorstr_("C:\\Windows\\Fonts\\Verdana.ttf"), 17);
		OverlayFont = IO.Fonts->AddFontFromFileTTF(xorstr_("C:\\Windows\\Fonts\\Verdana.ttf"), 13);

		ImGui_ImplWin32_Init(WindowHandle);
		ImGui_ImplDX11_Init(Device, DeviceContext);
		ImGui_ImplDX11_CreateDeviceObjects();

		bInitialisedGraphics = true;

		PIRATESENSE_LOG(LOG_Core, LogType::Success, xorstr_(L"Initialised graphics."));

		return true;
	}

	return false;
}

void PirateSense::RenderGraphics()
{
	ImGuiIO& IO = ImGui::GetIO();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// @todo: Move this to a better input handling location.
	if (GetAsyncKeyState(VK_INSERT) & 1)
	{
		bUIActive = !bUIActive;

		IO.MouseDrawCursor = bUIActive;
	}

	// Render Overlay.
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
		ImGui::Begin(xorstr_("Overlay"), nullptr, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar);
		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetWindowSize(ImVec2(IO.DisplaySize.x, IO.DisplaySize.y), ImGuiCond_Always);

		ImGui::PushFont(OverlayFont);

		ImGui::Text(xorstr_("Overlay")); // @todo: Replace with watermark text.

		for (ModuleInterface* Module : ModuleInterface::GetModules())
		{
			//Module->RenderOverlay(ImGui::GetCurrentWindow()->DrawList); // @todo: Uncomment when crash fixed
		}

		ImGui::End();

		ImGui::PopFont();

		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);
	}

	// Render Menu.
	{
		ImGui::PushFont(MenuFont);

		if (bUIActive)
		{
			ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_Once);
			ImGui::Begin(xorstr_("PirateSense"), nullptr, ImGuiWindowFlags_NoCollapse);

			ImGuiStyle& Style = ImGui::GetStyle();
			Style.WindowPadding = ImVec2(8, 8);
			Style.WindowRounding = 7.0f;
			Style.FramePadding = ImVec2(4, 3);
			Style.FrameRounding = 0.0f;
			Style.ItemSpacing = ImVec2(6, 4);
			Style.ItemInnerSpacing = ImVec2(4, 4);
			Style.IndentSpacing = 20.0f;
			Style.ScrollbarSize = 14.0f;
			Style.ScrollbarRounding = 9.0f;
			Style.GrabMinSize = 5.0f;
			Style.GrabRounding = 0.0f;
			Style.WindowBorderSize = 0;
			Style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
			Style.FramePadding = ImVec2(4, 3);
			Style.Colors[ImGuiCol_TitleBg] = ImColor(255, 26, 85, 225);
			Style.Colors[ImGuiCol_TitleBgActive] = ImColor(255, 46, 85, 225);
			Style.Colors[ImGuiCol_Button] = ImColor(255, 26, 85, 225);
			Style.Colors[ImGuiCol_ButtonActive] = ImColor(255, 46, 85, 225);
			Style.Colors[ImGuiCol_ButtonHovered] = ImColor(41, 40, 41, 255);
			Style.Colors[ImGuiCol_Separator] = ImColor(70, 70, 70, 255);
			Style.Colors[ImGuiCol_SeparatorActive] = ImColor(76, 76, 76, 255);
			Style.Colors[ImGuiCol_SeparatorHovered] = ImColor(76, 76, 76, 255);
			Style.Colors[ImGuiCol_Tab] = ImColor(255, 26, 85, 225);
			Style.Colors[ImGuiCol_TabHovered] = ImColor(255, 66, 85, 225);
			Style.Colors[ImGuiCol_TabActive] = ImColor(255, 46, 85, 225);
			Style.Colors[ImGuiCol_SliderGrab] = ImColor(255, 26, 85, 225);
			Style.Colors[ImGuiCol_SliderGrabActive] = ImColor(255, 46, 85, 225);
			Style.Colors[ImGuiCol_MenuBarBg] = ImColor(76, 76, 76, 255);
			Style.Colors[ImGuiCol_FrameBg] = ImColor(37, 36, 37, 255);
			Style.Colors[ImGuiCol_FrameBgActive] = ImColor(37, 36, 37, 255);
			Style.Colors[ImGuiCol_FrameBgHovered] = ImColor(37, 36, 37, 255);
			Style.Colors[ImGuiCol_Header] = ImColor(0, 0, 0, 0);
			Style.Colors[ImGuiCol_HeaderActive] = ImColor(0, 0, 0, 0);
			Style.Colors[ImGuiCol_HeaderHovered] = ImColor(46, 46, 46, 255);
			Style.Colors[ImGuiCol_CheckMark] = ImColor(255, 46, 85, 255);

			if (ImGui::BeginTabBar(xorstr_("TabBar")))
			{
				for (ModuleInterface* Module : ModuleInterface::GetModules())
				{
					Module->RenderMenu();
				}
				
				ImGui::EndTabBar();
			}

			ImGui::End();
		}

		ImGui::PopFont();
	}

	ImGui::Render();

	DeviceContext->OMSetRenderTargets(1, &MainRenderTargetView, nullptr);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

HRESULT PirateSense::OnPresent_Hook(IDXGISwapChain* SwapChain, const UINT SyncInterval, const UINT Flags)
{
	// First time running this function we should run some setup stuff like stuff for ImGui.
	if (!bInitialisedGraphics)
	{
		if (!InitGraphics(SwapChain))
		{
			return Hooks::OnPresent_Original(SwapChain, SyncInterval, Flags);
		}
	}

	if (UpdateGameVars())
	{
		for (ModuleInterface* Module : ModuleInterface::GetModules())
		{
			//Module->Update(); // @todo: Uncomment when crash fixed
		}

		RenderGraphics();

		for (ModuleInterface* Module : ModuleInterface::GetModules())
		{
			//Module->PostRender(); // @todo: Uncomment when crash fixed
		}
	}

	if (DiscordPresence)
	{
		DiscordPresence->Update();
	}

	return Hooks::OnPresent_Original(SwapChain, SyncInterval, Flags);
}

HRESULT PirateSense::OnResizeBuffers_Hook(IDXGISwapChain* SwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
	if (MainRenderTargetView)
	{
		DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
		MainRenderTargetView->Release();
	}

	const HRESULT Result = Hooks::OnResizeBuffers_Original(SwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);

	if (!bInitialisedGraphics)
	{
		PIRATESENSE_LOG(LOG_Core, LogType::Error, xorstr_(L"Tried to resize buffers without hooking to OnPresent!"));
	}

	ID3D11Texture2D* Buffer = nullptr;

	if (FAILED(SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&Buffer))))
	{
		PIRATESENSE_LOG(LOG_Core, LogType::Error, xorstr_(L"Failed to get Buffer!"))
	}

	if (!Buffer || FAILED(Device->CreateRenderTargetView(Buffer, nullptr, &MainRenderTargetView)))
	{
		PIRATESENSE_LOG(LOG_Core, LogType::Error, xorstr_(L"Failed to create new Render Target View!"));
	}

	Buffer->Release();

	DeviceContext->OMSetRenderTargets(1, &MainRenderTargetView, nullptr);

	const D3D11_VIEWPORT Viewport{ 0.f, 0.f, static_cast<float>(Width), static_cast<float>(Height), 0.f, 1.f };
	DeviceContext->RSSetViewports(1, &Viewport);

	return Result;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam);
LRESULT PirateSense::WindowProcedure_Hook(HWND WindowHandle, UINT Message, WPARAM WParam, LPARAM LParam)
{
	if (bUIActive)
	{
		if (ImGui::GetCurrentContext() != nullptr)
		{
			ImGuiIO& IO = ImGui::GetIO();

			switch (Message)
			{
			case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
			case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
			case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
			case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
				{
					int32_t Button = 0;

					if (Message == WM_LBUTTONDOWN || Message == WM_LBUTTONDBLCLK)
					{
						Button = 0;
					}
					if (Message == WM_RBUTTONDOWN || Message == WM_RBUTTONDBLCLK)
					{
						Button = 1;
					}
					if (Message == WM_MBUTTONDOWN || Message == WM_MBUTTONDBLCLK)
					{
						Button = 2;
					}
					if (Message == WM_XBUTTONDOWN || Message == WM_XBUTTONDBLCLK)
					{
						Button = (GET_XBUTTON_WPARAM(WParam) == XBUTTON1) ? 3 : 4;
					}

					if (!ImGui::IsAnyMouseDown() && GetCapture() == nullptr)
					{
						SetCapture(WindowHandle);
					}

					IO.MouseDown[Button] = true;

					break;
				}
			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case WM_MBUTTONUP:
			case WM_XBUTTONUP:
				{
					int32_t Button = 0;

					if (Message == WM_LBUTTONUP)
					{
						Button = 0;
					}
					if (Message == WM_RBUTTONUP)
					{
						Button = 1;
					}
					if (Message == WM_MBUTTONUP)
					{
						Button = 2;
					}
					if (Message == WM_XBUTTONUP)
					{
						Button = (GET_XBUTTON_WPARAM(WParam) == XBUTTON1) ? 3 : 4;
					}

					IO.MouseDown[Button] = false;

					if (!ImGui::IsAnyMouseDown() && GetCapture() == WindowHandle)
					{
						ReleaseCapture();
					}

					break;
				}
			case WM_MOUSEWHEEL:
				{
					IO.MouseWheel += static_cast<float>(GET_WHEEL_DELTA_WPARAM(WParam)) / static_cast<float>(WHEEL_DELTA);

					break;
				}
			case WM_MOUSEHWHEEL:
				{
					IO.MouseWheelH += static_cast<float>(GET_WHEEL_DELTA_WPARAM(WParam)) / static_cast<float>(WHEEL_DELTA);

					break;
				}
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
				{
					if (WParam < 256)
					{
						IO.KeysDown[WParam] = true;
					}
					break;
				}
			case WM_KEYUP:
			case WM_SYSKEYUP:
				{
					if (WParam < 256)
					{
						IO.KeysDown[WParam] = false;
					}

					break;
				}
			case WM_CHAR:
				{
					if (WParam > 0 && WParam < 0x10000)
					{
						IO.AddInputCharacterUTF16(static_cast<unsigned short>(WParam));
					}

					break;
				}
			default:
				{
					break;
				}
			}
		}

		const ImGuiMouseCursor& ImGUICursor = ImGui::GetMouseCursor();
		LPTSTR Win32Cursor = IDC_ARROW;
		switch (ImGUICursor)
		{
		case ImGuiMouseCursor_Arrow:
			{
				Win32Cursor = IDC_ARROW;
				
				break;
			}
		case ImGuiMouseCursor_TextInput:
			{
				Win32Cursor = IDC_IBEAM;
				
				break;
			}
		case ImGuiMouseCursor_ResizeAll:
			{
				Win32Cursor = IDC_SIZEALL;

				break;
			}
		case ImGuiMouseCursor_ResizeEW:
			{
				Win32Cursor = IDC_SIZEWE;
				
				break;
			}
		case ImGuiMouseCursor_ResizeNS:
			{
				Win32Cursor = IDC_SIZENS;

				break;
			}
		case ImGuiMouseCursor_ResizeNESW:
			{
				Win32Cursor = IDC_SIZENESW;

				break;
			}
		case ImGuiMouseCursor_ResizeNWSE:
			{
				Win32Cursor = IDC_SIZENWSE;

				break;
			}
		case ImGuiMouseCursor_Hand:
			{
				Win32Cursor = IDC_HAND;
				break;

			}
		case ImGuiMouseCursor_NotAllowed:
			{
				Win32Cursor = IDC_NO;

				break;
			}
		default:
			{
				break;
			}
		}

		Hooks::SetCursor_Original(::LoadCursor(nullptr, Win32Cursor));

		if (Message == WM_KEYUP)
		{
			return ::CallWindowProc(Hooks::WindowProcedure_Original, WindowHandle, Message, WParam, LParam);
		}

		return ::DefWindowProc(WindowHandle, Message, WParam, LParam);
	}

	return ::CallWindowProc(Hooks::WindowProcedure_Original, WindowHandle, Message, WParam, LParam);
}

HCURSOR PirateSense::SetCursor_Hook(HCURSOR Cursor)
{
	if (bUIActive)
	{
		return nullptr;
	}

	return Hooks::SetCursor_Original(Cursor);
}

BOOL PirateSense::SetCursorPos_Hook(int X, int Y)
{
	if (bUIActive)
	{
		return false;
	}

	return Hooks::SetCursorPos_Original(X, Y);
}

bool PirateSense::ShouldUpdateGameVars()
{
	if (!AthenaGameViewportClient)
	{
		return true;
	}

	if (!LocalPlayer)
	{
		return true;
	}

	if (!PlayerController)
	{
		return true;
	}

	if (!PlayerController->Character)
	{
		return true;
	}

	if (!LocalPlayerActor)
	{
		return true;
	}

	if (!World)
	{
		return true;
	}

	if (!GameInstance)
	{
		return true;
	}

	if (!GameState)
	{
		return true;
	}

	if (!Levels.Data)
	{
		return true;
	}

	//if (LocalPlayerActor && LocalPlayerActor->IsLoading())
	//{
	//	return true;
	//}

	return false;
}

bool PirateSense::UpdateGameVars()
{
	if (ShouldUpdateGameVars())
	{
		if (UWorld::GWorld)
		{
			World = *UWorld::GWorld;
		}
		
		if (World && World->OwningGameInstance)
		{
			GameInstance = World->OwningGameInstance;
		}
		
		if (GameInstance && GameInstance->LocalPlayers.Data)
		{
			LocalPlayer = GameInstance->LocalPlayers[0];
		}

		if (LocalPlayer)
		{
			PlayerController = LocalPlayer->PlayerController;
		}

		if (PlayerController)
		{
			APawn* Pawn = PlayerController->K2_GetPawn();
			std::cout << Pawn->GetName() << std::endl;
			LocalPlayerActor = Cast<AAthenaPlayerCharacter>(Pawn); // @todo: Crashes here
			std::cout << LocalPlayerActor->GetName() << std::endl;
		}

		if (World && World->GameState)
		{
			std::cout << World->GameState->GetName() << std::endl;
			GameState = Cast<AAthenaGameState>(World->GameState);
			std::cout << GameState->GetName() << std::endl;
		}

		if (World && World->Levels.Data)
		{
			std::cout << World->Levels.Count << std::endl;
			Levels = World->Levels;
		}
	}

	// Check again.
	return !ShouldUpdateGameVars();
}
