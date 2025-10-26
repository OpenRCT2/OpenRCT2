/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CampaignObject.h"

#include "../core/Guard.hpp"
#include "../core/Json.hpp"
#include "../scenario/ScenarioCategory.h"
#include "../scenario/ScenarioSources.h"
#include "DefaultObjects.h"

using namespace OpenRCT2;

namespace OpenRCT2
{
    void CampaignObject::ReadJson(IReadObjectContext* context, json_t& root)
    {
        Guard::Assert(root.is_object(), "CampaignObject::ReadJson expects parameter root to be object");
        PopulateTablesFromJson(context, root);

        json_t properties = root["properties"];
        Guard::Assert(properties.is_object());

        // Populate with default values if not in json
        _numUnlockedScenarios = Json::GetNumber(properties["numUnlockedScenarios"], 0);
        _numUnlockedGroups = Json::GetNumber(properties["numUnlockedGroups"], 0);
        _isOfficial = std::find(kDefaultCampaignObjects.begin(), kDefaultCampaignObjects.end(), GetIdentifier())
            != kDefaultCampaignObjects.end();

        Guard::Assert(root["strings"].is_object());
        json_t stringTable = Json::AsObject(root["strings"]);

        Guard::Assert(properties["scenarioGroups"].is_array());
        json_t groupList = Json::AsArray(properties["scenarioGroups"]);
        for (size_t i = 0; i < groupList.size(); i++)
        {
            json_t group = groupList[i];
            Guard::Assert(group.is_object());
            ReadGroupJson(group, stringTable);
        }
    }

    void CampaignObject::ReadGroupJson(json_t& group, json_t& stringTable)
    {
        auto groupEntry = new CampaignGroup();

        // Get scenario list
        json_t scenarioList = Json::AsArray(group["scenarios"]);
        Guard::Assert(scenarioList.is_array());
        for (size_t i = 0; i < scenarioList.size(); i++)
        {
            json_t scenario = scenarioList[i];
            Guard::Assert(scenario.is_object());
            ReadScenarioJson(scenario, groupEntry);
        }

        // Exclude group if no scenarios were linked
        if (groupEntry->scenarioList.size() > 0)
        {
            // Read group header
            if (!group.contains("header"))
            {
                // Case 1: Json tag excluded, use default categorization
                groupEntry->headerType = HeaderType::headerTypeDefault;
            }
            else if (group["header"].is_string())
            {
                std::string headerKey = Json::GetString(group["header"]);

                auto category = MatchDefaultHeaderType(headerKey);
                if (category != Scenario::Category::none)
                {
                    // Case 2: try lookup corresponding Scenario::Category
                    groupEntry->headerType = HeaderType::headerTypePreset;
                    groupEntry->headerPreset = category;
                }
                else if (stringTable.contains(headerKey))
                {
                    // Case 3: Save key and use to point to stringTable customKey
                    groupEntry->headerType = HeaderType::headerTypeLocalized;
                    new (&groupEntry->headerString) std::string(headerKey);
                }
                else
                {
                    // Case 4: Just read the value directly
                    groupEntry->headerType = HeaderType::headerTypeDirect;
                    new (&groupEntry->headerString) std::string(headerKey);
                }
            }

            _groupList.push_back(groupEntry);
        }
    }

    bool CampaignObject::MatchAndAddScenario(CampaignGroup* group, std::string fileName, std::string internalName) const
    {
        const ScenarioIndexEntry* scenarioEntry = ScenarioRepositoryGetByFilename(fileName);
        if (scenarioEntry != nullptr && scenarioEntry->InternalName == internalName && !CampaignContainsScenario(scenarioEntry))
        {
            group->scenarioList.push_back(scenarioEntry);
            return true;
        }
        return false;
    }

    void CampaignObject::ReadScenarioJson(json_t& scenario, CampaignGroup* group)
    {
        Guard::Assert(scenario["title"].is_string());
        u8string internalName = Json::GetString(scenario["title"]);

        if (scenario["fileName"].is_string())
        {
            MatchAndAddScenario(group, Json::GetString(scenario["fileName"]), internalName);
        }
        // Support multiple valid filenames, prioritize first match
        else if (scenario["fileName"].is_array())
        {
            for (auto& fileNameItem : scenario["fileName"])
            {
                if (MatchAndAddScenario(group, Json::GetString(fileNameItem), internalName))
                {
                    return;
                }
            }
        }
    }

    void CampaignObject::Load()
    {
        // This should not matter...
        LoadImages();
    }

    void CampaignObject::Unload()
    {
        // ...But just in case
        UnloadImages();
    }

    bool CampaignObject::IsOfficial() const
    {
        return _isOfficial;
    }

    int32_t CampaignObject::GetNumUnlockedScenarios() const
    {
        return _numUnlockedScenarios;
    }

    int32_t CampaignObject::GetNumUnlockedGroups() const
    {
        return _numUnlockedGroups;
    }

    const CampaignGroup* CampaignObject::GetGroupAt(int32_t index) const
    {
        return _groupList.at(index);
    }

    size_t CampaignObject::GetGroupCount() const
    {
        return _groupList.size();
    }

    size_t CampaignObject::GetScenarioCount() const
    {
        size_t result = 0;
        for (const CampaignGroup* group : _groupList)
        {
            result += group->scenarioList.size();
        }
        return result;
    }

    std::vector<const ScenarioIndexEntry*> CampaignObject::GetScenarioList() const
    {
        auto totalList = std::vector<const ScenarioIndexEntry*>{ GetScenarioCount() };
        for (const CampaignGroup* group : _groupList)
        {
            for (const ScenarioIndexEntry* scenario : group->scenarioList)
            {
                totalList.push_back(scenario);
            }
        }
        return totalList;
    }

    bool CampaignObject::CampaignContainsScenario(const ScenarioIndexEntry* scenario) const
    {
        for (const CampaignGroup* group : _groupList)
        {
            if (std::find(group->scenarioList.begin(), group->scenarioList.end(), scenario) != group->scenarioList.end())
            {
                return true;
            }
        }
        return false;
    }

    const std::string CampaignObject::GetCustomString(const std::string* customKey) const
    {
        const std::string str = GetStringTable().GetCustomString(customKey);
        return str;
    }

    const std::map<std::string, Scenario::Category> gDefaultHeaderIDMap = {
        { "beginner", Scenario::Category::beginner },
        { "challenging", Scenario::Category::challenging },
        { "expert", Scenario::Category::expert },
        { "real", Scenario::Category::real },
        { "other", Scenario::Category::other },
        { "rctClassicGraphiteGroup", Scenario::Category::rctClassicGraphite },
        { "rctClassicEmeraldGroup", Scenario::Category::rctClassicEmerald },
        { "rctClassicRubyGroup", Scenario::Category::rctClassicRuby },
        { "rctClassicSapphireGroup", Scenario::Category::rctClassicSapphire },
        { "rctClassicAmethystGroup", Scenario::Category::rctClassicAmethyst },
        { "rctClassicCoralGroup", Scenario::Category::rctClassicCoral },
        { "rctClassicBronzeGroup", Scenario::Category::rctClassicBronze },
        { "rctClassicIvoryGroup", Scenario::Category::rctClassicIvory },
        { "rctClassicSilverGroup", Scenario::Category::rctClassicSilver },
        { "rctClassicGoldGroup", Scenario::Category::rctClassicGold },
        { "dlc", Scenario::Category::dlc },
        { "buildYourOwn", Scenario::Category::buildYourOwn },
        { "competitions", Scenario::Category::competitions },
        { "timeMachine", Scenario::Category::timeMachine },
        { "katysDreamworld", Scenario::Category::katysDreamworld },
        { "bonus", Scenario::Category::bonus },
    };

    Scenario::Category MatchDefaultHeaderType(std::string headerId)
    {
        auto item = gDefaultHeaderIDMap.find(headerId);
        if (item != gDefaultHeaderIDMap.end())
        {
            return item->second;
        }
        return Scenario::Category::none;
    }

    size_t CampaignGroup::GetScenarioCount() const
    {
        return scenarioList.size();
    }
} // namespace OpenRCT2
