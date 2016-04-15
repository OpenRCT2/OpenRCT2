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
    uint32 GetCount() const { return _strings.size(); }

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
