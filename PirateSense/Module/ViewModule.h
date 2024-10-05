// PirateSense.

#pragma once

#include "ModuleInterface.h"

class ViewModule : public ModuleInterface
{
public:
    ViewModule();
    
    virtual void Update() override;
    virtual void RenderMenu() override;
};
