/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <string>
#include "Object.h"

class SceneryObject : public Object
{
private:
    rct_object_entry    _primarySceneryGroupEntry = { };

public:
    explicit SceneryObject(const rct_object_entry &entry) : Object(entry) { }
    virtual ~SceneryObject() = default;

    const rct_object_entry * GetPrimarySceneryGroup() { return &_primarySceneryGroupEntry; }

protected:
    void SetPrimarySceneryGroup(const rct_object_entry * entry) { _primarySceneryGroupEntry = *entry; }
    void SetPrimarySceneryGroup(const std::string &s);
};
