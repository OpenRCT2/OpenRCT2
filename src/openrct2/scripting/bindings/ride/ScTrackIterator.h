/*****************************************************************************
 * Copyright (c) 2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include "../../../Identifiers.h"
    #include "../../Duktape.hpp"

    #include <cstdint>
    #include <memory>

namespace OpenRCT2::Scripting
{
    class ScTrackIterator
    {
    private:
        CoordsXYZD _position;
        OpenRCT2::TrackElemType _type;
        [[maybe_unused]] RideId _ride;

    public:
        static std::shared_ptr<ScTrackIterator> FromElement(const CoordsXY& position, int32_t elementIndex);
        static void Register(duk_context* ctx);

        ScTrackIterator(const CoordsXYZD& position, OpenRCT2::TrackElemType type, RideId ride);

    private:
        DukValue position_get() const;
        DukValue segment_get() const;
        DukValue previousPosition_get() const;
        DukValue nextPosition_get() const;

        bool previous();
        bool next();
    };

} // namespace OpenRCT2::Scripting

#endif
