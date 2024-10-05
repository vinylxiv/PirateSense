// PirateSense.

#include "DiscordPresence.h"

#include <time.h>
#include <chrono>

static int64_t EpochTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

void DiscordRPC::Initialize()
{
	DiscordEventHandlers Handle;
	memset(&Handle, 0, sizeof(Handle));

	Discord_Initialize("", &Handle, 1, nullptr);
}

void DiscordRPC::Update()
{
	DiscordRichPresence DiscordPresence;
	memset(&DiscordPresence, 0, sizeof(DiscordPresence));

	DiscordPresence.state = "Shrouded Ghost real?";
	DiscordPresence.details = "In Game";
	DiscordPresence.startTimestamp = EpochTime;
	DiscordPresence.largeImageKey = "seadogs";
	DiscordPresence.largeImageText = "gay";
	DiscordPresence.smallImageKey = "";

	Discord_UpdatePresence(&DiscordPresence);
}
