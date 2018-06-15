/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../localisation/Localisation.h"
#include "../util/Util.h"
#include "String.hpp"

interface IStringReader
{
    virtual ~IStringReader() = default;

    virtual bool TryPeek(codepoint_t * outCodepoint)       abstract;
    virtual bool TryRead(codepoint_t * outCodepoint)       abstract;
    virtual void Skip()                                    abstract;
    virtual bool CanRead()                           const abstract;
};

class UTF8StringReader final : public IStringReader
{
public:
    explicit UTF8StringReader(const utf8 * text)
    {
        text = String::SkipBOM(text);

        _text = text;
        _current = text;
    }

    bool TryPeek(codepoint_t * outCodepoint) override
    {
        if (_current == nullptr) return false;

        codepoint_t codepoint = String::GetNextCodepoint(_current);
        *outCodepoint = codepoint;
        return true;
    }

    bool TryRead(codepoint_t * outCodepoint) override
    {
        if (_current == nullptr) return false;

        codepoint_t codepoint = String::GetNextCodepoint(_current, &_current);
        *outCodepoint = codepoint;
        if (codepoint == 0)
        {
            _current = nullptr;
            return false;
        }
        return true;
    }

    void Skip() override
    {
        codepoint_t codepoint;
        TryRead(&codepoint);
    }

    bool CanRead() const override
    {
        return _current != nullptr;
    }

private:
    const utf8 *_text;
    const utf8 *_current;
};
