/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "EntityBase.h"

class DataSerialiser;
struct CoordsXYZ;
struct CoordsXYZD;
struct paint_session;

struct Litter : EntityBase
{
    enum class Type : uint8_t
    {
        Vomit,
        VomitAlt,
        EmptyCan,
        Rubbish,
        BurgerBox,
        EmptyCup,
        EmptyBox,
        EmptyBottle,
        EmptyBowlRed,
        EmptyDrinkCarton,
        EmptyJuiceCup,
        EmptyBowlBlue,
    };

    static constexpr auto cEntityType = EntityType::Litter;
    Type SubType;
    uint32_t creationTick;
    static void Create(const CoordsXYZD& litterPos, Type type);
    static void RemoveAt(const CoordsXYZ& litterPos);
    void Serialise(DataSerialiser& stream);
    StringId GetName() const;
    uint32_t GetAge() const;
    void Paint(paint_session& session, int32_t imageDirection) const;
};
