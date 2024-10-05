// PirateSense.

#include "RenderingUtility.h"

#include <imgui_internal.h>

void RenderingUtility::RenderText(const char* text, const FVector2D& pos, const ImVec4& color, const bool outlined = true, const bool centered = true)
{
	if (!text) return;
	ImVec2 ImScreen = *reinterpret_cast<const ImVec2*>(&pos);
	const ImVec2& size = ImGui::CalcTextSize(text);
	const ImGuiWindow* window = ImGui::GetCurrentWindow();
	
	if (centered)
	{
		ImScreen.x -= size.x * 0.5f;
		ImScreen.y -= size.y;
	}
	
	ImVec2 UpperLeft = ImScreen;
	UpperLeft.x -= 4.f;
	UpperLeft.y -= 2.f;
	ImVec2 LowerRight;
	LowerRight.x = ImScreen.x + size.x + 2.f;
	LowerRight.y = ImScreen.y + size.y + 4.f;
	window->DrawList->AddRectFilled(UpperLeft, LowerRight, IM_COL32(0, 0, 0, 120), 5.f);
	
	if (outlined) { window->DrawList->AddText(nullptr, 0.f, ImVec2(ImScreen.x - 1.f, ImScreen.y + 1.f), ImGui::GetColorU32(IM_COL32_BLACK), text); }
	
	window->DrawList->AddText(nullptr, 0.f, ImScreen, ImGui::GetColorU32(color), text);
}

void RenderingUtility::Render2DBox(const FVector2D& top, const FVector2D& bottom, const float height, const float width, const ImVec4& color)
{
	const ImGuiWindow* window = ImGui::GetCurrentWindow();
	window->DrawList->AddRect({ top.X - width * 0.5f, top.Y }, { top.X + width * 0.5f, bottom.Y }, ImGui::GetColorU32(color), 0.f, 15, 1.5f);
}

bool RenderingUtility::Render3DBox(AController* const controller, const FVector& origin, const FVector& extent, const FRotator& rotation, const ImVec4& color)
{
	FVector vertex[2][4];
	vertex[0][0] = { -extent.X, -extent.Y,  -extent.Z };
	vertex[0][1] = { extent.X, -extent.Y,  -extent.Z };
	vertex[0][2] = { extent.X, extent.Y,  -extent.Z };
	vertex[0][3] = { -extent.X, extent.Y, -extent.Z };
	vertex[1][0] = { -extent.X, -extent.Y, extent.Z };
	vertex[1][1] = { extent.X, -extent.Y, extent.Z };
	vertex[1][2] = { extent.X, extent.Y, extent.Z };
	vertex[1][3] = { -extent.X, extent.Y, extent.Z };
	FVector2D screen[2][4];
	FTransform const Transform(rotation);
	for (auto k = 0; k < 2; k++)
	{
		for (auto i = 0; i < 4; i++)
		{
			auto& vec = vertex[k][i];
			vec = Transform.TransformPosition(vec) + origin;
			if (!controller->ProjectWorldLocationToScreen(vec, screen[k][i])) return false;
		}
	}
	const auto ImScreen = reinterpret_cast<ImVec2(&)[2][4]>(screen);
	const auto window = ImGui::GetCurrentWindow();
	for (auto i = 0; i < 4; i++)
	{
		window->DrawList->AddLine(ImScreen[0][i], ImScreen[0][(i + 1) % 4], ImGui::GetColorU32(color));
		window->DrawList->AddLine(ImScreen[1][i], ImScreen[1][(i + 1) % 4], ImGui::GetColorU32(color));
		window->DrawList->AddLine(ImScreen[0][i], ImScreen[1][i], ImGui::GetColorU32(color));
	}
	return true;
}
