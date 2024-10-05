// PirateSense.

#include "ViewModule.h"

#include <ThirdParty/XORSTR.h>

#include "Config.h"
#include "PirateSense.h"

ViewModule::ViewModule()
{
    ModuleName = xorstr_(L"View Module");
}

void ViewModule::Update()
{
    if (!PirateSense::PlayerController || !PirateSense::LocalPlayerActor)
    {
        return;
    }

    const UWieldedItemComponent* ItemComponent = PirateSense::LocalPlayerActor->WieldedItemComponent;
    AActor* Item = ItemComponent ? ItemComponent->CurrentlyWieldedItem : nullptr;
    const AProjectileWeapon* LocalProjectileWeapon = Cast<AProjectileWeapon>(Item);
    const ASpyglass* Spyglass = Item ? Cast<ASpyglass>(Item) : nullptr;

    static uint8_t Pass = 0;

    const float FOV = UFOVHandlerFunctions::GetTargetFOV(PirateSense::LocalPlayerActor);
        
    PirateSense::PlayerController->FOV(Config.View.FOV.fov);
        
    if (FOV == 17.f)
    {
        PirateSense::PlayerController->FOV(Config.View.FOV.fov * 0.2f);
    }
    if (Spyglass && GetAsyncKeyState(VK_LBUTTON) && !Config.View.FOV.bSpyRClickMode && Spyglass->BlurTime > 0.51f)
    {
        PirateSense::PlayerController->FOV(Config.View.FOV.fov * (1.f / Config.View.FOV.spyglassFovMul));
    }
    else if (Spyglass && GetAsyncKeyState(VK_RBUTTON) && Config.View.FOV.bSpyRClickMode)
    {
        PirateSense::PlayerController->FOV(Config.View.FOV.fov * (1.f / Config.View.FOV.spyglassFovMul));
    }
    else if (LocalProjectileWeapon && GetAsyncKeyState(VK_RBUTTON))
    {
        Pass++;

        const std::string& ItemName = Item->GetName();
        if (Pass >= 2 && ItemName.find("sniper_rifle_") != std::string::npos)
        {
            PirateSense::PlayerController->FOV(Config.View.FOV.fov * (1.f / Config.View.FOV.sniperFovMul));
        }
    }
    else
    {
        Pass = 0;
    }
}

void ViewModule::RenderMenu()
{
    if (ImGui::BeginTabItem(xorstr_("View")))
    {
        ImGui::Text(xorstr_("View"));

        ImGui::Columns(1, xorstr_("ViewStuff"), false);
        ImGui::Text(xorstr_("FOV"));
        if (ImGui::BeginChild(xorstr_("FOV"), ImVec2(0.f, 0.f), true, 0))
        {
            ImGui::Checkbox(xorstr_("Change FOV"), &Config.View.FOV.bFovEnable);
            ImGui::SliderFloat(xorstr_("FOV Value"), &Config.View.FOV.fov, 60.f, 150.f, xorstr_("%.0f"));
            ImGui::Checkbox(xorstr_("Spyglass Right Click"), &Config.View.FOV.bSpyRClickMode);
            ImGui::SliderFloat(xorstr_("Spyglass FOV Factor"), &Config.View.FOV.spyglassFovMul, 1.f, 25.f, xorstr_("%.0f"));
            ImGui::SliderFloat(xorstr_("Eye of Reach FOV Factor"), &Config.View.FOV.sniperFovMul, 1.f, 25.f, xorstr_("%.0f"));
        }
        ImGui::EndChild();

        ImGui::Columns();
        ImGui::EndTabItem();
    }
}
