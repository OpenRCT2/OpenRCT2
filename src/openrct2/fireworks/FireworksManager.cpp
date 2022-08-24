/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "FireworksManager.h"

#include "../entity/EntityRegistry.h"
#include "Firework.h"

#include <algorithm>

using namespace OpenRCT2::Fireworks;

OpenRCT2::Fireworks::FireworksManager::FireworksManager()
{
    _sequence.SetHandler(*this);
}

OpenRCT2::Fireworks::FireworksManager::~FireworksManager()
{
}

void OpenRCT2::Fireworks::FireworksManager::OnEvent(const Event& e)
{
    // create an entity of type firework and keep a pointer in our class so we can delete it any time we want
    auto spawner = _sequence.GetSpawner(e.SpawnerId);
    if (spawner != nullptr)
    {
        auto firework = Firework::Create(spawner->Location, e.Height, e.ObjectId, e.Palette1, e.Palette2, e.Palette3);
        _fireworks.push_back(firework);
    }
}

void OpenRCT2::Fireworks::FireworksManager::OnUpdate()
{
    //for (auto firework : _fireworks)
        //firework->Update();
}

void OpenRCT2::Fireworks::FireworksManager::OnMusicLaunch(const std::string& musicId)
{
}

void OpenRCT2::Fireworks::FireworksManager::OnReset()
{
    for (auto firework:_fireworks)
    {
        EntityRemove(firework);
    }
}

void OpenRCT2::Fireworks::FireworksManager::Update()
{
    _sequence.Update(1);
}

FireworksSequence& OpenRCT2::Fireworks::FireworksManager::GetSequence()
{
    // // O: insert return statement here
    return _sequence;
}
