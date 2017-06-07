#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#include "../common.h"
#include "../localisation/localisation.h"
#include "../util/util.h"
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
