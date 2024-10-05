// PirateSense.

#pragma once

#include <imgui.h>

#include <SDK/SDK.h>

class RenderingUtility
{
public:
    static void RenderText(const char* text, const FVector2D& pos, const ImVec4& color, const bool outlined, const bool centered);
    static void Render2DBox(const FVector2D& top, const FVector2D& bottom, const float height, const float width, const ImVec4& color);
    static bool Render3DBox(AController* constcontroller, const FVector& origin, const FVector& extent, const FRotator& rotation, const ImVec4& color);
    //static bool Render3DBox(AController* constcontroller, const FVector& origin, const FVector& extent, const FRotator& rotation, const ImVec4& color);
    //static bool RenderSkeleton(AController* const controller, USkeletalMeshComponent* const mesh, const FMatrix& comp2world, const std::pair<const BYTE*, const BYTE>* skeleton, int size, const ImVec4& color);
};
