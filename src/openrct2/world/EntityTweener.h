/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SpriteBase.h"
#include <vector>

class EntityTweener
{
    std::vector<SpriteBase*> Entities;
    std::vector<CoordsXYZ> PrePos;
    std::vector<CoordsXYZ> PostPos;

private:
    void PopulateEntities();

public:
    static EntityTweener& Get();

    void PreTick();
    void PostTick();
    void RemoveEntity(SpriteBase* entity);
    void Tween(float alpha);
    void Restore();
    void Reset();
};
