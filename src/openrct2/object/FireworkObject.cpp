/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#include "FireworkObject.h"

#include "../Context.h"
#include "../PlatformEnvironment.h"
#include "../core/Json.hpp"
#include "../core/Path.hpp"
#include "../drawing/Image.h"

using namespace OpenRCT2;

void FireworkObject::ReadJson(IReadObjectContext* context, json_t& root)
{
    Guard::Assert(root.is_object(), "SmallSceneryObject::ReadJson expects parameter root to be object");

    auto properties = root["properties"];
    if (properties.is_object())
    {
        _firework.Width = properties["Width"];
        _firework.Height = properties["Height"];
        _firework.SoundId = properties["SoundId"];
        _firework.UseRemap1 = properties["UseRemap1"];
        _firework.UseRemap2 = properties["UseRemap2"];
        _firework.UseRemap3 = properties["UseRemap3"];
    }

    PopulateTablesFromJson(context, root);
}

void FireworkObject::Load()
{
    GetStringTable().Sort();
    _firework.Name = language_allocate_object_string(GetName());
    _firework.Image = gfx_object_allocate_images(GetImageTable().GetImages(), GetImageTable().GetCount());
}

void FireworkObject::Unload()
{
    language_free_object_string(_firework.Name);
    gfx_object_free_images(_firework.Image, GetImageTable().GetCount());

    _firework.Name = 0;
    _firework.Image = 0;
}

const Firework& FireworkObject::GetFirework() const
{
    return _firework;
}
