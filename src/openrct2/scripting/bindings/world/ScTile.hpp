/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../../../common.h"
#    include "../../Duktape.hpp"
#    include "ScTileElement.hpp"

#    include <cstdio>
#    include <cstring>
#    include <utility>
#    include <vector>

namespace OpenRCT2::Scripting
{
    class ScTile
    {
    private:
        CoordsXY _coords;

    public:
        ScTile(const CoordsXY& coords);

    private:
        int32_t x_get() const;

        int32_t y_get() const;

        uint32_t numElements_get() const;

        std::vector<std::shared_ptr<ScTileElement>> elements_get() const;

        DukValue data_get() const;
        void data_set(DukValue value);

        std::shared_ptr<ScTileElement> getElement(uint32_t index) const;
        std::shared_ptr<ScTileElement> insertElement(uint32_t index);

        void removeElement(uint32_t index);

        TileElement* GetFirstElement() const;

        static size_t GetNumElements(const TileElement* first);

        duk_context* GetDukContext() const;

    public:
        static void Register(duk_context* ctx);
    };
} // namespace OpenRCT2::Scripting

#endif
