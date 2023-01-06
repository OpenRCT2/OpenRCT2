/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Balloon.h"

#include "../Game.h"
#include "../audio/audio.h"
#include "../core/DataSerialiser.h"
#include "../network/network.h"
#include "../paint/Paint.h"
#include "../profiling/Profiling.h"
#include "../scenario/Scenario.h"
#include "../util/Util.h"
#include "EntityRegistry.h"

template<> bool EntityBase::Is<Balloon>() const
{
    return Type == EntityType::Balloon;
}

void Balloon::Update()
{
    Invalidate();
    if (popped == 1)
    {
        frame++;
        if (frame >= 5)
        {
            EntityRemove(this);
        }
    }
    else
    {
        time_to_move++;
        if (time_to_move >= 3)
        {
            time_to_move = 0;
            frame++;
            // NOTE: To keep S6 Compatibility this field needs to roll over after 1 byte
            if (frame == 256)
            {
                frame = 0;
            }
            MoveTo({ x, y, z + 1 });

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
        if ((sprite_index.ToUnderlying() & 7) || (random & 0xFFFF) < 0x2000)
        {
            Pop();
        }
        else
        {
            int16_t shift = ((random & 0x80000000) ? -6 : 6);
            MoveTo({ x + shift, y, z });
        }
    }
}

void Balloon::Pop()
{
    popped = 1;
    frame = 0;
    OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::BalloonPop, { x, y, z });
}

void Balloon::Create(const CoordsXYZ& balloonPos, int32_t colour, bool isPopped)
{
    auto* balloon = CreateEntity<Balloon>();
    if (balloon == nullptr)
        return;

    balloon->sprite_width = 13;
    balloon->sprite_height_negative = 22;
    balloon->sprite_height_positive = 11;
    balloon->MoveTo(balloonPos);
    balloon->time_to_move = 0;
    balloon->frame = 0;
    balloon->colour = colour;
    balloon->popped = (isPopped ? 1 : 0);
}

void Balloon::Serialise(DataSerialiser& stream)
{
    EntityBase::Serialise(stream);
    stream << frame;
    stream << popped;
    stream << time_to_move;
    stream << colour;
}

void Balloon::Paint(PaintSession& session, int32_t imageDirection) const
{
    PROFILED_FUNCTION();

    uint32_t imageId = 22651 + (frame & 7);
    if (popped != 0)
    {
        imageId += 8;
    }

    auto image = ImageId(imageId, colour);
    PaintAddImageAsParent(session, image, { 0, 0, z }, { 1, 1, 0 });
}
