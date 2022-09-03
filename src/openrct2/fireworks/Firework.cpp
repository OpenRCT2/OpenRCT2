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
#include "../Game.h"
#include "../drawing/Drawing.h"
#include "FireworksSequence.h"

#include <algorithm>

Firework* Firework::Create(
    const TileCoordsXY& tile, const int32_t height, const std::string& objectId, const colour_t color1, const colour_t color2,
    const colour_t color3)
{
    auto* firework = CreateEntity<Firework>();
    if (firework == nullptr)
        return nullptr;

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
            firework->sprite_height_negative = 128;
            firework->sprite_height_positive = 128;
            firework->sprite_width = 96;
        }
        
    }

    // set the position
    auto pos = CoordsXYZ{ tile.ToCoordsXY(), height * LAND_HEIGHT_STEP };
    firework->MoveTo(pos);
    return firework;
}

void Firework::Update()
{
    Invalidate();

    auto targetFrame = _currentFrame + 1;
    if (targetFrame >= _numFrames)
    {
        //EntityRemove(this);
        //targetFrame = _currentFrame;
        targetFrame = 0;
    }
    _currentFrame = targetFrame;
}

void Firework::Paint(paint_session& session, int32_t imageDirection)
{
    PROFILED_FUNCTION();

    //rct_drawpixelinfo& dpi = session.DPI;
    /*if (dpi.zoom_level > ZoomLevel{ 1 })
        return;*/

    session.InteractionType = ViewportInteractionItem::None;
    auto imageId = ImageId(_baseImage + _currentFrame * 4 + imageDirection);

    //clipping hack
    if (imageDirection == 0 || imageDirection == 16)
        PaintAddImageAsParent(session, imageId, { 0, 0, z }, { 255, 255, 64 });
    else if (imageDirection == 24)
        PaintAddImageAsParent(session, imageId, { -16, 0, z }, { 255, 255, 64 });
    else
        PaintAddImageAsParent(session, imageId, { 0, -16, z }, { 255, 255, 64 });
}

template<> bool EntityBase::Is<Firework>() const
{
    return Type == EntityType::Firework;
}
