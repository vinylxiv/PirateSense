// PirateSense.

#include "MiscModule.h"

#include <format>
#include <imgui_internal.h>
#include <ThirdParty/XORSTR.h>

#include "Config.h"
#include "PirateSense.h"
#include "SDK/SDK.h"
#include "Utility/RenderingUtility.h"

MiscModule::MiscModule()
{
	ModuleName = xorstr_(L"Misc Module");
}

void MiscModule::Update()
{
	if (!PirateSense::LocalPlayerActor || !PirateSense::PlayerController || !PirateSense::Levels.Data)
	{
		return;
	}

	if (!Config.Misc.bTimeOfDay && !Config.Misc.bNoFog)
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

		for (uint32_t ActorIndex = 0; ActorIndex < Actors.Count; ActorIndex++)
		{
			AActor* Actor = Actors[ActorIndex];
			if (!Actor)
			{
				continue;
			}

			if (ALightingController* LightingController = Actor ? Cast<ALightingController>(Actor) : nullptr)
			{
				LightingController->IsFixedTimeOfDay = Config.Misc.bTimeOfDay;
				
				if (Config.Misc.bTimeOfDay)
				{
					LightingController->FixedTimeOfDay = Config.Misc.TimeOfDay;
				}

				if (Config.Misc.bNoFog)
				{
					LightingController->ToggleFog(LightingController, 0x7A8, true);
					LightingController->ToggleFog(LightingController, 0x7B0, true);

				}
				else
				{
					LightingController->ToggleFog(LightingController, 0x7A8, false);
					LightingController->ToggleFog(LightingController, 0x7B0, false);
				}
			}
		}
	}
}

void MiscModule::RenderMenu()
{
    if (ImGui::BeginTabItem(xorstr_("Misc")))
    {
        ImGui::Text(xorstr_("Misc"));

        ImGui::Columns(3, xorstr_("ActorMiscCol"), false);
        ImGui::Text(xorstr_("Extra"));
        if (ImGui::BeginChild(xorstr_("Ships"), ImVec2(0.f, 0.f), true, 0))
        {
            ImGui::Checkbox(xorstr_("Enable"), &Config.Misc.Ship.bEnable);
            ImGui::Checkbox(xorstr_("Player List"), &Config.Misc.Ship.bPlayerlist);
            ImGui::Checkbox(xorstr_("Ship Info"), &Config.Misc.Ship.bShipInfo);

            ImGui::Checkbox(xorstr_("Compass"), &Config.Misc.bCompass);
            ImGui::Checkbox(xorstr_("Anti AFK Kick"), &Config.Misc.bAntiAFKKick);
            ImGui::Checkbox(xorstr_("Oxygen Bar"), &Config.Misc.bOxygen);

            ImGui::Checkbox(xorstr_("Custom Time of Day"), &Config.Misc.bTimeOfDay);
            ImGui::SliderFloat(xorstr_("Time"), &Config.Misc.TimeOfDay, 1.f, 24.f, xorstr_("%.0f:00"));

            ImGui::Checkbox(xorstr_("No Fog"), &Config.Misc.bNoFog);
        }
        ImGui::EndChild();

        ImGui::NextColumn();
        ImGui::Text(xorstr_("Crosshair"));
        if (ImGui::BeginChild(xorstr_("Crosshair"), ImVec2(0.f, 0.f), true, 0))
        {
            ImGui::Checkbox(xorstr_("CrossHair"), &Config.Misc.bCrosshair);
            ImGui::SliderFloat(xorstr_("Size"), &Config.Misc.CrosshairSize, 1.f, 50.f, xorstr_("%.0f"));
            ImGui::SliderFloat(xorstr_("Thickness"), &Config.Misc.CrosshairThickness, 1.f, 50.f, xorstr_("%.0f"));
            ImGui::ColorEdit4(xorstr_("Color"), &Config.Misc.CrosshairColor.x, 0);
        }
        ImGui::EndChild();

        ImGui::Columns();
        ImGui::EndTabItem();
    }
}

void MiscModule::RenderOverlay(ImDrawList* DrawList)
{
	if (!PirateSense::LocalPlayerActor || !PirateSense::PlayerController)
	{
		return;
	}
	
	AShip* Ship = Cast<AShip>(PirateSense::LocalPlayerActor->GetCurrentShip());
    if (Ship && Config.Misc.Ship.bEnable && Config.Misc.Ship.bShipInfo)
    {
        const FVector& Velocity = Ship->GetVelocity() / 100.f;
    	const float Speed = Velocity.Size();

        FVector2D ScreenPosition = FVector2D(10.f, 50.f);

    	const std::string SpeedString = std::vformat(xorstr_("Speed: {0}m/s"), std::make_format_args(Speed));
        RenderingUtility::RenderText(SpeedString.c_str(), ScreenPosition, ImVec4( 1.f, 1.f, 1.f, 1.f), false, false);
		ScreenPosition.Y += 20.f;

    	const AHullDamage* HullDamage = Ship->GetHullDamage();
        int32_t Holes = HullDamage ? HullDamage->ActiveHullDamageZones.Count : 0;
    	
    	const std::string HolesString = std::vformat(xorstr_("Holes: {0}"), std::make_format_args(Holes));
        RenderingUtility::RenderText(HolesString.c_str(), ScreenPosition, ImVec4( 1.f, 1.f, 1.f, 1.f), false, false);
    	ScreenPosition.Y += 20.f;

    	AShipInternalWater* Water = Ship->GetInternalWater();
        int32_t WaterAmount = Water ? static_cast<int32_t>(Water->GetNormalizedWaterAmount() * 100.f) : 0;
		
    	const std::string WaterString = std::vformat(xorstr_("Water: {0}%"), std::make_format_args(WaterAmount));
        RenderingUtility::RenderText(WaterString.c_str(), ScreenPosition, ImVec4( 1.f, 1.f, 1.f, 1.f), false, false);
    	ScreenPosition.Y += 20.f;

        const float internal_water_percent = Water ? Water->GetNormalizedWaterAmount() : 0.f;
        DrawList->AddLine({ ScreenPosition.X - 1.f, ScreenPosition.Y }, { ScreenPosition.X + 100 + 1.f, ScreenPosition.Y }, 0xFF000000, 6);
        DrawList->AddLine({ ScreenPosition.X, ScreenPosition.Y }, { ScreenPosition.X + 100.f, ScreenPosition.Y }, 0xFF00FF00, 4);
        DrawList->AddLine({ ScreenPosition.X, ScreenPosition.Y }, { ScreenPosition.X + (100.f * internal_water_percent), ScreenPosition.Y }, 0xFF0000FF, 4);
    }

	const UWieldedItemComponent* ItemComponent = PirateSense::LocalPlayerActor->WieldedItemComponent;
	AActor* Item = ItemComponent ? ItemComponent->CurrentlyWieldedItem : nullptr;
	const AMeleeWeapon* LocalSword = Cast<AMeleeWeapon>(Item);
    if (LocalSword && Config.Misc.Sword.bEnable)
    {
    	LocalSword->DataAsset->BlockingMovementSpeed = Config.Misc.Sword.bFastSword ? 3 : 2;
    	
    	LocalSword->DataAsset->HeavyAttack->ClampYawRange = Config.Misc.Sword.bNoClamp ? -1.f : 90.f;
    }

	AProjectileWeapon* LocalProjectileWeapon = Cast<AProjectileWeapon>(Item);
    if (LocalProjectileWeapon && Config.Misc.Weapon.bEnable)
    {
    	LocalProjectileWeapon->WeaponParameters.TimeoutTolerance = Config.Misc.Weapon.bFastSwitch ? 0.f : 0.25f;
    }

	const ImGuiIO& IO = ImGui::GetIO();
	if (Config.Misc.bCrosshair)
	{
		DrawList->AddLine({ IO.DisplaySize.x * 0.5f - Config.Misc.CrosshairSize, IO.DisplaySize.y * 0.5f }, { IO.DisplaySize.x * 0.5f + Config.Misc.CrosshairSize, IO.DisplaySize.y * 0.5f }, ImGui::GetColorU32(Config.Misc.CrosshairColor), Config.Misc.CrosshairThickness);
		DrawList->AddLine({ IO.DisplaySize.x * 0.5f, IO.DisplaySize.y * 0.5f - Config.Misc.CrosshairSize }, { IO.DisplaySize.x * 0.5f, IO.DisplaySize.y * 0.5f + Config.Misc.CrosshairSize }, ImGui::GetColorU32(Config.Misc.CrosshairColor), Config.Misc.CrosshairThickness);
	}

	if (PirateSense::PlayerController)
	{
		if (AOnlineAthenaPlayerController* OnlineAthenaPlayerController = Cast<AOnlineAthenaPlayerController>(PirateSense::PlayerController))
		{
			OnlineAthenaPlayerController->IdleDisconnectEnabled = !Config.Misc.bAntiAFKKick;
		}

		if (PirateSense::PlayerController->PlayerCameraManager && Config.Misc.bCompass)
		{
			const FRotator& CameraRotation = PirateSense::PlayerController->PlayerCameraManager->GetCameraRotation();
			const char* Directions[] = { xorstr_("North"), xorstr_("North East"), xorstr_("East"), xorstr_("South East"), xorstr_("South"), xorstr_("South West"), xorstr_("West"), xorstr_("North West") };
			
			const int32_t Yaw = (static_cast<int32_t>(CameraRotation.Yaw) + 450) % 360;
			const int32_t DirectionIndex = static_cast<int32_t>(static_cast<int32_t>(Yaw + 22.5f) % 360 * 0.0222222f);

			FVector2D ScreenPosition = { IO.DisplaySize.x * 0.5f, IO.DisplaySize.y * 0.02f };
			
			RenderingUtility::RenderText(Directions[DirectionIndex], ScreenPosition, ImVec4(1.f, 1.f, 1.f, 1.f), false, true);
			ScreenPosition.Y += 20.f;
			
			const std::string YawString = std::vformat(xorstr_("{0}"), std::make_format_args(Yaw));
			RenderingUtility::RenderText(YawString.c_str(), ScreenPosition, ImVec4(1.f, 1.f, 1.f, 1.f), false, true);
		}
	}

	UDrowningComponent* DrowningComponent = PirateSense::LocalPlayerActor->DrowningComponent;
	if (DrowningComponent && Config.Misc.bOxygen && PirateSense::LocalPlayerActor->IsInWater())
	{
		const float OxygenLevel = DrowningComponent->GetOxygenLevel();
		
		const float PositionX = IO.DisplaySize.x * 0.5f;
		const float PositionY = IO.DisplaySize.y * 0.85f;
		const float BarWidth = IO.DisplaySize.x * 0.05f;
		const float BarHeight = IO.DisplaySize.y * 0.0030f;
		
		DrawList->AddRectFilled({ PositionX - BarWidth, PositionY - BarHeight }, { PositionX + BarWidth, PositionY + BarHeight }, ImGui::GetColorU32(IM_COL32(0, 0, 0, 255)));
		DrawList->AddRectFilled({ PositionX - BarWidth, PositionY - BarHeight }, { PositionX - BarWidth + BarWidth * OxygenLevel * 2.f, PositionY + BarHeight }, ImGui::GetColorU32(IM_COL32(0, 200, 255, 255)));
	}
}

void MiscModule::PostRender()
{
	if (!PirateSense::GameState)
	{
		return;
	}
	
    if (Config.Misc.Ship.bPlayerlist)
	{
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);
		ImGui::SetNextWindowSize(ImVec2(250.f, 400.f), ImGuiCond_Once);
		ImGui::SetNextWindowPos(ImVec2(10.f, 180.f), ImGuiCond_Once);
		ImGui::Begin(xorstr_("PlayersList"), nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_AlwaysAutoResize | ImGuiColumnsFlags_NoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);

    	if (PirateSense::GameState->ShipService)
		{
			ImGui::BeginChild(xorstr_("Info"), { 0.f, 18.f });
			ImGui::Text(xorstr_("Playerlist"));
			ImGui::EndChild();
		}

		if (const ACrewService* CrewService = PirateSense::GameState->CrewService)
		{
			const TArray<FCrew>& Crews = CrewService->Crews;
			if (Crews.Data)
			{
				ImGui::Separator();
				for (uint32_t CrewIndex = 0; CrewIndex < Crews.Count; CrewIndex++)
				{
					const FCrew& Crew = Crews[CrewIndex];

					FCrewColor CrewColorToUse = FCrewColor(FGuid(), ImVec4(1.f, 1.f, 1.f, 1.f));

					// bool bFound = false;
					// for (auto& CrewColor : PirateSense::CrewColors)
					// {
					// 	if (CrewColor.Id == Crew.CrewId)
					// 	{
					// 		bFound = true;
					// 		CrewColorToUse = CrewColor;
					// 	}
					// }
					// if (!bFound)
					// {
					// 	const float R = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
					// 	const float G = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
					// 	const float B = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
					//
					// 	CrewColorToUse = FCrewColor(Crew.CrewId, ImVec4(R, G, B, 1.f));
					//
					// 	PirateSense::CrewColors.push_back(CrewColorToUse);
					// }

					const TArray<APlayerState*>& Players = Crew.Players;
					if (Players.Data)
					{
						switch (Crew.CrewSessionTemplate.MaxPlayers)
						{
						case 2:
							ImGui::TextColored(CrewColorToUse.Color, xorstr_("Sloop"));
							break;
						case 3:
							ImGui::TextColored(CrewColorToUse.Color, xorstr_("Brig"));
							break;
						case 4:
							ImGui::TextColored(CrewColorToUse.Color, xorstr_("Galleon"));
							break;
						default:
							ImGui::TextColored(CrewColorToUse.Color, xorstr_("Ship"));
							break;
						}

						for (uint32_t PlayerIndex = 0; PlayerIndex < Players.Count; PlayerIndex++)
						{
							APlayerState* Player = Players[PlayerIndex];
							if (!Player)
							{
								continue;
							}

							const char* Actions[] = { xorstr_("None"), xorstr_("Bailing"), xorstr_("Cannon"), xorstr_("Cannon"), xorstr_("Capstan"), xorstr_("Capstan"), xorstr_("Carrying Booty"), xorstr_("Carrying Booty"), xorstr_("Dead"), xorstr_("Dead"), xorstr_("Digging"), xorstr_("Dousing"), xorstr_("Emptying Bucket"), xorstr_("Harpoon"), xorstr_("Harpoon"), xorstr_("LoseHealth"), xorstr_("Repairing"), xorstr_("Sails"), xorstr_("Sails"), xorstr_("Wheel"), xorstr_("Wheel") };
							EPlayerActivityType CurrentActivity = Player->GetPlayerActivity();
							const uint8_t CurrentActivityAsByte = static_cast<uint8_t>(CurrentActivity);
							std::string ActivityString = CurrentActivityAsByte < 21 ? Actions[CurrentActivityAsByte] : xorstr_("None");

							std::string PlayerNameString;
							if (Player->PlayerName.Count > 0)
							{
								PlayerNameString.resize(Player->PlayerName.Count);
								[[maybe_unused]] int32_t Result = WideCharToMultiByte(CP_UTF8, 0, Player->PlayerName.Data, Player->PlayerName.Count, PlayerNameString.data(), static_cast<int32_t>(wcslen(Player->PlayerName.Data)), nullptr, nullptr) - 1;
							}
							else
							{
								PlayerNameString = xorstr_("???");
							}

							std::string PlayerString;
							PlayerString = std::vformat(xorstr_("{} | {}"), std::make_format_args(PlayerNameString, ActivityString));
							
							ImGui::Text(PlayerString.c_str());
							ImGui::NextColumn();
						}
						ImGui::Text(xorstr_(""));
						ImGui::Separator();
					}
				}
			}
		}
		ImGui::End();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	}
}
