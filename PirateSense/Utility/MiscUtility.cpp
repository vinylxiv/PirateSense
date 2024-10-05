// PirateSense.

#include "MiscUtility.h"

#include <Windows.h>

uintptr_t MiscUtility::MillisecondsNow()
{
    static LARGE_INTEGER s_frequency;
    static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
    if (s_use_qpc)
    {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return (1000LL * now.QuadPart) / s_frequency.QuadPart;
    }
	
    return GetTickCount64();
}

std::string MiscUtility::GetShortName(const std::string& Name)
{
	// Cannonball Stuff
	if (Name.find("cannon_ball") != std::string::npos)
	{
		return "Cannon Ball";
	}
	if (Name.find("cannonball_chain_shot") != std::string::npos)
	{
		return "Chainshot";
	}
	if (Name.find("cannonball_Grenade") != std::string::npos)
	{
		return "BlunderBomb";
	}
	if (Name.find("cannonball_cur_fire") != std::string::npos)
	{
		return "Fire Ball";
	}

	// Cursed Cannonballs

	//Purple
	if (Name.find("cannonball_cur_ballast") != std::string::npos)
	{
		return "Ballastball";
	}
	if (Name.find("cannonball_cur_passive") != std::string::npos)
	{
		return "Peaceball";
	}
	if (Name.find("cannonball_cur_anchor") != std::string::npos)
	{
		return "Anchorball";
	}
	if (Name.find("cannonball_cur_barrel") != std::string::npos)
	{
		return "Barrelball";
	}
	//Green
	if (Name.find("cannonball_cur_grog") != std::string::npos)
	{
		return "Grogball";
	}
	if (Name.find("cannonball_cur_rigging") != std::string::npos)
	{
		return "Riggingball";
	}
	if (Name.find("cannonball_cur_limp") != std::string::npos)
	{
		return "Limpball";
	}
	if (Name.find("cannonball_cur_boogie") != std::string::npos)
	{
		return "Jigball";
	}
	if (Name.find("cannonball_cur_snooze") != std::string::npos)
	{
		return "Wearyball";
	}



	// Wood
	if (Name.find("repair_wood") != std::string::npos)
	{
		return "Wood";
	}

	// Food Stuff
	if (Name.find("PomegranateFresh") != std::string::npos)
	{
		return "Pomegrant";
	}
	if (Name.find("CoconutFresh") != std::string::npos)
	{
		return "Coconut";
	}
	if (Name.find("BananaFresh") != std::string::npos)
	{
		return "Banana";
	}
	if (Name.find("PineappleFresh") != std::string::npos)
	{
		return "Pineapple";
	}
	if (Name.find("MangoFresh") != std::string::npos)
	{
		return "Mango";
	}

	// Meat Stuff
	if (Name.find("SnakeMeat") != std::string::npos)
	{
		return "Snake Meat";
	}

	if (Name.find("PorkMeat") != std::string::npos)
	{
		return "Pork Meat";
	}

	if (Name.find("SharkMeat") != std::string::npos)
	{
		return "Shark Meat";
	}

	if (Name.find("MegMeat") != std::string::npos)
	{
		return "Meg Meat";
	}

	if (Name.find("ChickenMeat") != std::string::npos)
	{
		return "Chicken Meat";
	}

	if (Name.find("KrakenMeat") != std::string::npos)
	{
		return "Kraken Meat";
	}

	// Fish Stuff
	// Splashtails
	if (Name.find("SplashTail") != std::string::npos)
	{
		return "SplashTail Fish";
	}

	// Battlegill
	if (Name.find("Battlegill") != std::string::npos)
	{
		return "Battlegill Fish";
	}


	// Pondies
	if (Name.find("Pondie") != std::string::npos)
	{
		return "Pondie Fish";
	}


	// Wrecker
	if (Name.find("Wrecker") != std::string::npos)
	{
		return "Wrecker Fish";
	}


	// Stormfish
	if (Name.find("StormFish") != std::string::npos)
	{
		return "Storm Fish";
	}


	// WildSplash
	if (Name.find("WildSplash") != std::string::npos)
	{
		return "WildSplash Fish";
	}


	// Devilfish
	if (Name.find("DevilFish") != std::string::npos)
	{
		return "Lava Devil Fish";
	}

	// Worm Stuff
	if (Name.find("GrubsFresh") != std::string::npos)
	{
		return "Grubs";
	}
	if (Name.find("LeechesFresh") != std::string::npos)
	{
		return "Leeches";
	}
	if (Name.find("EarthwormsFresh") != std::string::npos)
	{
		return "Earthworms";
	}

	// Firework Stuff
	if (Name.find("fireworks_flare") != std::string::npos)
	{
		return "Flare";
	}
	if (Name.find("fireworks_rocket") != std::string::npos)
	{
		return "Fireworks S";
	}
	if (Name.find("fireworks_cake") != std::string::npos)
	{
		return "Fireworks M";
	}
	if (Name.find("fireworks_living") != std::string::npos)
	{
		return "Fireworks L";
	}

	if (Name.find("MapInABarrel") != std::string::npos)
	{
		return "Scroll";
	}

	return Name;
}
