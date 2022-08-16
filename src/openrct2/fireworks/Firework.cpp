/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "FireworksSequence.h"

#include <algorithm>
#include "Firework.h"

using namespace OpenRCT2::Fireworks;

OpenRCT2::Fireworks::Firework::Firework()
{
}

OpenRCT2::Fireworks::Firework::~Firework()
{
}

void OpenRCT2::Fireworks::Firework::Create(
    const TileCoordsXY& tile, const uint32_t height, const std::string& objectId, const colour_t color1, const colour_t color2,
    const colour_t color3)
{
}

void OpenRCT2::Fireworks::Firework::Update()
{
}

void OpenRCT2::Fireworks::Firework::Paint(paint_session& session, int32_t imageDirection)
{
}

template<> bool EntityBase::Is<Firework>() const
{
    return Type == EntityType::Firework;
}
