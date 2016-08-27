#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include <vector>

extern "C"
{
    #include "../common.h"
    #include "../util/util.h"
    #include "localisation.h"
}

#include "../core/StringBuilder.hpp"
#include "../core/StringReader.hpp"

class LanguagePack final {
public:
    static LanguagePack * FromFile(uint16 id, const utf8 * path);
    static LanguagePack * FromText(uint16 id, const utf8 * text);

    ~LanguagePack();

    uint16 GetId()    const { return _id;             }
    uint32 GetCount() const { return (uint32)_strings.size(); }

    const utf8 * GetString(rct_string_id stringId) const;

    void SetString(rct_string_id stringId, const utf8 * str)
    {
        if (_strings.size() >= (size_t)stringId)
        {
            _strings[stringId] = str;
        }
    }

    rct_string_id GetObjectOverrideStringId(const char * objectIdentifier, uint8 index);
    rct_string_id GetScenarioOverrideStringId(const utf8 * scenarioFilename, uint8 index);

private:
    struct ObjectOverride
    {
        char         name[8];
        const utf8 * strings[4];
    };

    struct ScenarioOverride
    {
        const utf8 * filename;
        union {
            const utf8 * strings[3];
            struct {
                const utf8 * name;
                const utf8 * park;
                const utf8 * details;
            };
        };
    };

    uint16 _id;
    utf8 * _stringData;

    std::vector<const utf8*>      _strings;
    std::vector<ObjectOverride>   _objectOverrides;
    std::vector<ScenarioOverride> _scenarioOverrides;

    LanguagePack(uint16 id, const utf8 * text);

    ObjectOverride *   GetObjectOverride(const char * objectIdentifier);
    ScenarioOverride * GetScenarioOverride(const utf8 * scenarioFilename);

    ///////////////////////////////////////////////////////////////////////////
    // Parsing
    ///////////////////////////////////////////////////////////////////////////
    StringBuilder      _stringDataSB;
    utf8 *             _currentGroup;
    ObjectOverride *   _currentObjectOverride;
    ScenarioOverride * _currentScenarioOverride;

    void ParseLine(IStringReader * reader);
    void ParseGroupObject(IStringReader * reader);
    void ParseGroupScenario(IStringReader * reader);
    void ParseString(IStringReader * reader);

    bool ParseToken(IStringReader * reader, uint32 * token, bool * isByte);
};
