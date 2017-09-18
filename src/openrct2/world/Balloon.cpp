#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#include "../network/network.h"

#include "../audio/audio.h"
#include "../scenario/scenario.h"
#include "../util/util.h"
#include "sprite.h"

bool rct_sprite::IsBalloon()
{
    return this->balloon.sprite_identifier == SPRITE_IDENTIFIER_MISC &&
           this->balloon.misc_identifier == SPRITE_MISC_BALLOON;
}

rct_balloon * rct_sprite::AsBalloon()
{
    rct_balloon * result = nullptr;
    if (IsBalloon())
    {
        result = (rct_balloon *)this;
    }
    return result;
}

void rct_balloon::Update()
{
    invalidate_sprite_2((rct_sprite *)this);
    if (popped == 1)
    {
        frame += 256;
        if (frame >= 1280)
        {
            sprite_remove((rct_sprite *)this);
        }
    }
    else
    {
        sint32 original_var26a = var_26a;
        var_26a += 85;
        if (original_var26a >= 255 - 85)
        {
            var_26b++;
            sprite_move(x, y, z + 1, (rct_sprite*)this);

            sint32 maxZ = 1967 - ((x ^ y) & 31);
            if (z >= maxZ)
            {
                Pop();
            }
        }
    }
}

void rct_balloon::Press()
{
    if (popped != 1)
    {
        // There is a random chance that pressing the balloon will not pop it
        // and instead shift it slightly
        uint32 random = scenario_rand();
        if ((sprite_index & 7) || (random & 0xFFFF) < 0x2000)
        {
            Pop();
        }
        else
        {
            sint16 shift = ((random & 0x80000000) ? -6 : 6);
            sprite_move(x + shift, y, z, (rct_sprite *)this);
        }
    }
}

void rct_balloon::Pop()
{
    popped = 1;
    frame = 0;
    audio_play_sound_at_location(SOUND_BALLOON_POP, x, y, z);
}

static money32 game_command_balloon_press(uint16 spriteIndex, uint8 flags)
{
    rct_sprite * sprite = try_get_sprite(spriteIndex);
    if (sprite == nullptr || !sprite->IsBalloon())
    {
        log_error("Tried getting invalid sprite for balloon: %u", spriteIndex);
        return MONEY32_UNDEFINED;
    }
    else
    {
        if (flags & GAME_COMMAND_FLAG_APPLY)
        {
            sprite->AsBalloon()
                ->Press();
        }
        return 0;
    }
}

extern "C"
{
    void create_balloon(sint32 x, sint32 y, sint32 z, sint32 colour, bool isPopped)
    {
        rct_sprite* sprite = create_sprite(2);
        if (sprite != nullptr)
        {
            sprite->balloon.var_14 = 13;
            sprite->balloon.var_09 = 22;
            sprite->balloon.var_15 = 11;
            sprite->balloon.sprite_identifier = SPRITE_IDENTIFIER_MISC;
            sprite_move(x, y, z, sprite);
            sprite->balloon.misc_identifier = SPRITE_MISC_BALLOON;
            sprite->balloon.frame = 0;
            sprite->balloon.colour = colour;
            sprite->balloon.popped = (isPopped ? 1 : 0);
        }
    }

    void balloon_update(rct_balloon * balloon)
    {
        balloon->Update();
    }

    void game_command_balloon_press(sint32 * eax, sint32 * ebx, sint32 * ecx, sint32 * edx, sint32 * esi, sint32 * edi, sint32 * ebp)
    {
        *ebx = game_command_balloon_press(*eax & 0xFFFF, *ebx & 0xFF);
    }
}
