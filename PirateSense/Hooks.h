// PirateSense.

#pragma once

#include <d3d11.h>
#include <Windows.h>

// v2.9.0
// These most likely won't change unless the engine version changes.
// World: 0x87FD110 (48 8B 05 ? ? ? ? 48 8B 88 ? ? ? ? 48 85 C9 74 06 48 8B 49 70)
// Names: 0x8735490 (48 8B 1D ? ? ? ? 48 85 DB 75 ? B9 08 04 00 00)
// Objects: 0x8739AD0 (89 0D ? ? ? ? 48 8B DF 48 89 5C 24)

class Hooks
{
public:
	static inline constexpr uint16_t MethodIndex_OnPresent = 8;
	static inline constexpr uint16_t MethodIndex_OnResizeBuffers = 13;

	using Present = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT);
	static inline Present OnPresent_Original = nullptr;

	using ResizeBuffers = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
	static inline ResizeBuffers OnResizeBuffers_Original = nullptr;

	static inline WNDPROC WindowProcedure_Original = nullptr;

	using SetCursor = HCURSOR(WINAPI*)(HCURSOR);
	static inline SetCursor SetCursor_Original = nullptr;

	using SetCursorPos = BOOL(WINAPI*)(int X, int Y);
	static inline SetCursorPos SetCursorPos_Original = nullptr;

	static uintptr_t FindGWorld(const MODULEINFO Module)
	{
		// Signature (v2.9.0): 48 8B 05 ? ? ? ? 48 8B 88 ? ? ? ? 48 85 C9 74 06 48 8B 49 70
		
		char Sign[] = "\x48\x8B\x05\x00\x00\x00\x00\x48\x8B\x88\x00\x00\x00\x00\x48\x85\xC9\x74\x06\x48\x8B\x49\x70";
		char Mask[] = "xxx????xxx????xxxxxxxxx";

		uintptr_t Address = ts::Aobs(Sign, Mask, reinterpret_cast<uintptr_t>(Module.lpBaseOfDll), reinterpret_cast<uintptr_t>(Module.lpBaseOfDll) + static_cast<uintptr_t>(Module.SizeOfImage));
		if (!Address)
		{
			return 0;
		}

		const uint32_t Offset = *reinterpret_cast<uint32_t*>(Address + 3);
		Address = Address + 7 + Offset;
		
		return Address;
	}

	static uintptr_t FindGObjects(const MODULEINFO Module)
	{
		// Signature (v2.9.0): 89 0D ? ? ? ? 48 8B DF 48 89 5C 24
		
		char Sign[] = "\x89\x0D\x00\x00\x00\x00\x48\x8B\xDF\x48\x89\x5C\x24";
		char Mask[] = "xx????xxxxxxx";

		uintptr_t Address = ts::Aobs(Sign, Mask, reinterpret_cast<uintptr_t>(Module.lpBaseOfDll), reinterpret_cast<uintptr_t>(Module.lpBaseOfDll) + static_cast<uintptr_t>(Module.SizeOfImage));
		if (!Address)
		{
			return 0;
		}

		const int32_t Offset = *reinterpret_cast<uint32_t*>(Address + 2);
		Address = Address + 6 + Offset + 16;

		return Address;
	}

	static uintptr_t FindGNames(const MODULEINFO Module)
	{
		// Signature (v2.9.0): 48 8B 1D ? ? ? ? 48 85 DB 75 ? B9 08 04 00 00

		char Sign[] = "\x48\x8B\x1D\x00\x00\x00\x00\x48\x85\xDB\x75\x00\xB9\x08\x04\x00\x00";
		char Mask[] = "xxx????xxxx?xxxxx";

		uintptr_t Address = ts::Aobs(Sign, Mask, reinterpret_cast<uintptr_t>(Module.lpBaseOfDll), reinterpret_cast<uintptr_t>(Module.lpBaseOfDll) + static_cast<uintptr_t>(Module.SizeOfImage));
		if (!Address)
		{
			return 0;
		}

		const int32_t Offset = *reinterpret_cast<uint32_t*>(Address + 3);
		Address = Address + 7 + Offset;

		return Address;
	}
};
