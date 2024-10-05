// PirateSense.

#pragma once

#include "ModuleInterface.h"

class MiscModule : public ModuleInterface
{
public:
    MiscModule();
    
    virtual void Update() override;
    virtual void RenderMenu() override;
    virtual void RenderOverlay(ImDrawList* DrawList) override;
    virtual void PostRender() override;
};
