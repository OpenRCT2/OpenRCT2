/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Firework.h"

#include "../Context.h"
#include "../entity/EntityRegistry.h"
#include "../object/FireworkObject.h"
#include "../object/Object.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../paint/Paint.h"
#include "../profiling/Profiling.h"
#include "FireworksSequence.h"

#include <algorithm>

Firework* Firework::Create(
    const TileCoordsXY& tile, const int32_t height, const std::string& objectId, const colour_t color1, const colour_t color2,
    const colour_t color3)
{
    auto* firework = CreateEntity<Firework>();
    if (firework == nullptr)
        return nullptr;

    // set the position
    auto pos = CoordsXYZ{ tile.ToCoordsXY(), height };
    firework->x = pos.x;
    firework->y = pos.y;
    firework->z = pos.z;

    firework->_currentFrame = 0;
    firework->_color1 = color1;
    firework->_color2 = color2;
    firework->_color3 = color3;

    // get the base image
    auto context = OpenRCT2::GetContext();
    auto& objectManager = context->GetObjectManager();

    auto object = objectManager.LoadObject(objectId);

    if (object)
    {
        auto fireworkObject = reinterpret_cast<FireworkObject*>(object);
        if (fireworkObject)
        {
            firework->_numFrames = fireworkObject->GetNumImages();
            firework->_baseImage = fireworkObject->GetFirework().Image;
            firework->sprite_height_negative = 20;
            firework->sprite_height_positive = 20;
            firework->sprite_width = 20;
        }
        
    }
    return firework;
}

void Firework::Update()
{
    _currentFrame++;
    if (_currentFrame > _numFrames)
        EntityRemove(this);
}

void Firework::Paint(paint_session& session, int32_t imageDirection)
{
    PROFILED_FUNCTION();

    rct_drawpixelinfo& dpi = session.DPI;
    if (dpi.zoom_level > ZoomLevel{ 1 })
        return;

    auto imageId = ImageId(_baseImage + _currentFrame);
    PaintAddImageAsParent(session, imageId, { x, y, z }, { 1, 1, 0 });
}

template<> bool EntityBase::Is<Firework>() const
{
    return Type == EntityType::Firework;
}
