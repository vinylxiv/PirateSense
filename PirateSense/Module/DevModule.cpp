// PirateSense.

#include "DevModule.h"

#include <SDK/SDK.h>
#include <ThirdParty/XORSTR.h>

#include "Config.h"
#include "PirateSense.h"
#include "Utility/RenderingUtility.h"

DevModule::DevModule()
{
    ModuleName = xorstr_(L"Dev Module");
}

void DevModule::RenderMenu()
{
#ifdef DEVBUILD
    {
        if (ImGui::BeginTabItem(xorstr_("Dev")))
        {
            ImGui::Text(xorstr_("Dev"));

            ImGui::Columns(1, xorstr_("DevCol"), false);
            ImGui::Text(xorstr_("Dev"));
            if (ImGui::BeginChild(xorstr_("Dev"), ImVec2(0.f, 0.f), true, 0))
            {
                ImGui::Checkbox(xorstr_("Show Debug Names"), &Config.Dev.bDebugNames);
                ImGui::Text(xorstr_("Filter Keyword"));
                ImGui::InputText(xorstr_("Keyword"), Config.Dev.DebugNamesFilter, IM_ARRAYSIZE(Config.Dev.DebugNamesFilter));

                if (ImGui::Button(xorstr_("Force Crash")))
                {
                    const ACharacter* Dummy = nullptr;
                    std::string Result = Dummy->GetName();
                }
            }
            ImGui::EndChild();

            ImGui::Columns();
            ImGui::EndTabItem();
        }
    }
#endif // DEVBUILD
}

void DevModule::RenderOverlay(ImDrawList* DrawList)
{
    if (!PirateSense::LocalPlayerActor || !PirateSense::PlayerController || !PirateSense::Levels.Data)
    {
        return;
    }

    if (!Config.Dev.bDebugNames)
    {
        return;
    }
    
    for (uint32_t LevelIndex = 0; LevelIndex < PirateSense::Levels.Count; LevelIndex++)
    {
        const ULevel* Level = PirateSense::Levels[LevelIndex];
        if (!Level)
        {
            continue;
        }

        const TArray<AActor*>& Actors = Level->AActors;
        if (!Actors.Data)
        {
            continue;
        }

        const FVector& LocalPlayerLocation = PirateSense::LocalPlayerActor->K2_GetActorLocation();

        for (uint32_t ActorIndex = 0; ActorIndex < Actors.Count; ActorIndex++)
        {
            AActor* Actor = Actors[ActorIndex];
            if (!Actor)
            {
                continue;
            }
            
            const FVector& ActorLocation = Actor->K2_GetActorLocation();
            const float Distance = LocalPlayerLocation.DistTo(ActorLocation) / 100.f;
            
            if (Distance <= Config.Dev.DebugRenderDistance)
            {
                FVector2D ScreenLocation;
                if (PirateSense::PlayerController->ProjectWorldLocationToScreen(ActorLocation, ScreenLocation))
                {
                    const std::string& ActorName = Actor->GetName();
                    const std::string& DebugNamesFilter = Config.Dev.DebugNamesFilter;
                    
                    if (DebugNamesFilter.empty() || DebugNamesFilter.find(ActorName) != std::string::npos)
                    {
                        RenderingUtility::RenderText(ActorName.c_str(), ScreenLocation, { 1.f,1.f,1.f,1.f }, false, true);
                    }
                }
            }
        }
    }
}
