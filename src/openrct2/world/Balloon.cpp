/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Game.h"
#include "../audio/audio.h"
#include "../network/network.h"
#include "../scenario/Scenario.h"
#include "../util/Util.h"
#include "Sprite.h"

bool rct_sprite::IsBalloon()
{
    return this->balloon.sprite_identifier == SPRITE_IDENTIFIER_MISC && this->balloon.type == SPRITE_MISC_BALLOON;
}

Balloon* rct_sprite::AsBalloon()
{
    Balloon* result = nullptr;
    if (IsBalloon())
    {
        result = (Balloon*)this;
    }
    return result;
}

void Balloon::Update()
{
    invalidate_sprite_2(this);
    if (popped == 1)
    {
        frame++;
        if (frame >= 5)
        {
            sprite_remove(this);
        }
    }
    else
    {
        time_to_move++;
        if (time_to_move >= 3)
        {
            time_to_move = 0;
            frame++;
            sprite_move(x, y, z + 1, this);

            int32_t maxZ = 1967 - ((x ^ y) & 31);
            if (z >= maxZ)
            {
                Pop();
            }
        }
    }
}

void Balloon::Press()
{
    if (popped != 1)
    {
        // There is a random chance that pressing the balloon will not pop it
        // and instead shift it slightly
        uint32_t random = scenario_rand();
        if ((sprite_index & 7) || (random & 0xFFFF) < Probability::_12_5_Percent)
        {
            Pop();
        }
        else
        {
            int16_t shift = ((random & 0x80000000) ? -6 : 6);
            sprite_move(x + shift, y, z, this);
        }
    }
}

void Balloon::Pop()
{
    popped = 1;
    frame = 0;
    audio_play_sound_at_location(SoundId::BalloonPop, { x, y, z });
}

void create_balloon(int32_t x, int32_t y, int32_t z, int32_t colour, bool isPopped)
{
    rct_sprite* sprite = create_sprite(SPRITE_IDENTIFIER_MISC);
    if (sprite != nullptr)
    {
        sprite->balloon.sprite_width = 13;
        sprite->balloon.sprite_height_negative = 22;
        sprite->balloon.sprite_height_positive = 11;
        sprite->balloon.sprite_identifier = SPRITE_IDENTIFIER_MISC;
        sprite_move(x, y, z, &sprite->balloon);
        sprite->balloon.type = SPRITE_MISC_BALLOON;
        sprite->balloon.time_to_move = 0;
        sprite->balloon.frame = 0;
        sprite->balloon.colour = colour;
        sprite->balloon.popped = (isPopped ? 1 : 0);
    }
}

void balloon_update(Balloon* balloon)
{
    balloon->Update();
}
