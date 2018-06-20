/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>
#include "Date.h"

namespace OpenRCT2
{
    class Park;

    /**
     * Class to update the state of the map and park.
     */
    class GameState final
    {
    private:
        std::unique_ptr<Park> _park;
        Date                  _date;

    public:
        GameState();
        GameState(const GameState&) = delete;

        Date& GetDate() { return _date; }
        Park& GetPark() { return *_park; }

        void InitAll(int32_t mapSize);
        void Update();
        void UpdateLogic();
    };
}
