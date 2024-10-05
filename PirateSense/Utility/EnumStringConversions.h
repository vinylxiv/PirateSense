// PirateSense.

#pragma once

#include <kiero.h>
#include <string>

class Utility
{
public:
	static std::wstring RenderTypeToString(kiero::RenderType::Enum RenderType)
	{
		switch (RenderType)
		{
		case kiero::RenderType::D3D9: return L"D3D9"; break;
		case kiero::RenderType::D3D10: return L"D3D10"; break;
		case kiero::RenderType::D3D11: return L"D3D11"; break;
		case kiero::RenderType::D3D12: return L"D3D12"; break;
		case kiero::RenderType::OpenGL: return L"OpenGL"; break;
		case kiero::RenderType::Vulkan: return L"Vulkan"; break;
		case kiero::RenderType::Auto: return L"Auto"; break;

		case kiero::RenderType::None:
		default: return L"None"; break;
		}
	}
};
