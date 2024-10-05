// PirateSense.

#pragma once

#include <imgui.h>
#include <string>
#include <vector>

#include "Utility/Logging.h"

class ModuleInterface
{
public:
    std::wstring ModuleName = xorstr_(L"Unknown Module");
    
public:
    ModuleInterface() {}
    virtual ~ModuleInterface() {}

    // Called on PirateSense::Init
    virtual void Init() {}
    // ReSharper disable once CppMemberFunctionMayBeStatic
    void PostInit()
    {
        PIRATESENSE_DEVLOGARGS(LOG_Core, LogType::Info, xorstr_(L"Initialised Module: {0}"), std::vector<std::variant<std::wstring>>{ ModuleName });
    }
    // Called on PirateSense::Shutdown
    virtual void Shutdown()
    {
        PIRATESENSE_DEVLOGARGS(LOG_Core, LogType::Info, xorstr_(L"Shutting Down Module: {0}"), std::vector<std::variant<std::wstring>>{  });

        delete this;
    }
    // Called on PirateSense::OnPresent_Hook. Should be used for updating or setting data before rendering.
    virtual void Update() {}
    // Called on PirateSense::OnPresent_Hook. Called after ModuleInterface::Update, before ModuleInterface::RenderOverlay.
    virtual void RenderMenu() {}
    // Called on PirateSense::OnPresent_Hook. Called after ModuleInterface::RenderMenu, before ModuleInterface::PostRender.
    virtual void RenderOverlay(ImDrawList* DrawList) {}
    // Called on PirateSense::OnPresent_Hook. Called after ModuleInterface::RenderMenu and ModuleInterface::RenderOverlay, in case the module wants to draw an extra window or something.
    virtual void PostRender() {}

    static std::vector<ModuleInterface*>& GetModules() { return Modules; }

    static ModuleInterface* FindModule(const std::wstring& Name)
    {
        for (ModuleInterface* Module : Modules)
        {
            if (!Module || Module->ModuleName != Name)
            {
                continue;
            }

            return Module;
        }

        return nullptr;
    }

    template<typename T>
    static void AddModule()
    {
        static_assert(std::is_base_of_v<ModuleInterface, T>, "Attempting to add a module with type that isn't a ModuleInterface.");

        if (ModuleInterface* NewModule = new T)
        {
            NewModule->Init();
            NewModule->PostInit();
            
            Modules.push_back(NewModule);
        }
    }
    
    static void ShutdownModules()
    {
        for (ModuleInterface* Module : Modules)
        {
            if (!Module)
            {
                continue;
            }

            Module->Shutdown();
        }

        Modules.clear();
    }

private:
    static std::vector<ModuleInterface*> Modules;
};
