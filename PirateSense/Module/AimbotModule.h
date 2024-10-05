// PirateSense.

#pragma once

#include <d3d11.h>

#include "ModuleInterface.h"
#include "SDK/SDK.h"

class AimbotModule : public ModuleInterface
{
public:
    float Best = FLOAT_MAX;
    AActor* Target = nullptr; 
    FVector Location;
    FRotator DeltaRotation;
    float Smoothness = 0.f;
    
public:
    AimbotModule();
    
    virtual void Update() override;
    virtual void RenderMenu() override;
    virtual void RenderOverlay(ImDrawList* DrawList) override;
};
