/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../network/network.h"

#include "../audio/audio.h"
#include "../scenario/Scenario.h"
#include "../util/Util.h"
#include "Sprite.h"

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
        frame++;
        if (frame >= 5)
        {
            sprite_remove((rct_sprite *)this);
        }
    }
    else
    {
        time_to_move++;
        if (time_to_move >= 3)
        {
            time_to_move = 0;
            frame++;
            sprite_move(x, y, z + 1, (rct_sprite*)this);

            int32_t maxZ = 1967 - ((x ^ y) & 31);
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
        uint32_t random = scenario_rand();
        if ((sprite_index & 7) || (random & 0xFFFF) < 0x2000)
        {
            Pop();
        }
        else
        {
            int16_t shift = ((random & 0x80000000) ? -6 : 6);
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

static money32 game_command_balloon_press(uint16_t spriteIndex, uint8_t flags)
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

void create_balloon(int32_t x, int32_t y, int32_t z, int32_t colour, bool isPopped)
{
    rct_sprite* sprite = create_sprite(2);
    if (sprite != nullptr)
    {
        sprite->balloon.sprite_width = 13;
        sprite->balloon.sprite_height_negative = 22;
        sprite->balloon.sprite_height_positive = 11;
        sprite->balloon.sprite_identifier = SPRITE_IDENTIFIER_MISC;
        sprite_move(x, y, z, sprite);
        sprite->balloon.misc_identifier = SPRITE_MISC_BALLOON;
        sprite->balloon.time_to_move = 0;
        sprite->balloon.frame = 0;
        sprite->balloon.colour = colour;
        sprite->balloon.popped = (isPopped ? 1 : 0);
    }
}

void balloon_update(rct_balloon * balloon)
{
    balloon->Update();
}

void game_command_balloon_press(
    int32_t *                  eax,
    int32_t *                  ebx,
    [[maybe_unused]] int32_t * ecx,
    [[maybe_unused]] int32_t * edx,
    [[maybe_unused]] int32_t * esi,
    [[maybe_unused]] int32_t * edi,
    [[maybe_unused]] int32_t * ebp)
{
    *ebx = game_command_balloon_press(*eax & 0xFFFF, *ebx & 0xFF);
}
