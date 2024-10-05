// PirateSense.

#pragma once

#include "ModuleInterface.h"

class ESPModule : public ModuleInterface
{
public:
    ESPModule();
    
    virtual void RenderMenu() override;
    virtual void RenderOverlay(ImDrawList* DrawList) override;
};
