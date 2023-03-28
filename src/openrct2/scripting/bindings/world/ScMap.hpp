/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
#    include "../ride/ScRide.hpp"
#    include "../ride/ScTrackIterator.h"
#    include "../world/ScTile.hpp"

namespace OpenRCT2::Scripting
{
    class ScMap
    {
    private:
        duk_context* _context;

    public:
        ScMap(duk_context* ctx);

        DukValue size_get() const;

        int32_t numRides_get() const;

        int32_t numEntities_get() const;

        std::vector<std::shared_ptr<ScRide>> rides_get() const;

        std::shared_ptr<ScRide> getRide(int32_t id) const;

        std::shared_ptr<ScTile> getTile(int32_t x, int32_t y) const;

        DukValue getEntity(int32_t id) const;

        std::vector<DukValue> getAllEntities(const std::string& type) const;

        std::vector<DukValue> getAllEntitiesOnTile(const std::string& type, const DukValue& tilePos) const;

        DukValue createEntity(const std::string& type, const DukValue& initializer);

        DukValue getTrackIterator(const DukValue& position, int32_t elementIndex) const;

        static void Register(duk_context* ctx);

    private:
        DukValue GetEntityAsDukValue(const EntityBase* sprite) const;
    };

} // namespace OpenRCT2::Scripting

#endif
