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

#include <algorithm>

using namespace OpenRCT2::Fireworks;

OpenRCT2::Fireworks::FireworksManager::FireworksManager()
{
}

OpenRCT2::Fireworks::FireworksManager::~FireworksManager()
{
}

void OpenRCT2::Fireworks::FireworksManager::OnEvent(const Event& e)
{
}

void OpenRCT2::Fireworks::FireworksManager::OnUpdate()
{
}

void OpenRCT2::Fireworks::FireworksManager::OnMusicLaunch(const std::string& musicId)
{
}

void OpenRCT2::Fireworks::FireworksManager::OnReset()
{
}

void OpenRCT2::Fireworks::FireworksManager::Update()
{
}

FireworksSequence& OpenRCT2::Fireworks::FireworksManager::GetSequence()
{
    // // O: insert return statement here
    return _sequence;
}
