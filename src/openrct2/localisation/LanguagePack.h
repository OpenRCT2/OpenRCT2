/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/StringTypes.h"
#include "../localisation/StringIdType.h"

#include <memory>

struct ILanguagePack
{
    virtual ~ILanguagePack() = default;

    virtual uint16_t GetId() const = 0;
    virtual uint32_t GetCount() const = 0;

    virtual void RemoveString(StringId stringId) = 0;
    virtual void SetString(StringId stringId, const std::string& str) = 0;
    virtual const utf8* GetString(StringId stringId) const = 0;
};

namespace OpenRCT2::LanguagePackFactory
{
    std::unique_ptr<ILanguagePack> FromFile(uint16_t id, const utf8* path);
    std::unique_ptr<ILanguagePack> FromLanguageId(uint16_t id);
    std::unique_ptr<ILanguagePack> FromText(uint16_t id, const utf8* text);
} // namespace OpenRCT2::LanguagePackFactory
