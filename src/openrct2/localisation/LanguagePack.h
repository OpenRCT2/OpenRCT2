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
#include "../common.h"

interface ILanguagePack
{
    virtual ~ILanguagePack() = default;

    virtual uint16 GetId() const abstract;
    virtual uint32 GetCount() const abstract;

    virtual void            RemoveString(rct_string_id stringId) abstract;
    virtual void            SetString(rct_string_id stringId, const std::string &str) abstract;
    virtual const utf8 *    GetString(rct_string_id stringId) const abstract;
    virtual rct_string_id   GetObjectOverrideStringId(const char * objectIdentifier, uint8 index) abstract;
    virtual rct_string_id   GetScenarioOverrideStringId(const utf8 * scenarioFilename, uint8 index) abstract;
};

namespace LanguagePackFactory
{
    ILanguagePack * FromFile(uint16 id, const utf8 * path);
    ILanguagePack * FromText(uint16 id, const utf8 * text);
}
