#include "cheats.h"

bool gCheatsSandboxMode = false;
bool gCheatsDisableClearanceChecks = false;
bool gCheatsDisableSupportLimits = false;
bool gCheatsShowAllOperatingModes = false;

void cheats_reset()
{
	gCheatsSandboxMode = false;
	gCheatsDisableClearanceChecks = false;
	gCheatsDisableSupportLimits = false;
	gCheatsShowAllOperatingModes = false;
}
