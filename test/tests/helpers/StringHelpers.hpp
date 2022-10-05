/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <cassert>
#include <string>
#include <string_view>

inline std::string StringFromHex(std::string_view input)
{
    assert((input.size() & 1) == 0);

    std::string result;
    result.reserve(input.size() / 2);
    for (size_t i = 0; i < input.size(); i += 2)
    {
        auto val = std::stoi(std::string(input.substr(i, 2)), nullptr, 16);
        result.push_back(val);
    }
    return result;
}

inline std::string NormaliseLineEndings(std::string_view input)
{
    std::string result;
    result.reserve(input.size());
    auto ignoreNextNewLine = false;
    for (auto c : input)
    {
        if (c == '\r')
        {
            ignoreNextNewLine = true;
            result.push_back('\n');
        }
        else if (c == '\n')
        {
            if (ignoreNextNewLine)
            {
                ignoreNextNewLine = false;
            }
            else
            {
                result.push_back('\n');
            }
        }
        else
        {
            ignoreNextNewLine = false;
            result.push_back(c);
        }
    }
    return result;
}
