/*****************************************************************************
 * Copyright (c) 2014 Ted John
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include "addresses.h"
#include "audio/audio.h"
#include "audio/mixer.h"
#include "cmdline.h"
#include "config.h"
#include "editor.h"
#include "localisation/localisation.h"
#include "openrct2.h"
#include "platform/osinterface.h"

int gOpenRCT2StartupAction = STARTUP_ACTION_TITLE;
char gOpenRCT2StartupActionPath[512] = { 0 };

/**
 * Launches the game, after command line arguments have been parsed and processed.
 */
void openrct2_launch()
{
	get_system_info();
	audio_init();
	audio_get_devices();
	get_dsound_devices();
	config_init();
	language_open(gGeneral_config.language);
	rct2_init();
	Mixer_Init(NULL);

	switch (gOpenRCT2StartupAction) {
	case STARTUP_ACTION_INTRO:
		RCT2_GLOBAL(RCT2_ADDRESS_RUN_INTRO_TICK_PART, uint8) = 8;
		break;
	case STARTUP_ACTION_TITLE:
		RCT2_GLOBAL(RCT2_ADDRESS_RUN_INTRO_TICK_PART, uint8) = 0;
		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_TITLE_DEMO;
		break;
	case STARTUP_ACTION_OPEN:
		assert(gOpenRCT2StartupActionPath != NULL);
		rct2_open_file(gOpenRCT2StartupActionPath);

		RCT2_GLOBAL(RCT2_ADDRESS_RUN_INTRO_TICK_PART, uint8) = 0;
		RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) = SCREEN_FLAGS_PLAYING;
		break;
	case STARTUP_ACTION_EDIT:
		if (strlen(gOpenRCT2StartupActionPath) == 0)
			editor_load();
		else
			editor_load_landscape(gOpenRCT2StartupActionPath);
		break;
	}

	rct2_loop();
	osinterface_free();

	// HACK Some threads are still running which causes the game to not terminate. Investigation required!
	exit(gExitCode);
}