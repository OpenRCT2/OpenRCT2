/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../drawing/Drawing.h"
#include "../entity/EntityBase.h"
#include "../interface/Colour.h"
#include "../world/Location.hpp"

struct paint_session;

struct Firework : EntityBase
{
private:
public:
    static constexpr auto cEntityType = EntityType::Firework;
    static Firework* Create(
        const TileCoordsXY& tile, const int32_t height, const std::string& objectId, const colour_t color1,
        const colour_t color2, const colour_t color3);
    void Update();
    void Paint(paint_session& session, int32_t imageDirection);

private:
    uint32_t _currentFrame;

    const rct_g1_element* _imageTable;
    uint32_t _numFrames;

    colour_t _color1;
    colour_t _color2;
    colour_t _color3;

    bool _useRemap1;
    bool _useRemap2;
    bool _useRemap3;

    uint32_t _frameWidth;
    uint32_t _frameHeight;
};
