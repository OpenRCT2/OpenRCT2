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

        size_t GetScenarioCount() const;
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

        void ReadGroupJson(json_t& group, json_t& stringTable);
        void ReadScenarioJson(json_t& scenario, CampaignGroup* group);
        bool MatchAndAddScenario(CampaignGroup* group, std::string fileName, std::string internalName) const;

    public:
        void ReadJson(IReadObjectContext* context, json_t& root) override;
        void Load() override;
        void Unload() override;

        bool IsOfficial() const;
        int32_t GetNumUnlockedScenarios() const;
        int32_t GetNumUnlockedGroups() const;

        const CampaignGroup* GetGroupAt(int32_t index) const;
        size_t GetGroupCount() const;

        // Gets total number of scenarios in campaign across all groups
        size_t GetScenarioCount() const;
        // Gets combined list of all scenarios across all groups
        std::vector<const ScenarioIndexEntry*> GetScenarioList() const;

        // Returns whether scenario is contained within any groups
        bool CampaignContainsScenario(const ScenarioIndexEntry* scenario) const;

        const std::string GetCustomString(const std::string* customKey) const;
    };

    Scenario::Category MatchDefaultHeaderType(std::string headerId);
} // namespace OpenRCT2
