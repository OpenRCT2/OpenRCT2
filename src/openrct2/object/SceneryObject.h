/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Object.h"

#include <string>
namespace OpenRCT2
{
    class SceneryObject : public Object
    {
    private:
        ObjectEntryDescriptor _primarySceneryGroupEntry = {};

    public:
        virtual ~SceneryObject() = default;

        const ObjectEntryDescriptor& GetPrimarySceneryGroup() const
        {
            return _primarySceneryGroupEntry;
        }

    protected:
        void SetPrimarySceneryGroup(const ObjectEntryDescriptor& entry)
        {
            _primarySceneryGroupEntry = entry;
        }
    };
} // namespace OpenRCT2
