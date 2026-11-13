/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../scenario/ScenarioCategory.h"
#include "../scenario/ScenarioRepository.h"
#include "Object.h"

#include <array>

namespace OpenRCT2
{
    struct IReadObjectContext;

    enum class HeaderType : uint8_t
    {
        // Group header unspecified, use default behavior
        headerTypeDefault,
        // Group header specified as predefined Scenario::Category type
        headerTypePreset,
        // Group header specified in JSON string table
        headerTypeLocalized,
        // Group header read directly from JSON value
        headerTypeDirect
    };

    struct CampaignGroup
    {
    public:
        CampaignGroup()
        {
            headerType = HeaderType::headerTypeDirect;
            headerPreset = Scenario::Category::none;
        }

        ~CampaignGroup()
        {
        }

        HeaderType headerType;
        std::vector<const ScenarioIndexEntry*> scenarioList;
        union
        {
            Scenario::Category headerPreset;
            std::string headerString;
        };

        size_t getScenarioCount() const;
    };

    class CampaignObject final : public Object
    {
    private:
        // Whether the campaign is officially bundled with OpenRCT2, or a custom list.
        bool _isOfficial;
        // Number of scenario groups (by header/discriminator) to be unlocked at a time.
        // Fully completing a group will unlock the next. (RCTC style progression)
        // If excluded from JSON group-based unlocking will be disabled
        int32_t _numUnlockedGroups;
        // Number of scenarios to be unlocked at a time, if unlocking is enabled.
        int32_t _numUnlockedScenarios;

        std::vector<const CampaignGroup*> _groupList;

        void readGroupJson(json_t& group, json_t& stringTable);
        void readScenarioJson(json_t& scenario, CampaignGroup* group);
        bool matchAndAddScenario(CampaignGroup* group, std::string fileName, std::string internalName) const;

    public:
        void ReadJson(IReadObjectContext* context, json_t& root) override;
        void Load() override;
        void Unload() override;

        bool isOfficial() const;
        int32_t getNumUnlockedScenarios() const;
        int32_t getNumUnlockedGroups() const;

        const CampaignGroup* getGroupAt(int32_t index) const;
        size_t getGroupCount() const;

        // Gets total number of scenarios in campaign across all groups
        size_t getScenarioCount() const;
        // Gets combined list of all scenarios across all groups
        std::vector<const ScenarioIndexEntry*> getScenarioList() const;

        // Returns whether scenario is contained within any groups
        bool campaignContainsScenario(const ScenarioIndexEntry* scenario) const;

        const std::string getCustomString(const std::string* customKey) const;
    };

    Scenario::Category matchDefaultHeaderType(std::string headerId);
} // namespace OpenRCT2
