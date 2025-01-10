/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../config/Config.h"

using namespace OpenRCT2;

template<typename T>
class DrawingUniqueLock
{
    T& _mutex;
    const bool _enabled;

public:
    DrawingUniqueLock(T& mutex)
        : _mutex(mutex)
        , _enabled(Config::Get().general.MultiThreading)
    {
        if (_enabled)
            _mutex.lock();
    }
    ~DrawingUniqueLock()
    {
        if (_enabled)
            _mutex.unlock();
    }
};

template<typename T>
class DrawingSharedLock
{
    T& _mutex;
    const bool _enabled;

public:
    DrawingSharedLock(T& mutex)
        : _mutex(mutex)
        , _enabled(Config::Get().general.MultiThreading)
    {
        if (_enabled)
            _mutex.lock_shared();
    }
    ~DrawingSharedLock()
    {
        if (_enabled)
            _mutex.unlock_shared();
    }
};
