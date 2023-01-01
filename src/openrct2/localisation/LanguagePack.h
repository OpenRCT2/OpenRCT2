/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/String.hpp"

#include <memory>
#include <string>
#include <string_view>

struct ILanguagePack
{
    virtual ~ILanguagePack() = default;

    virtual uint16_t GetId() const abstract;
    virtual uint32_t GetCount() const abstract;

    virtual void RemoveString(StringId stringId) abstract;
    virtual void SetString(StringId stringId, const std::string& str) abstract;
    virtual const utf8* GetString(StringId stringId) const abstract;
    virtual StringId GetObjectOverrideStringId(std::string_view legacyIdentifier, uint8_t index) abstract;
    virtual StringId GetScenarioOverrideStringId(const utf8* scenarioFilename, uint8_t index) abstract;
};

namespace LanguagePackFactory
{
    std::unique_ptr<ILanguagePack> FromFile(uint16_t id, const utf8* path);
    std::unique_ptr<ILanguagePack> FromText(uint16_t id, const utf8* text);
} // namespace LanguagePackFactory
