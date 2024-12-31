/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StringBuilder.h"

#include "String.hpp"

#include <iterator>

using namespace OpenRCT2;

StringBuilder::StringBuilder(size_t capacity)
{
    _buffer.reserve(capacity);
}

void StringBuilder::Append(int32_t codepoint)
{
    Append(static_cast<codepoint_t>(codepoint));
}

void StringBuilder::Append(codepoint_t codepoint)
{
    size_t codepointLength = String::getCodepointLength(codepoint);
    std::basic_string<utf8> data(codepointLength, {});
    String::writeCodepoint(data.data(), codepoint);
    _buffer.insert(_buffer.end(), data.begin(), data.end());
}

void StringBuilder::Append(std::string_view text)
{
    size_t textLength = text.length();
    Append(text.data(), textLength);
}

void StringBuilder::Append(const utf8* text, size_t textLength)
{
    _buffer.insert(_buffer.end(), text, text + textLength);
}

void StringBuilder::Append(const StringBuilder* sb)
{
    Append(sb->GetBuffer(), sb->GetLength());
}

void StringBuilder::Clear()
{
    _buffer.clear();
}

std::string StringBuilder::GetStdString() const
{
    return std::string(GetBuffer(), GetLength());
}

const utf8* StringBuilder::GetBuffer() const
{
    // buffer may be empty, so return an immutable empty string
    if (_buffer.empty())
        return "";
    return _buffer.c_str();
}

size_t StringBuilder::GetLength() const
{
    return _buffer.size();
}
