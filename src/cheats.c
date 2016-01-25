#include "cheats.h"

bool gCheatsSandboxMode = false;
bool gCheatsDisableClearanceChecks = false;
bool gCheatsDisableSupportLimits = false;
bool gCheatsShowAllOperatingModes = false;
bool gCheatsShowVehiclesFromOtherTrackTypes = false;
bool gCheatsFastLiftHill = false;
bool gCheatsDisableBrakesFailure = false;
bool gCheatsDisableAllBreakdowns = false;
bool gCheatsUnlockAllPrices = false;
bool gCheatsBuildInPauseMode = false;
bool gCheatsIgnoreRideIntensity = false;
bool gCheatsDisableVandalism = false;

void cheats_reset()
{
	gCheatsSandboxMode = false;
	gCheatsDisableClearanceChecks = false;
	gCheatsDisableSupportLimits = false;
	gCheatsShowAllOperatingModes = false;
	gCheatsShowVehiclesFromOtherTrackTypes = false;
	gCheatsFastLiftHill = false;
	gCheatsDisableBrakesFailure = false;
	gCheatsDisableAllBreakdowns = false;
	gCheatsUnlockAllPrices = false;
	gCheatsBuildInPauseMode = false;
	gCheatsIgnoreRideIntensity = false;
	gCheatsDisableVandalism = false;
}
