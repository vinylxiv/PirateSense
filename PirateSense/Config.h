// PirateSense.

#pragma once

#include <imgui.h>

static inline struct Configuration
{
	struct
	{
		struct
		{
			bool bEnable = false;

			float RenderDistance = 100.f;
			ImVec4 ColorVisible = { 1.f, 1.f, 1.f, 1.f };
			ImVec4 ColorInvisible = { 1.f, 1.f, 1.f, 1.f };
			bool bTeam = false;
			bool bTracers = false;
			float TracersThickness = 1.f;

			bool bItems = false;
			bool bHealth = false;
		}Players;

		struct
		{
			bool bEnable = false;
			bool bItems = false;
			bool bNames = false;
			bool bHealth = false;
		}Skeletons;

		struct
		{
			bool bEnable = false;
			bool bItems = false;
		}OceanCrawler;

		struct
		{
			bool bEnable = false;

			bool bSkeletons = false;
			bool bGhosts = false;

			float RenderDistance = 15000.f;
		}Ships;

		struct
		{
			bool bEnable = false;

			float RenderDistance = 15000.f;
		}Events;

		struct
		{
			bool bEnable = false;

			struct
			{
				bool bRowboats = false;
				bool bRowboatItems = false;
				bool bRowboatHealth = false;
				bool bRowboatLivery = false;
			}Rowboats;
			
			bool bMermaids = false;
			bool bShipwrecks = false;

			bool bShark = false;
			bool bMapPins = false;
			bool bMegalodon = false;
			bool bItems = false;
			bool bBarrels = false;
			bool BBarrelItems = false;

			bool bTrackedShips = false;
			bool bTrackedBooty = false;

			bool bXMarks = false;
			bool bStorm = false;
			bool bFogBank = false;
			bool bAmmo = false;
			bool bNPC = false;
			bool bSeagulls = false;
			bool bMermaidStatues = false;

			bool bEmissaryTables = false;

			bool bNetProxy = false;
		}Misc;
	}ESP;

	struct
	{
		struct
		{
			bool bEnable = false;
			bool bVisibleOnly = false;
			bool bTeam = false;
			float fYaw = 1.f;
			float fPitch = 1.f;
			float fSmoothness = 1.f;
			float fAimHeight = 1.f;
		}Players;

		struct
		{
			bool bEnable = false;
			bool bVisibleOnly = false;
			float fYaw = 1.f;
			float fPitch = 1.f;
			float fSmoothness = 1.f;
			float fAimHeight = 1.f;
		}Skeletons;

		struct
		{
			bool bEnable = false;
		}Harpoon;
	}Aimbot;

	struct
	{
		bool bCompass = false;

		bool bAntiAFKKick = false;

		bool bOxygen = false;

		bool bTimeOfDay = false;

		float TimeOfDay = 0.f;

		bool bNoFog = false;

		bool bCrosshair = false;
		float CrosshairSize = 1.f;
		float CrosshairThickness = 1.f;
		ImVec4 CrosshairColor = { 1.f, 1.f, 1.f, 1.f };

		struct
		{
			bool bEnable = false;

			bool bFastSword = false;
			bool bNoClamp = false;
		}Sword;

		struct
		{
			bool bEnable = false;

			bool bFastSwitch = false;

		}Weapon;

		struct
		{
			bool bEnable = false;

			bool bPlayerlist = false;
			bool bShipInfo = false;
		}Ship;
	}Misc;

	struct
	{
		struct
		{
			bool bEnable = false;
		}InstantLadder;

		struct
		{
			bool bEnable = false;
		}UnderwaterWalk;
	}Exploits;

	struct
	{
		struct
		{
			bool bFovEnable = false;
			float fov = 90.f;
			bool bSpyRClickMode = true;
			float sniperFovMul = 1.0;
			float spyglassFovMul = 1.0;
		}FOV;
	}View;

	struct
	{
		bool bDebugNames = false;
		float DebugRenderDistance = 2000.f;
		int DebugNamesTextSize = 20;
		char DebugNamesFilter[0x64] = { 0 };
	}Dev;
}Config;