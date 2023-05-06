/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StringReader.h"

#include "../localisation/Localisation.h"
#include "../util/Util.h"
#include "String.hpp"

UTF8StringReader::UTF8StringReader(const utf8* text)
{
    text = String::SkipBOM(text);

    _text = text;
    _current = text;
}

bool UTF8StringReader::TryPeek(codepoint_t* outCodepoint)
{
    if (_current == nullptr)
        return false;

    codepoint_t codepoint = String::GetNextCodepoint(_current);
    *outCodepoint = codepoint;
    return true;
}

bool UTF8StringReader::TryRead(codepoint_t* outCodepoint)
{
    if (_current == nullptr)
        return false;

    codepoint_t codepoint = String::GetNextCodepoint(_current, &_current);
    *outCodepoint = codepoint;
    if (codepoint == 0)
    {
        _current = nullptr;
        return false;
    }
    return true;
}

void UTF8StringReader::Skip()
{
    codepoint_t codepoint;
    TryRead(&codepoint);
}

bool UTF8StringReader::CanRead() const
{
    return _current != nullptr;
}
