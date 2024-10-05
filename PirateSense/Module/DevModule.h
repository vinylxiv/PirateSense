// PirateSense.

#pragma once

#include "ModuleInterface.h"

class DevModule : public ModuleInterface
{
public:
    DevModule();
    
    virtual void RenderMenu() override;
    virtual void RenderOverlay(ImDrawList* DrawList) override;
};
