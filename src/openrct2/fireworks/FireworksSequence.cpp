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

using namespace OpenRCT2::Fireworks;

FireworksSequence::FireworksSequence()
    : _currentTime(0)
    , _currentEvent(0)
    , _playing(false)
    , _handler(nullptr)
{
}

FireworksSequence::~FireworksSequence()
{
}

void FireworksSequence::Start()
{
    if (_handler == nullptr)
        return;

    _currentTime = 0;
    _currentEvent = 0;
    _playing = true;

    // reset the handlers
    _handler->OnReset();
    _handler->OnMusicLaunch(BackgroundMusic);
}

void FireworksSequence::Update(const uint32_t updateSteps)
{
    if (!_playing)
        return;

    // check if there is a handler
    if (_handler == nullptr)
        return;

    // check if there is any event
    if (_events.empty())
        return;

    // check if there is any spawner
    if (_spawners.empty())
        return;

    _currentTime += updateSteps;

    // check if we need to launch events
    while (_currentEvent < _events.size())
    {
        const auto& currentEvent = _events[_currentEvent];
        if (currentEvent.Time <= _currentTime)
        {
            // call the onEvent function of the handler
            _handler->OnEvent(currentEvent);
            _currentEvent++;
        }
        else
            break;
    }

    // call the update functions of the handlers
    uint32_t steps = 0;
    while (steps < updateSteps)
    {
        _handler->OnUpdate();
        steps++;
    }
}

void OpenRCT2::Fireworks::FireworksSequence::Stop()
{
    if (_handler == nullptr)
        return;
    _currentTime = 0;
    _currentEvent = 0;
    _playing = false;

    // reset the handler
    _handler->OnReset();
}

Spawner FireworksSequence::AddSpawner(const TileCoordsXY& location)
{
    // check if there is a spawner on the location
    for (const auto& spawner : _spawners)
    {
        if (spawner.Location == location)
            return spawner;
    }

    // add the spawner and get the next available id
    Spawner spawner;
    spawner.Location = location;

    uint32_t newId = 0;
    auto result = std::begin(_spawners);
    do
    {
        newId++;
        result = std::find_if(std::begin(_spawners), std::end(_spawners), [newId](const auto& s) { return s.Id == newId; });
    } while (result != std::end(_spawners));

    spawner.Id = newId;
    _spawners.push_back(spawner);
    return spawner;
}

bool FireworksSequence::RemoveSpawner(const TileCoordsXY& location)
{
    auto result = std::find_if(
        std::begin(_spawners), std::end(_spawners), [location](const auto& s) { return s.Location == location; });
    if (result != std::end(_spawners))
    {
        _spawners.erase(result);
        return true;
    }
    return false;
}

const Spawner* OpenRCT2::Fireworks::FireworksSequence::GetSpawner(const uint32_t id) const
{
    auto spawner = std::find_if(_spawners.begin(), _spawners.end(), [id](const auto& s) { return s.Id == id; });
    if (spawner != _spawners.end())
        return &(* spawner);
    return nullptr;
}

Event FireworksSequence::AddEvent(
    const std::string& objectId, const Spawner& spawner, const uint32_t height, const uint32_t time, const colour_t palette1,
    const colour_t palette2, const colour_t palette3)
{
    Event e;
    e.ObjectId = objectId;
    e.SpawnerId = spawner.Id;
    e.Height = height;
    e.Time = time;
    e.Palette1 = palette1;
    e.Palette2 = palette2;
    e.Palette3 = palette3;

    // find an available id for the event
    uint32_t newId = 0;
    auto result = std::begin(_events);
    do
    {
        newId++;
        result = std::find_if(std::begin(_events), std::end(_events), [newId](const auto& e) { return e.Id == newId; });
    } while (result != std::end(_events));

    e.Id = newId;
    _events.push_back(e);

    // finally, resort the events from ascendant time
    std::sort(_events.begin(), _events.end(), [](const auto& e0, const auto& e1) { return e0.Time < e1.Time; });

    return e;
}

bool FireworksSequence::RemoveEvent(const uint32_t time, const Spawner& spawner)
{
    // remove all events that are on time and spawnerid
    std::vector<Event> toRemove;

    _events.erase(std::remove_if(_events.begin(), _events.end(), [spawner, time](const auto& e) {
        return e.Time == time && e.SpawnerId == spawner.Id;
    }));
    std::sort(_events.begin(), _events.end(), [](const auto& e0, const auto& e1) { return e0.Time < e1.Time; });
    return true;
}

bool OpenRCT2::Fireworks::FireworksSequence::HasFinished() const
{
    const auto& lastEvent = _events.back();
    return lastEvent.Time <= _currentTime;
}

void OpenRCT2::Fireworks::FireworksSequence::SetHandler(IFireworkHandler& handler)
{
    _handler = &handler;
}
