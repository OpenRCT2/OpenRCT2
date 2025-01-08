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

struct IStringReader
{
    virtual ~IStringReader() = default;

    virtual bool TryPeek(codepoint_t* outCodepoint) = 0;
    virtual bool TryRead(codepoint_t* outCodepoint) = 0;
    virtual void Skip() = 0;
    virtual bool CanRead() const = 0;
};

class UTF8StringReader final : public IStringReader
{
public:
    explicit UTF8StringReader(const utf8* text);

    bool TryPeek(codepoint_t* outCodepoint) override;
    bool TryRead(codepoint_t* outCodepoint) override;
    void Skip() override;
    bool CanRead() const override;

private:
    const utf8* _text;
    const utf8* _current;
};
