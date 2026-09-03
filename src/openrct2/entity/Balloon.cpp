/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Balloon.h"

#include "../GameState.h"
#include "../audio/Audio.h"
#include "../core/DataSerialiser.h"
#include "../ride/RideData.h"
#include "../scenario/Scenario.h"
#include "../world/TileElementsView.h"
#include "../world/tile_element/TrackElement.h"
#include "EntityRegistry.h"

namespace OpenRCT2
{
    template<>
    bool EntityBase::is<Balloon>() const
    {
        return type == EntityType::balloon;
    }

    void Balloon::update()
    {
        invalidate();
        if (popped == 1)
        {
            frame++;
            if (frame >= 5)
            {
                getGameState().entities.entityRemove(this);
            }
        }
        else
        {
            timeToMove++;
            if (timeToMove >= 3)
            {
                timeToMove = 0;
                frame++;
                // NOTE: To keep S6 Compatibility this field needs to roll over after 1 byte
                if (frame == 256)
                {
                    frame = 0;
                }

                if (collides())
                {
                    pop(false);
                    return;
                }

                moveTo({ x, y, z + 1 });

                int32_t maxZ = 1967 - ((x ^ y) & 31);
                if (z >= maxZ)
                {
                    pop(true);
                }
            }
        }
    }

    void Balloon::press()
    {
        if (popped != 1)
        {
            // There is a random chance that pressing the balloon will not pop it
            // and instead shift it slightly
            uint32_t random = ScenarioRand();
            if ((id.ToUnderlying() & 7) || (random & 0xFFFF) < 0x2000)
            {
                pop(true);
            }
            else
            {
                int16_t shift = ((random & 0x80000000) ? -6 : 6);
                moveTo({ x + shift, y, z });
            }
        }
    }

    void Balloon::pop(bool playSound)
    {
        popped = 1;
        frame = 0;
        if (playSound)
        {
            Audio::Play3D(Audio::SoundId::balloonPop, { x, y, z });
        }
    }

    void Balloon::create(const CoordsXYZ& balloonPos, Drawing::Colour colour, bool isPopped)
    {
        auto* balloon = getGameState().entities.createEntity<Balloon>();
        if (balloon == nullptr)
            return;

        balloon->spriteData.width = 13;
        balloon->spriteData.heightMin = 22;
        balloon->spriteData.heightMax = 11;
        balloon->moveTo(balloonPos);
        balloon->timeToMove = 0;
        balloon->frame = 0;
        balloon->colour = colour;
        balloon->popped = (isPopped ? 1 : 0);
    }

    void Balloon::serialise(DataSerialiser& stream)
    {
        EntityBase::serialise(stream);
        stream << frame;
        stream << popped;
        stream << timeToMove;
        stream << colour;
    }

    bool Balloon::collides() const
    {
        for (auto* tileElement : TileElementsView(CoordsXY(x, y)))
        {
            // the balloon has height so we add some padding to prevent it clipping through things.
            int32_t balloon_top = z + kCoordsZStep * 2;
            if (balloon_top == tileElement->getBaseZ())
            {
                return true;
            }

            // check for situations where guests can drop a balloon inside a covered building
            bool check_ceiling = tileElement->getType() == TileElementType::entrance;
            if (tileElement->getType() == TileElementType::track)
            {
                const TrackElement* trackElement = tileElement->asTrack();
                const auto* ride = GetRide(trackElement->getRideIndex());
                if (ride != nullptr && ride->getRideTypeDescriptor().flags.has(RtdFlag::hasRoofOverWholeRide))
                {
                    check_ceiling = true;
                }
                else
                {
                    check_ceiling = (ride != nullptr) ? RideHasStationShelter(*ride) : false;
                }
            }

            if (check_ceiling)
            {
                if (balloon_top > tileElement->getBaseZ() && z < tileElement->getClearanceZ())
                {
                    return true;
                }
            }
        }
        return false;
    }
} // namespace OpenRCT2
