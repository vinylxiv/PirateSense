// PirateSense.

#include "AimbotModule.h"

#include <ThirdParty/XORSTR.h>

#include "Config.h"
#include "PirateSense.h"

AimbotModule::AimbotModule()
{
	ModuleName = xorstr_(L"Aimbot Module");
}

void AimbotModule::Update()
{
	if (!PirateSense::PlayerController || PirateSense::LocalPlayerActor || !PirateSense::Levels.Data)
	{
		return;
	}

	APlayerCameraManager* Camera = PirateSense::PlayerController->PlayerCameraManager;
	const FVector& CameraLocation = Camera->GetCameraLocation();
	const FRotator& CameraRotation = Camera->GetCameraRotation();

	const UWieldedItemComponent* WieldedItemComponent = PirateSense::LocalPlayerActor->WieldedItemComponent;
	AActor* Item = WieldedItemComponent ? WieldedItemComponent->CurrentlyWieldedItem : nullptr;
	const bool bHasProjectileWeapon = Item ? Item->IsA<AProjectileWeapon>() : false;
	const AProjectileWeapon* LocalProjectileWeapon = Cast<AProjectileWeapon>(Item);

	if (!bHasProjectileWeapon)
	{
		return;
	}

	Best = FLOAT_MAX;

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

			AAthenaCharacter* ActorAsCharacter = Cast<AAthenaCharacter>(Actor);
			const bool bIsActorPlayer = Actor->IsA<AAthenaPlayerCharacter>();
			const bool bIsActorSkeleton = Actor->IsA<AAICreatureCharacter>();

			if (((Config.Aimbot.Players.bEnable && bIsActorPlayer) || (Config.Aimbot.Skeletons.bEnable && bIsActorSkeleton)) && Actor != PirateSense::LocalPlayerActor && ActorAsCharacter && !ActorAsCharacter->IsDead())
			{
				FVector ActorLocation = Actor->K2_GetActorLocation();
				if (!ActorAsCharacter->IsInWater() && LocalProjectileWeapon->WeaponParameters.NumberOfProjectiles == 1)
				{
					ActorLocation.Z += bIsActorPlayer ? Config.Aimbot.Players.fAimHeight : Config.Aimbot.Skeletons.fAimHeight;
				}
				
				const float Distance = LocalPlayerLocation.DistTo(ActorLocation);
				if (Distance > LocalProjectileWeapon->WeaponParameters.ProjectileMaximumRange)
				{
					continue;
				}

				// if ((bIsActorPlayer ? Config.Aimbot.Players.bVisibleOnly : Config.Aimbot.Skeletons.bVisibleOnly) && !PirateSense::PlayerController->LineOfSightTo(Actor, LocalPlayerLocation, false))
				// {
				// 	continue;
				// }
				//
				// if ((bIsActorPlayer ? Config.Aimbot.Players.bTeam : false) && UCrewFunctions::AreCharactersInSameCrew(UObject::Cast<ACharacter>(Actor), PirateSense::LocalPlayerActor))
				// {
				// 	continue;
				// }
				
				const FRotator& RotationDelta = UKismetMathLibrary::NormalizedDeltaRotator(UKismetMathLibrary::FindLookAtRotation(CameraLocation, ActorLocation), CameraRotation);
				const float ABSYaw = abs(RotationDelta.Yaw);
				const float ABSPitch = abs(RotationDelta.Pitch);
				
				if (ABSYaw > (bIsActorPlayer ? Config.Aimbot.Players.fYaw : Config.Aimbot.Skeletons.fYaw) || ABSPitch > (bIsActorPlayer ? Config.Aimbot.Players.fPitch : Config.Aimbot.Skeletons.fPitch))
				{
					continue;
				}
				
				const float Sum = ABSYaw + ABSPitch;
				
				if (Sum < Best)
				{
					Target = Actor;
					Location = ActorLocation;
					DeltaRotation = RotationDelta;
					Best = Sum;
					Smoothness = (bIsActorPlayer ? Config.Aimbot.Players.fSmoothness : Config.Aimbot.Skeletons.fSmoothness);
				}
			}
		}
	}

	if (Target != nullptr)
	{
		if (ImGui::IsMouseDown(1))
		{
			FVector LocalVelocity = PirateSense::LocalPlayerActor->GetVelocity();
			if (AActor* LocalShip = PirateSense::LocalPlayerActor->GetCurrentShip())
			{
				LocalVelocity += LocalShip->GetVelocity();
			}
			
			FVector TargetVelocity = Target->GetVelocity();
			AAthenaCharacter* TargetAsCharacter = Cast<AAthenaCharacter>(Target);
			if (AActor* TargetShip = TargetAsCharacter ? TargetAsCharacter->GetCurrentShip() : nullptr)
			{
				TargetVelocity += TargetShip->GetVelocity();
			}
			
			const FVector TargetRelativeVelocity = TargetVelocity - LocalVelocity;
			const float BulletSpeed = LocalProjectileWeapon->WeaponParameters.AmmoParams.Velocity;
			const FVector RelativeLocation = PirateSense::LocalPlayerActor->K2_GetActorLocation() - Location;
			const float A = TargetRelativeVelocity.Size() - BulletSpeed * BulletSpeed;
			const float B = (RelativeLocation * TargetRelativeVelocity * 2.f).Sum();
			const float C = RelativeLocation.SizeSquared();
			const float D = B * B - 4 * A * C;
			
			if (D > 0)
			{
				const float DRoot = sqrtf(D);
				const float X1 = (-B + DRoot) / (2 * A);
				const float X2 = (-B - DRoot) / (2 * A);
				
				if (X1 >= 0 && X1 >= X2)
				{
					Location += TargetRelativeVelocity * X1;
				}
				else if (X2 >= 0)
				{
					Location += TargetRelativeVelocity * X2;
				}

				DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(UKismetMathLibrary::FindLookAtRotation(CameraLocation, Location), CameraRotation);
				
				const float ActualSmoothness = 1.f / Smoothness;
				
				PirateSense::PlayerController->AddYawInput(DeltaRotation.Yaw * ActualSmoothness);
				PirateSense::PlayerController->AddPitchInput(DeltaRotation.Pitch * -ActualSmoothness);
			}
		}
	}
}

void AimbotModule::RenderMenu()
{
	if (ImGui::BeginTabItem(xorstr_("Aimbot")))
	{
		ImGui::Text(xorstr_("Aimbot"));

		ImGui::Columns(2, xorstr_("AimbotStuff"), false);
		ImGui::Text(xorstr_("Players"));
		if (ImGui::BeginChild(xorstr_("Players"), ImVec2(0.f, 0.f), true, 0))
		{
			ImGui::Checkbox(xorstr_("Enable"), &Config.Aimbot.Players.bEnable);
			//ImGui::Checkbox(xorstr_("Visible Only"), &Config.Aimbot.Players.bVisibleOnly);
			//ImGui::Checkbox(xorstr_("Aim At Teammates"), &Config.Aimbot.Players.bTeam);
			ImGui::SliderFloat(xorstr_("Yaw"), &Config.Aimbot.Players.fYaw, 1.f, 200.f, xorstr_("%.0f"), ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat(xorstr_("Pitch"), &Config.Aimbot.Players.fPitch, 1.f, 200.f, xorstr_("%.0f"), ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat(xorstr_("Smoothness"), &Config.Aimbot.Players.fSmoothness, 1.f, 100.f, xorstr_("%.0f"), ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat(xorstr_("Aim Height"), &Config.Aimbot.Players.fAimHeight, 0.f, 100.f, xorstr_("%.0f"), ImGuiSliderFlags_AlwaysClamp);
		}
		ImGui::EndChild();

		ImGui::NextColumn();
		ImGui::Text(xorstr_("Skeletons"));
		if (ImGui::BeginChild(xorstr_("Skeletons"), ImVec2(0.f, 0.f), true, 0))
		{
			ImGui::Checkbox(xorstr_("Enable"), &Config.Aimbot.Skeletons.bEnable);
			//ImGui::Checkbox(xorstr_("Visible Only"), &Config.Aimbot.Players.bVisibleOnly);
			//ImGui::Checkbox(xorstr_("Aim At Teammates"), &Config.Aimbot.Players.bTeam);
			ImGui::SliderFloat(xorstr_("Yaw"), &Config.Aimbot.Skeletons.fYaw, 1.f, 200.f, xorstr_("%.0f"), ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat(xorstr_("Pitch"), &Config.Aimbot.Skeletons.fPitch, 1.f, 200.f, xorstr_("%.0f"), ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat(xorstr_("Smoothness"), &Config.Aimbot.Skeletons.fSmoothness, 1.f, 100.f, xorstr_("%.0f"), ImGuiSliderFlags_AlwaysClamp);
			ImGui::SliderFloat(xorstr_("Aim Height"), &Config.Aimbot.Skeletons.fAimHeight, 0.f, 100.f, xorstr_("%.0f"), ImGuiSliderFlags_AlwaysClamp);
		}
		ImGui::EndChild();

		ImGui::Columns();
		ImGui::EndTabItem();
	}
}

void AimbotModule::RenderOverlay(ImDrawList* DrawList)
{
	if (Target != nullptr)
	{
		FVector2D ScreenLocation;
		if (PirateSense::PlayerController->ProjectWorldLocationToScreen(Location, ScreenLocation))
		{
			const ImGuiIO& IO = ImGui::GetIO();
			const ImU32 Color = ImGui::GetColorU32(IM_COL32(0, 200, 0, 255));
			
			DrawList->AddLine({ IO.DisplaySize.x * 0.5f , IO.DisplaySize.y * 0.5f }, { ScreenLocation.X, ScreenLocation.Y }, Color);
			DrawList->AddCircle({ ScreenLocation.X, ScreenLocation.Y }, 3.f, Color);
		}
	}
}
