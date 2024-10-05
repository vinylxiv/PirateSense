// PirateSense.

#include "ESPModule.h"

#include <format>
#include <ThirdParty/XORSTR.h>

#include "Config.h"
#include "PirateSense.h"
#include "Utility/RenderingUtility.h"

ESPModule::ESPModule()
{
	ModuleName = xorstr_(L"ESP Module");
}

void ESPModule::RenderMenu()
{
	if (ImGui::BeginTabItem(xorstr_("ESP")))
	{
		ImGui::Text(xorstr_("ESP"));

		ImGui::Columns(4, xorstr_("ActorEspCol"), false);

		ImGui::Text(xorstr_("Players"));
		if (ImGui::BeginChild(xorstr_("PlayersESP"), ImVec2(0.f, 0.f), true, 0))
		{
			ImGui::Checkbox(xorstr_("Enable"), &Config.ESP.Players.bEnable);
			ImGui::SliderFloat(xorstr_("Distance"), &Config.ESP.Players.RenderDistance, 1.f, 2000.f, xorstr_("%.0f"));
			ImGui::ColorEdit4(xorstr_("Visible Color"), &Config.ESP.Players.ColorVisible.x, 0);
			ImGui::ColorEdit4(xorstr_("Invisible Color"), &Config.ESP.Players.ColorInvisible.x, 0);
			ImGui::Checkbox(xorstr_("Teammates"), &Config.ESP.Players.bTeam);
			ImGui::Checkbox(xorstr_("Tracers"), &Config.ESP.Players.bTracers);
			ImGui::SliderFloat(xorstr_("Thickness"), &Config.ESP.Players.TracersThickness, 1.f, 10.f, xorstr_("%.0f"));

			ImGui::Checkbox(xorstr_("Items"), &Config.ESP.Players.bItems);
			ImGui::Checkbox(xorstr_("Health"), &Config.ESP.Players.bHealth);
		}
		ImGui::EndChild();

		ImGui::NextColumn();
		ImGui::Text(xorstr_("Mobs"));
		if (ImGui::BeginChild(xorstr_("MobsESP"), ImVec2(0.f, 0.f), true, 0))
		{
			ImGui::Checkbox(xorstr_("Skeletons"), &Config.ESP.Skeletons.bEnable);
			ImGui::Checkbox(xorstr_("Skeleton Items"), &Config.ESP.Skeletons.bItems);
			ImGui::Checkbox(xorstr_("Skeleton Names"), &Config.ESP.Skeletons.bNames);
			
			ImGui::Checkbox(xorstr_("Ocean Crawlers"), &Config.ESP.OceanCrawler.bEnable);
			ImGui::Checkbox(xorstr_("Ocean Crawler Items"), &Config.ESP.OceanCrawler.bItems);
		}
		ImGui::EndChild();

		ImGui::NextColumn();
		ImGui::Text(xorstr_("Ships"));
		if (ImGui::BeginChild(xorstr_("ShipsESP"), ImVec2(0.f, 0.f), true, 0))
		{
			ImGui::Checkbox(xorstr_("Enable"), &Config.ESP.Ships.bEnable);
			ImGui::Checkbox(xorstr_("Skeletons"), &Config.ESP.Ships.bSkeletons);
			ImGui::Checkbox(xorstr_("Ghosts"), &Config.ESP.Ships.bGhosts);
			ImGui::SliderFloat(xorstr_("Distance"), &Config.ESP.Ships.RenderDistance, 1.f, 2000.f, xorstr_("%.0f"));
		}
		ImGui::EndChild();

		ImGui::NextColumn();
		ImGui::Text(xorstr_("Misc"));
		if (ImGui::BeginChild(xorstr_("MiscESP"), ImVec2(0.f, 0.f), true, 0))
		{
			ImGui::Checkbox(xorstr_("Events"), &Config.ESP.Misc.bEnable);
			
			ImGui::Checkbox(xorstr_("Events"), &Config.ESP.Events.bEnable);
			ImGui::SliderFloat(xorstr_("Distance"), &Config.ESP.Events.RenderDistance, 1.f, 10000.f, xorstr_("%.0f"));

			ImGui::Checkbox(xorstr_("Mermaids"), &Config.ESP.Misc.bMermaids);
			ImGui::Checkbox(xorstr_("Rowboats"), &Config.ESP.Misc.Rowboats.bRowboats);
			ImGui::Checkbox(xorstr_("Rowboats Health"), &Config.ESP.Misc.Rowboats.bRowboatHealth);
			ImGui::Checkbox(xorstr_("Rowboats Livery"), &Config.ESP.Misc.Rowboats.bRowboatLivery);
			ImGui::Checkbox(xorstr_("Rowboat Items"), &Config.ESP.Misc.Rowboats.bRowboatItems);
			ImGui::Checkbox(xorstr_("Sharks"), &Config.ESP.Misc.bShark);
			ImGui::Checkbox(xorstr_("Shipwrecks"), &Config.ESP.Misc.bShipwrecks);
			ImGui::Checkbox(xorstr_("Map Pins"), &Config.ESP.Misc.bMapPins);
			ImGui::Checkbox(xorstr_("Megalodon"), &Config.ESP.Misc.bMegalodon);
			ImGui::Checkbox(xorstr_("Items"), &Config.ESP.Misc.bItems);
			ImGui::Checkbox(xorstr_("Barrels"), &Config.ESP.Misc.bBarrels);
			ImGui::Checkbox(xorstr_("Barrel Items"), &Config.ESP.Misc.BBarrelItems);

			ImGui::Checkbox(xorstr_("Tracked Ships"), &Config.ESP.Misc.bTrackedShips);
			ImGui::Checkbox(xorstr_("Tracked Booty"), &Config.ESP.Misc.bTrackedBooty);

			ImGui::Checkbox(xorstr_("Storm"), &Config.ESP.Misc.bStorm);
			ImGui::Checkbox(xorstr_("FogBank"), &Config.ESP.Misc.bFogBank);
			ImGui::Checkbox(xorstr_("Ammo"), &Config.ESP.Misc.bAmmo);
			ImGui::Checkbox(xorstr_("NPC"), &Config.ESP.Misc.bNPC);
			ImGui::Checkbox(xorstr_("Seagulls"), &Config.ESP.Misc.bSeagulls);
			ImGui::Checkbox(xorstr_("Mermaid Statues"), &Config.ESP.Misc.bMermaidStatues);

			//ImGui::Checkbox(xorstr_("Vote Table"), &Config.ESP.Misc.bEmissaryTables);

			//ImGui::Checkbox(xorstr_("Net Proxy"), &Config.ESP.Misc.bNetProxy);
		}
		ImGui::EndChild();

		ImGui::Columns();
		ImGui::EndTabItem();
	}
}

void ESPModule::RenderOverlay(ImDrawList* DrawList)
{
	if (!PirateSense::LocalPlayerActor || !PirateSense::PlayerController || !PirateSense::Levels.Data)
	{
		return;
	}

	if (!Config.ESP.Events.bEnable && !Config.ESP.Misc.bEnable && !Config.ESP.Players.bEnable && !Config.ESP.Ships.bEnable && !Config.ESP.Skeletons.bEnable && !Config.ESP.OceanCrawler.bEnable)
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
			const FRotator& ActorRotation = Actor->K2_GetActorRotation();
			const float ActorDistance = LocalPlayerLocation.DistTo(ActorLocation) / 100.f;
			const float Velocity = Actor->GetVelocity().Size() / 100.f;
			FVector Origin, Extent;
			Actor->GetActorBounds(true, Origin, Extent);
			
			FVector2D ScreenPosition;
			if (PirateSense::PlayerController->ProjectWorldLocationToScreen(ActorLocation, ScreenPosition))
			{
				continue;
			}

			FVector2D TopPos;
			PirateSense::PlayerController->ProjectWorldLocationToScreen({ ActorLocation.X, ActorLocation.Y, ActorLocation.Z + Extent.Z }, TopPos);
			FVector2D BottomPos;
			PirateSense::PlayerController->ProjectWorldLocationToScreen({ ActorLocation.X, ActorLocation.Y, ActorLocation.Z - Extent.Z }, BottomPos);
			const float Height = abs(BottomPos.Y - TopPos.Y);
			
			if (Config.ESP.Ships.bEnable)
			{
				FVector ShipActorLocation = ActorLocation;
				ShipActorLocation.Z = 3000;
\
				AShip* Ship = Cast<AShip>(Actor);
				AShipNetProxy* ShipNetProxy = Cast<AShipNetProxy>(Actor);
				if (!Ship && !ShipNetProxy)
				{
					continue;
				}
				
				if (ActorDistance > Config.ESP.Ships.RenderDistance)
				{
					continue;
				}
				
				if (Ship && ActorDistance < 1726.f) // @todo: Move distance thresholds to a header file where they can all be accessed ez.
				{
					float InternalWater = 0.f;
					if (Ship && Ship->GetInternalWater())
					{
						InternalWater = Ship->GetInternalWater()->GetNormalizedWaterAmount() * 100.f;
					}

					FCrewColor CrewColorToUse = FCrewColor(FGuid(), ImVec4(1.f, 0.5f, 0.f, 1.f));
					// if (Ship && Ship->CrewOwnershipComponent)
					// {
					// 	for (auto& CrewColor : PirateSense::CrewColors)
					// 	{
					// 		if (Ship->CrewOwnershipComponent->CachedCrewId == CrewColor.Id)
					// 		{
					// 			CrewColorToUse = CrewColor;
					// 			break;
					// 		}
					// 	}
					// }
					
					AHullDamage* HullDamage = Cast<AHullDamage>(Ship->HullDamage);
					int32_t Holes = HullDamage ? HullDamage->ActiveHullDamageZones.Count : 0;

					const char* Directions[] = { xorstr_("N"), xorstr_("NE"), xorstr_("E"), xorstr_("SE"), xorstr_("S"), xorstr_("SW"), xorstr_("W"), xorstr_("NW") };
					int32_t Yaw = (static_cast<int32_t>(Actor->K2_GetActorRotation().Yaw) + 450) % 360;
					int32_t Index = static_cast<int32_t>(static_cast<int32_t>(Yaw + 22.5f) % 360 * 0.0222222f);

					uint32_t NumCharactersOnboard = 0;
					if (Ship->OnboardCharacterTrackerComponent && Ship->OnboardCharacterTrackerComponent->OnboardCharacters.Data)
					{
						NumCharactersOnboard = Ship->OnboardCharacterTrackerComponent->OnboardCharacters.Count;
					}
						
					std::string FlagName = "None";
					// Crashes at Flag->EmissaryFlagStaticMeshComponent->StaticMesh
					// if (ABP_LargeShipTemplate_C* Template = Ship->isLargeShipTemplate() ? reinterpret_cast<ABP_LargeShipTemplate_C*>(Ship) : nullptr)
					// {
					// 	AEmissaryFlag* Flag = Template && Template->BP_EmissaryFlag && Template->BP_EmissaryFlag->ChildActor && Template->BP_EmissaryFlag->ChildActor->isEmissaryFlag() ? reinterpret_cast<AEmissaryFlag*>(Template->BP_EmissaryFlag->ChildActor) : nullptr;
					// 	if (Flag && Flag->EmissaryFlagStaticMeshComponent && Flag->EmissaryFlagStaticMeshComponent->StaticMesh)
					// 	{
					// 		FlagName = Flag->EmissaryFlagStaticMeshComponent->StaticMesh->GetName();
					// 	}
					// 	
					// }
					// if (ABP_MediumShipTemplate_C* Template = Ship->isMediumShipTemplate() ? reinterpret_cast<ABP_MediumShipTemplate_C*>(Ship) : nullptr)
					// {
					// 	AEmissaryFlag* Flag = Template && Template->BP_EmissaryFlag && Template->BP_EmissaryFlag->ChildActor && Template->BP_EmissaryFlag->ChildActor->isEmissaryFlag() ? reinterpret_cast<AEmissaryFlag*>(Template->BP_EmissaryFlag->ChildActor) : nullptr;
					// 	// if (Flag && Flag->EmissaryFlagStaticMeshComponent && Flag->EmissaryFlagStaticMeshComponent->StaticMesh)
					// 	// {
					// 	// 	FlagName = Flag->EmissaryFlagStaticMeshComponent->StaticMesh->GetName();
					// 	// }
					// 	//std::cout << "MediumShipTempalte" << std::endl;
					// }
					// if (ABP_SmallShipTemplate_C* Template = Ship->isSmallShipTemplate() ? reinterpret_cast<ABP_SmallShipTemplate_C*>(Ship) : nullptr)
					// {
					// 	AEmissaryFlag* Flag = Template && Template->BP_EmissaryFlag && Template->BP_EmissaryFlag->ChildActor && Template->BP_EmissaryFlag->ChildActor->isEmissaryFlag() ? reinterpret_cast<AEmissaryFlag*>(Template->BP_EmissaryFlag->ChildActor) : nullptr;
					// 	if (Flag && Flag->EmissaryFlagStaticMeshComponent && Flag->EmissaryFlagStaticMeshComponent->StaticMesh)
					// 	{
					// 		FlagName = Flag->EmissaryFlagStaticMeshComponent->StaticMesh->GetName();
					// 		
					// 	}
					// 	//std::cout << "SmallShipTempalte" << std::endl;
					// }
						
					std::string ShipString;
					if (Actor->NameContains(xorstr_("BP_Small")))
					{
						ShipString = xorstr_("Sloop");
					}
					else if (Actor->NameContains(xorstr_("BP_Medium")))
					{
						ShipString = xorstr_("Brig");
					}
					else if (Actor->NameContains(xorstr_("BP_Large")))
					{
						ShipString = xorstr_("Galleon");
					}
					else if (Actor->NameContains(xorstr_("AISmall")) && Config.ESP.Ships.bSkeletons)
					{
						ShipString = xorstr_("Skelly Sloop");
					}
					else if (Actor->NameContains(xorstr_("AILarge")) && Config.ESP.Ships.bSkeletons)
					{
						ShipString = xorstr_("Skelly Galleon");
					}
					
					if (Ship->SinkingComponent ? Ship->SinkingComponent->IsSinking() : false)
					{
						ShipString +=  xorstr_(" vvv Sinking vvv");
					}
					else
					{
						ShipString += std::vformat(xorstr_(" ({0}%)<{1}>({2}m)({3}m/s)({4})({5})"), std::make_format_args(InternalWater, Holes, ActorDistance, Velocity, Directions[Index], NumCharactersOnboard));
					}

					RenderingUtility::RenderText(ShipString.c_str(), ScreenPosition, CrewColorToUse.Color, false, true);
				}
				else if (ShipNetProxy && ActorDistance > 1725.f)
				{
					std::string ShipString;
					if (Actor->NameContains(xorstr_("BP_Small")))
					{
						ShipString = xorstr_("Sloop");
					}
					else if (Actor->NameContains(xorstr_("BP_Medium")))
					{
						ShipString = xorstr_("Brig");
					}
					else if (Actor->NameContains(xorstr_("BP_Large")))
					{
						ShipString = xorstr_("Galleon");
					}
					else if (Actor->NameContains(xorstr_("AISmall")) && Config.ESP.Ships.bSkeletons)
					{
						ShipString = xorstr_("Skelly Sloop");
					}
					else if (Actor->NameContains(xorstr_("AILarge")) && Config.ESP.Ships.bSkeletons)
					{
						ShipString = xorstr_("Skelly Galleon");
					}
					
					ShipString += std::vformat(xorstr_(" ({0})"), std::make_format_args(ActorDistance));

					RenderingUtility::RenderText(ShipString.c_str(), ScreenPosition, ImVec4(1.f, 0.5f, 0.f, 1.f), false, true);
				}

				AAggressiveGhostShip* GhostShip = Cast<AAggressiveGhostShip>(Actor);
				if (Config.ESP.Ships.bGhosts && GhostShip)
				{
					int32_t Lives = GhostShip->NumShotsLeftToKill;
					std::string LivesString;
					for (int32_t Index = 0; Index < Lives; Index++)
					{
						LivesString += xorstr_("*");
					}
					
					const std::string ShipString = std::vformat(xorstr_("Ghost Ship ({0}) | {1}"), std::make_format_args(ActorDistance, LivesString));

					if (Lives > 0)
					{
						RenderingUtility::RenderText(ShipString.c_str(), ScreenPosition, ImVec4(0.2f, 1.f, 0.2f, 1.f), false, true);
					}
				}
			}

			AAthenaCharacter* Character = Cast<AAthenaCharacter>(Actor);
			AAthenaPlayerCharacter* PlayerCharacter = Cast<AAthenaPlayerCharacter>(Actor);

			if (Config.ESP.Players.bEnable)
			{
				if (PlayerCharacter && Actor != PirateSense::LocalPlayerActor && Character && !Character->IsDead())
				{
					if (ActorDistance >= Config.ESP.Players.RenderDistance)
					{
						continue;
					}

					APlayerState* PlayerState = Cast<AAthenaCharacter>(Actor)->PlayerState;
					const FString& PlayerName = PlayerState->PlayerName;
					if (!PlayerState || !PlayerName.Data)
					{
						continue;
					}

					std::string RawPlayerNameString;
					if (PlayerName.c_str())
					{
						const int32_t Size = static_cast<int32_t>(wcslen(PlayerName.c_str()));
						RawPlayerNameString.resize(Size);
						[[maybe_unused]] int32_t Result = WideCharToMultiByte(CP_UTF8, 0, PlayerName.c_str(), Size, RawPlayerNameString.data(), Size, nullptr, nullptr) - 1;
					}
					else
					{
						RawPlayerNameString = xorstr_("???");
					}
					
					const std::string PlayerNameString = std::vformat(xorstr_("{0} ({1}m)"), std::make_format_args(RawPlayerNameString, ActorDistance));
					const float HeightAdjustment = Height * 0.05f;
					FVector2D PlayerNamePosition = { TopPos.X, TopPos.Y - HeightAdjustment };
					RenderingUtility::RenderText(PlayerNameString.c_str(), PlayerNamePosition, ImVec4(1.f, 1.f, 1.f, 1.f), false, true);

					if (Character && Config.ESP.Players.bHealth)
					{
						UHealthComponent* HealthComponent = Character->HealthComponent;
						if (!HealthComponent)
						{
							continue;
						}

						const std::string HealthString = std::vformat(xorstr_("(HP: {0}%)"), std::make_format_args(HealthComponent->GetCurrentHealth() / HealthComponent->GetMaxHealth()));
						RenderingUtility::RenderText(HealthString.c_str(), BottomPos, ImVec4(1.f, 1.f, 1.f, 1.f), false, true);
					}

					if (Config.ESP.Players.bItems)
					{
						UWieldedItemComponent* WieldedItemComponent = Character->WieldedItemComponent;
						AWieldableItem* CurrentItem = WieldedItemComponent && WieldedItemComponent->CurrentlyWieldedItem ? Cast<AWieldableItem>(WieldedItemComponent->CurrentlyWieldedItem) : nullptr;
						AItemInfo* ItemInfo = CurrentItem ? CurrentItem->ItemInfo : nullptr;
						if (!ItemInfo || !ItemInfo->Desc)
						{
							continue;
						}

						std::string ItemNameString;
						if (ItemInfo->Desc->Title.TextData)
						{
							const int32_t Size = static_cast<int32_t>(wcslen(ItemInfo->Desc->Title.TextData->Text));
							ItemNameString.resize(Size);
							[[maybe_unused]] int32_t Result = WideCharToMultiByte(CP_UTF8, 0, ItemInfo->Desc->Title.TextData->Text, Size, ItemNameString.data(), Size, nullptr, nullptr) - 1;
						}
						else
						{
							ItemNameString = xorstr_("???");
						}
						
						RenderingUtility::RenderText(ItemNameString.c_str(), FVector2D(BottomPos.X, BottomPos.Y + 20), ImVec4(1.f, 1.f, 1.f, 1.f), false, true);
					}
				}
				else if (PlayerCharacter && Actor != PirateSense::LocalPlayerActor && PlayerCharacter && PlayerCharacter->IsDead())
				{
					if (ActorDistance >= Config.ESP.Players.RenderDistance)
					{
						continue;
					}

					APlayerState* PlayerState = PlayerCharacter->PlayerState;
					if (!PlayerState) 
					{
						continue;
					}
					const FString& PlayerName = PlayerState->PlayerName;
					if (!PlayerName.Data)
					{
						continue;
					}

					std::string RawPlayerNameString;
					if (PlayerName.c_str())
					{
						const int32_t Size = static_cast<int32_t>(wcslen(PlayerName.c_str()));
						RawPlayerNameString.resize(Size);
						[[maybe_unused]] int32_t Result = WideCharToMultiByte(CP_UTF8, 0, PlayerName.c_str(), Size, RawPlayerNameString.data(), Size, nullptr, nullptr) - 1;
					}
					else
					{
						RawPlayerNameString = xorstr_("???");
					}

					const std::string PlayerNameString = std::vformat(xorstr_("{0} Dead ({1}m)"), std::make_format_args(RawPlayerNameString, ActorDistance));
					const float HeightAdjustment = Height * 0.05f;
					FVector2D PlayerNamePosition = { TopPos.X, TopPos.Y - HeightAdjustment };
					RenderingUtility::RenderText(PlayerNameString.c_str(), PlayerNamePosition, ImVec4(1.f, 1.f, 1.f, 1.f), false, true);
				}
			}

			if (Config.ESP.Skeletons.bEnable)
			{
				FVector FixedExtent = FVector(40.f, 40.f, 120.f);

				AAthenaAICharacter* AICharacter = Cast<AAthenaAICharacter>(Actor);
				if (AICharacter && !Actor->IsA<AOceanCrawlerAICharacter>() && !AICharacter->IsDead())
				{
					if (!RenderingUtility::Render3DBox(PirateSense::PlayerController, ActorLocation, FixedExtent, ActorRotation, ImVec4(1.f, 0.5f, 0.f, 1.f)))
					{
						continue;
					}
						
					if (Config.ESP.Skeletons.bNames)
					{
						const std::string SkeletonString = std::vformat(xorstr_("Skeleton ({0}m)"), std::make_format_args(ActorDistance));
						RenderingUtility::RenderText(SkeletonString.c_str(), TopPos, ImVec4(1.f, 1.f, 1.f, 0.6f), false, true);
					}

					if (Character && Config.ESP.Skeletons.bItems)
					{
						UWieldedItemComponent* WieldedItemComponent = Character->WieldedItemComponent;
						AWieldableItem* CurrentItem = WieldedItemComponent && WieldedItemComponent->CurrentlyWieldedItem ? Cast<AWieldableItem>(WieldedItemComponent->CurrentlyWieldedItem) : nullptr;
						AItemInfo* ItemInfo = CurrentItem ? CurrentItem->ItemInfo : nullptr;
						if (!ItemInfo || !ItemInfo->Desc)
						{
							continue;
						}

						std::string ItemNameString;
						if (ItemInfo->Desc->Title.TextData)
						{
							const int32_t Size = static_cast<int32_t>(wcslen(ItemInfo->Desc->Title.TextData->Text));
							ItemNameString.resize(Size);
							[[maybe_unused]] int32_t Result = WideCharToMultiByte(CP_UTF8, 0, ItemInfo->Desc->Title.TextData->Text, Size, ItemNameString.data(), Size, nullptr, nullptr) - 1;
						}
						else
						{
							ItemNameString = xorstr_("???");
						}
									
						RenderingUtility::RenderText(ItemNameString.c_str(), FVector2D(BottomPos.X, BottomPos.Y + 20), ImVec4(1.f, 0.5f, 1.f, 1.f), false, true);
					}
				}
			}

			if (Config.ESP.Misc.bMermaids && Actor->IsA<AMermaid>())
			{
				const std::string MermaidString = std::vformat(xorstr_("Mermaid ({0}m)"), std::make_format_args(ActorDistance));
				RenderingUtility::RenderText(MermaidString.c_str(), ScreenPosition, ImVec4(1.f, 1.f, 1.f, 1.f), false, true);
			}

			if (Config.ESP.Misc.bStorm && Actor->IsA<AStorm>())
			{
				const std::string StormString = std::vformat(xorstr_("Storm ({0}m)"), std::make_format_args(ActorDistance));
				RenderingUtility::RenderText(StormString.c_str(), ScreenPosition, ImVec4(1.f, 1.f, 1.f, 1.f), false, true);
			}

			if (Config.ESP.Misc.bFogBank && Actor->IsA<AFogBank>())
			{
				const std::string FogBankString = std::vformat(xorstr_("Fog Bank ({0}m)"), std::make_format_args(ActorDistance));
				RenderingUtility::RenderText(FogBankString.c_str(), ScreenPosition, ImVec4(1.f, 1.f, 1.f, 1.f), false, true);
			}

			if (Config.ESP.Misc.bAmmo && Actor->IsA<AAmmoChest>())
			{
				const std::string AmmoString = std::vformat(xorstr_("Ammo ({0}m)"), std::make_format_args(ActorDistance));
				RenderingUtility::RenderText(AmmoString.c_str(), ScreenPosition, ImVec4(1.f, 1.f, 1.f, 1.f), false, true);
			}

			if (Config.ESP.Misc.bNPC && Actor->IsA<ANPC>())
			{
				const std::string NPCString = std::vformat(xorstr_("NPC ({0}m)"), std::make_format_args(ActorDistance));
				RenderingUtility::RenderText(NPCString.c_str(), ScreenPosition, ImVec4(1.f, 1.f, 1.f, 1.f), false, true);
			}
			
			if (Config.ESP.Misc.bSeagulls && Actor->IsA<ASeagulls>())
			{
				const std::string SeagullsString = std::vformat(xorstr_("Seagulls ({0}m)"), std::make_format_args(ActorDistance));
				RenderingUtility::RenderText(SeagullsString.c_str(), ScreenPosition, ImVec4(1.f, 1.f, 1.f, 1.f), false, true);
			}

			if (Config.ESP.Misc.bMermaidStatues && Actor->IsA<ASunkenCurseArtefact>())
			{
				std::string StatusTypeString = xorstr_("Statue");
				ImVec4 Color = ImVec4(1.f, 1.f, 1.f, 1.f);
				if (Actor->NameContains(xorstr_("Ruby")))
				{
					StatusTypeString = xorstr_("Ruby Statue");
					Color = ImVec4(1.f, 0.f, 0.f, 1.f);
				}
				if (Actor->NameContains(xorstr_("Emerald")))
				{
					StatusTypeString = xorstr_("Emerald Statue");
					Color = ImVec4(0.f, 1.f, 0.f, 1.f);
				}
				if (Actor->NameContains(xorstr_("Sapphire")))
				{
					StatusTypeString = xorstr_("Sapphire Statue");
					Color = ImVec4(0.f, 0.f, 1.f, 1.f);
				}
				
				const std::string MermaidString = std::vformat(xorstr_("{0} ({1}m)"), std::make_format_args(StatusTypeString.c_str(), ActorDistance));
				RenderingUtility::RenderText(MermaidString.c_str(), ScreenPosition, Color, false, true);
			}

			// if (Config.ESP.Misc.bEmissaryTables && Actor->IsA<AEmissaryVoteInteractionActor>())
			// {
			// 	// if (dist > cfg->esp.others.mermaidsRenderDistance)
			// 	// {
			// 	//	continue;
			// 	// {
			//
			// 	std::string VoteName = xorstr_("None");
			// 	AEmissaryVoteInteractionActor* EmmisaryTable = UObject::Cast<AEmissaryVoteInteractionActor>(Actor);
			// 	if (EmmisaryTable)
			// 	{
			// 		EmmisaryTable->EmissaryActiveInfo.Title.TextData->Text;
			// 	}
			//
			// 	const std::string EmissaryTableString = std::vformat(xorstr_("{0}"), std::make_format_args(EmmisaryTable->EmissaryActiveInfo.Title.TextData->Text));
			// 	RenderingUtility::RenderText(buf, screen, ImVec4(1.f, 1.f, 1.f, 1.f), false, true);
			// }

			if (Config.ESP.Misc.Rowboats.bRowboats && Actor->IsA<ARowboat>())
			{
				// if (ActorDistance > cfg->esp.others.rowboatsRenderDistance)
				// {
				// 	continue;
				// }
				
				bool bSinking = false;
					
				if (ARowboat* Rowboat = Cast<ARowboat>(Actor))
				{
					bSinking = Rowboat->IsSinking;
					// const TArray<FShipLivery>& LiveryDescs = Rowboat->LiveryComponent->LiveryDescs;
					// if (Config.ESP.Misc.Rowboats.bRowboatLivery)
					// {
					// 	for (uint32_t Index = 0; Index < LiveryDescs.Count; Index++)
					// 	{
					// 				
					// 	}
					// }

					UHealthComponent* HealthComponent = Rowboat->HealthComponent;
					if (Config.ESP.Misc.Rowboats.bRowboatHealth && HealthComponent)
					{
						const std::string HealthString = std::vformat(xorstr_("HP: {0}%"), std::make_format_args(HealthComponent->GetCurrentHealth() / HealthComponent->GetMaxHealth()));
						RenderingUtility::RenderText(HealthString.c_str(), BottomPos, ImVec4(1.f, 1.f, 1.f, 1.f), false, true);
					}
				}

				std::string RowboatString;
				if (Actor->NameContains(xorstr_("Cannon")))
				{
					RowboatString += xorstr_("Cannon Rowboat");
				}
				else if (Actor->NameContains(xorstr_("Harpoon")))
				{
					RowboatString += xorstr_("Harpoon Rowboat");
				}
				// else if (Actor->CompareName(xorstr_("Lantern")))
				// {
				// 	RowboatString += xorstr_("Lantern Rowboat");
				// }
				else if (Actor->NameContains(xorstr_("Rowboat")))
				{
					RowboatString += xorstr_("Rowboat");
				}

				RowboatString += std::vformat(xorstr_(" {0}"), std::make_format_args(ActorDistance));

				if (bSinking)
				{
					RowboatString += xorstr_(" vvv Sinking vvv");
				}
				
				RenderingUtility::RenderText(RowboatString.c_str(), ScreenPosition, ImVec4(1.f, 1.f, 1.f, 1.f), false, true);
			}

			AItemProxy* ItemProxy = Cast<AItemProxy>(Actor);
			if (Config.ESP.Misc.bItems && ItemProxy)
			{
				// if (ActorDistance > cfg->esp.items.renderDistance)
				// {
				// 	continue;
				// }

				if (UItemDesc* Description = ItemProxy->GetItemInfoFromItemProxy()->Desc)
				{
					std::string ItemNameString;
					if (Description->Title.TextData)
					{
						const int32_t Size = static_cast<int32_t>(wcslen(Description->Title.TextData->Text));
						ItemNameString.resize(Size);
						[[maybe_unused]] int32_t Result = WideCharToMultiByte(CP_UTF8, 0, Description->Title.TextData->Text, Size, ItemNameString.data(), Size, nullptr, nullptr) - 1;
					}
					else
					{
						ItemNameString = xorstr_("???");
					}
					
					RenderingUtility::RenderText(ItemNameString.c_str(), ScreenPosition, ImVec4(1.f, 1.f, 1.f, 1.f), false, true);
				}
			}

			if (Config.ESP.Misc.bBarrels && Actor->IsA<AStorageContainer>() && !Actor->IsA<ABuoyantStorageContainer>())
			{
				// if (ActorDistance > cfg.visuals.items.barreldrawdistance)
				// {
				// 	continue;
				// }
				
				RenderingUtility::RenderText(xorstr_("B"), ScreenPosition, ImVec4(1.f, 1.f, 1.f, 1.f), false, true);

				AStorageContainer* Barrel = Cast<AStorageContainer>(Actor);
				if (Config.ESP.Misc.BBarrelItems && Barrel && Barrel->StorageContainer && Barrel->StorageContainer->ContainerNodes.ContainerNodes.Data)
				{
					const TArray<FStorageContainerNode>& Nodes = Barrel->StorageContainer->ContainerNodes.ContainerNodes;

					for (uint32_t Index = 0; Index < Nodes.Count; Index++)
					{
						const FStorageContainerNode& Node = Nodes[Index];
						if (Node.ItemDesc)
						{
							if (UItemDesc* ItemDesc = Node.ItemDesc->CreateDefaultObject<UItemDesc>())
							{
								std::string ItemNameString;
								if (ItemDesc->Title.TextData)
								{
									const int32_t Size = static_cast<int32_t>(wcslen(ItemDesc->Title.TextData->Text));
									ItemNameString.resize(Size);
									[[maybe_unused]] int32_t Result = WideCharToMultiByte(CP_UTF8, 0, ItemDesc->Title.TextData->Text, Size, ItemNameString.data(), Size, nullptr, nullptr) - 1;
								}
								else
								{
									ItemNameString = xorstr_("???");
								}

								ItemNameString += std::vformat(xorstr_(" [{0}]"), std::make_format_args(Node.NumItems));

								FVector2D ItemScreenPosition = ScreenPosition;
								ItemScreenPosition.Y += 20;

								RenderingUtility::RenderText(ItemNameString.c_str(), ItemScreenPosition, ImVec4(1.f, 1.f, 1.f, 1.f), false, false);
							}
						}
					}
				}
			}

			if (Config.ESP.Misc.bShark && Actor->IsA<ASharkPawn>() && !Actor->IsA<ATinyShark>())
			{
				const float HeightAdjust = Height * 0.05f;
				FVector2D Position = { TopPos.X, TopPos.Y - HeightAdjust };

				const std::string SharkString = std::vformat(xorstr_("Shark ({0}m)"), std::make_format_args(ActorDistance));
				RenderingUtility::RenderText(SharkString.c_str(), Position, ImVec4(1.f, 1.f, 1.f, 1.f), false, true);
			}

			if (Config.ESP.Misc.bMegalodon && Actor->IsA<ATinyShark>())
			{
				const float HeightAdjust = Height * 0.05f;
				FVector2D Position = { TopPos.X, TopPos.Y - HeightAdjust };

				const std::string MegalodonString = std::vformat(xorstr_("Megalodon ({0}m)"), std::make_format_args(ActorDistance));
				RenderingUtility::RenderText(MegalodonString.c_str(), Position, ImVec4(1.f, 1.f, 1.f, 1.f), false, true);
			}

			if (Config.ESP.Misc.bNetProxy && Actor->IsA<ANetProxy>())
			{
				const float HeightAdjust = Height * 0.05f;
				FVector2D Position = { TopPos.X, TopPos.Y - HeightAdjust };

				const std::string NetProxyString = std::vformat(xorstr_("NetProxy ({0}m)"), std::make_format_args(ActorDistance));
				RenderingUtility::RenderText(NetProxyString.c_str(), Position, ImVec4(1.f, 1.f, 1.f, 1.f), false, true);
			}

			if (Config.ESP.Misc.bShipwrecks && (Actor->IsA<AShipwreck>() || Actor->NameContains(xorstr_("BP_Shipwreck_01_a_NetProxy_C"))))
			{
				const float HeightAdjust = Height * 0.05f;
				FVector2D Position = { TopPos.X, TopPos.Y - HeightAdjust };

				const std::string ShipwreckString = std::vformat(xorstr_("Shipwreck ({0}m)"), std::make_format_args(ActorDistance));
				RenderingUtility::RenderText(ShipwreckString.c_str(), Position, ImVec4(1.f, 1.f, 1.f, 1.f), false, true);
			}

			if (Config.ESP.Misc.bMapPins)
			{
				const AMapTable* MapTable = Cast<AMapTable>(Actor);
				if (MapTable && PirateSense::LocalPlayerActor->GetCurrentShip() == Actor->GetParentActor())
				{
					const TArray<FVector2D>& MapPins = MapTable->MapPins;
					for (uint32_t Index = 0; Index < MapPins.Count; Index++)
					{
						FVector2D CurrentMapPin = MapPins[Index];
						CurrentMapPin *= 100.f;
						
						FVector CurrentMapPinWorld;
						CurrentMapPinWorld.X = CurrentMapPin.X;
						CurrentMapPinWorld.Y = CurrentMapPin.Y;
						CurrentMapPinWorld.Z = 0.f;
						
						FVector2D PinScreenPosition;
						if (PirateSense::PlayerController->ProjectWorldLocationToScreen(CurrentMapPinWorld, PinScreenPosition))
						{
							const float DistanceToPin = LocalPlayerLocation.DistTo(CurrentMapPinWorld) / 100.f;
							const std::string PinString = std::vformat(xorstr_("Pin ({0}m)"), std::make_format_args(DistanceToPin));
							RenderingUtility::RenderText(PinString.c_str(), PinScreenPosition, ImVec4(1.f,1.f,1.f,1.f), false, true);
						}
					}
				}
			}

			if (Config.ESP.Misc.bTrackedShips)
			{
				const AMapTable* MapTable = Cast<AMapTable>(Actor);

				if (MapTable && ActorDistance <= 50.f && MapTable->TrackedShips.Data)
				{
					const TArray<FWorldMapShipLocation>& MapShips = MapTable->TrackedShips;

					for (uint32_t Index = 0; Index < MapShips.Count; Index++)
					{
						FVector2D CurrentTrackedShip = MapShips[Index].Location;
						char EmissaryLevel = MapShips[Index].EmissaryLevel;
						char ReapersMarkLevel = MapShips[Index].ReapersMarkLevel;
						const UClass* ShipSize = MapShips[Index].ShipSize;
						std::string ShipSizeName = ShipSize ? ShipSize->GetName() : xorstr_("");
						
						std::string ShipString;
						if (ShipSizeName.find(xorstr_("SmallShip")) != std::string::npos)
						{
							ShipString += xorstr_("Sloop");
						}
						else if (ShipSizeName.find(xorstr_("MediumShip")) != std::string::npos)
						{
							ShipString += xorstr_("Brig");
						}
						else if (ShipSizeName.find(xorstr_("LargeShip")) != std::string::npos)
						{
							ShipString += xorstr_("Galleon");
						}
						else
						{
							ShipString += xorstr_("Unknown");
						}

						std::string OwnerFactionString;
						if (MapShips[Index].OwnerIsInFaction)
						{
							OwnerFactionString = std::vformat(xorstr_(" {0}"), std::make_format_args(MapShips[Index].OwnerFactionName.GetName()));
						}

						// std::string OwnerMaxFactionString;
						// if (MapShips[Index].OwnerIsMaxFaction)
						// {
						// 	OwnerMaxFactionString = std::vformat(xorstr_(" ({0})"), std::make_format_args(MapShips[Index].OwnerIsMaxFaction));
						// }

						CurrentTrackedShip *= 10.f;
						
						FVector CurrentTrackedShipWorld;
						CurrentTrackedShipWorld.X = CurrentTrackedShip.X / 10.f;
						CurrentTrackedShipWorld.Y = CurrentTrackedShip.Y / 10.f;
						CurrentTrackedShipWorld.Z = LocalPlayerLocation.Z;
						
						FVector2D TrackedShipScreenPosition;
						const float DistanceFromTrackedShip = LocalPlayerLocation.DistTo(CurrentTrackedShipWorld) / 100.f;

						if (PirateSense::PlayerController->ProjectWorldLocationToScreen(CurrentTrackedShipWorld, TrackedShipScreenPosition) && DistanceFromTrackedShip > 1750.f)
						{
							const std::string PinString = std::vformat(xorstr_("Tracked ({0}){1}: ({2}m) ({3}e) ({4}lvl)"), std::make_format_args(ShipString, MapShips[Index].OwnerIsInFaction ? OwnerFactionString : xorstr_(""), DistanceFromTrackedShip, EmissaryLevel, ReapersMarkLevel));

							RenderingUtility::RenderText(PinString.c_str(), TrackedShipScreenPosition, ImVec4(1.f, 0.5f, 0.f, 0.8f), false, true);
						}
					}
				}
			}

			if (Config.ESP.Misc.bTrackedBooty)
			{
				const AMapTable* MapTable = Cast<AMapTable>(Actor);

				if (MapTable && ActorDistance <= 50.f && MapTable->TrackedBootyItemInfos.Data)
				{
					const TArray<FReapersMarkTrackedBootyItemInfo>& MapBootys = MapTable->TrackedBootyItemInfos;

					for (uint32_t Index = 0; Index < MapBootys.Count; Index++)
					{
						FVector CurrentTrackedBooty = MapBootys[Index].ItemLocation;

						const char* ItemTypes[] = { xorstr_("None"), xorstr_("Reapers Chest"), xorstr_("FOTD Key"), xorstr_("Bounty Chest"), xorstr_("Doubloon Reapers Chest"), xorstr_("FOF Key") };

						CurrentTrackedBooty *= 10.f;
						FVector CurrentTrackedBootyWorld;
						CurrentTrackedBootyWorld.X = CurrentTrackedBooty.X / 10.f;
						CurrentTrackedBootyWorld.Y = CurrentTrackedBooty.Y / 10.f;
						CurrentTrackedBootyWorld.Z = LocalPlayerLocation.Z + 20.f;
						
						FVector2D BootyScreenPosition;
						const float DistanceFromBooty = LocalPlayerLocation.DistTo(CurrentTrackedBootyWorld) / 100.f;
							
						if (PirateSense::PlayerController->ProjectWorldLocationToScreen(CurrentTrackedBootyWorld, BootyScreenPosition) && DistanceFromBooty > 150.f)
						{
							const std::string PinString = std::vformat(xorstr_("Tracked ({0}): ({1}m)"), std::make_format_args(ItemTypes[MapBootys[Index].ItemType], DistanceFromBooty));
							RenderingUtility::RenderText(PinString.c_str(), BootyScreenPosition, ImVec4(0.f, 0.5f, 1.f, 0.7f), false, true);
						}
					}
				}
			}

			if (Config.ESP.Events.bEnable && Actor->IsA<AGameplayEventSignal>())
			{
				if (ActorDistance > Config.ESP.Events.RenderDistance)
				{
					continue;
				}

				std::string EventNameString;
				if (Actor->NameContains(xorstr_("ShipCloud")))
				{
					EventNameString += xorstr_("Ship Cloud");
				}
				else if (Actor->NameContains(xorstr_("AshenLord")))
				{
					EventNameString += xorstr_("Ashen Lord");
				}
				// else if (Actor->NameContains(xorstr_("Flameheart")))
				// {
				// 	EventNameString += xorstr_("Flame Heart");
				// }
				else if (Actor->NameContains(xorstr_("LegendSkellyFort")))
				{
					EventNameString += xorstr_("Fort of Fortune");
				}
				else if (Actor->NameContains(xorstr_("RitualSkullCloud")))
				{
					EventNameString += xorstr_("Fort of the Damned");
				}
				else if (Actor->NameContains(xorstr_("BP_SkellyFort")))
				{
					EventNameString += xorstr_("Skelly Fort");
				}
				else if (Actor->NameContains(xorstr_("BP_GhostShip_TornadoCloud_C")))
				{
					EventNameString += xorstr_("Ghost Fleet");
				}
				// else if (Actor->NameContains(xorstr_("BP_PhantomTornado_C")))
				// {
				// 	EventNameString += xorstr_("Athena Veil");
				// }
				else
				{
					EventNameString += xorstr_("Event");
				}

				EventNameString += std::vformat(xorstr_(" ({0}m)"), std::make_format_args(ActorDistance));

				RenderingUtility::RenderText(EventNameString.c_str(), ScreenPosition, ImVec4(1.f, 0.2f, 0.f, 1.f), false, true);
			}
		}
	}
}
