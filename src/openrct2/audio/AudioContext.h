#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* OpenRCT2 is the work of many authors, a full list can be found in contributors.md
* For more information, visit https://github.com/OpenRCT2/OpenRCT2
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* A full copy of the GNU General Public License can be found in licence.txt
*****************************************************************************/
#pragma endregion

#pragma once

#include "../common.h"

namespace OpenRCT2
{
    namespace Audio
    {
        interface IAudioChannel;

        /**
         * Audio services for playing music and sound effects.
         */
        interface IAudioContext
        {
            virtual ~IAudioContext() = default;

            virtual void StartTitleMusic() abstract;

            virtual IAudioChannel * PlaySound(sint32 soundId, sint32 volume, sint32 pan) abstract;
            virtual IAudioChannel * PlaySoundAtLocation(sint32 soundId, sint16 x, sint16 y, sint16 z) abstract;
            virtual IAudioChannel * PlaySoundPanned(sint32 soundId, sint32 pan, sint16 x, sint16 y, sint16 z) abstract;

            virtual void ToggleAllSounds() abstract;
            virtual void PauseSounds() abstract;
            virtual void UnpauseSounds() abstract;

            virtual void StopAll() abstract;
            virtual void StopCrowdSound() abstract;
            virtual void StopRainSound() abstract;
            virtual void StopRideMusic() abstract;
            virtual void StopTitleMusic() abstract;
            virtual void StopVehicleSounds() abstract;
        };
    }
}
