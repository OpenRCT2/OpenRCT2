/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScResearch.hpp"

    #include "../../../Context.h"
    #include "../../../GameState.h"
    #include "../../../core/String.hpp"
    #include "../../../management/Research.h"
    #include "../../../ride/RideData.h"
    #include "../../Duktape.hpp"
    #include "../../ScriptEngine.h"
    #include "../object/ScObject.hpp"

using namespace OpenRCT2;

namespace OpenRCT2::Scripting
{
    static const DukEnumMap<uint8_t> ResearchStageMap({
        { "initial_research", RESEARCH_STAGE_INITIAL_RESEARCH },
        { "designing", RESEARCH_STAGE_DESIGNING },
        { "completing_design", RESEARCH_STAGE_COMPLETING_DESIGN },
        { "unknown", RESEARCH_STAGE_UNKNOWN },
        { "finished_all", RESEARCH_STAGE_FINISHED_ALL },
    });

    static const DukEnumMap<ResearchCategory> ResearchCategoryMap({
        { "transport", ResearchCategory::Transport },
        { "gentle", ResearchCategory::Gentle },
        { "rollercoaster", ResearchCategory::Rollercoaster },
        { "thrill", ResearchCategory::Thrill },
        { "water", ResearchCategory::Water },
        { "shop", ResearchCategory::Shop },
        { "scenery", ResearchCategory::SceneryGroup },
    });

    static const DukEnumMap<Research::EntryType> ResearchEntryTypeMap({
        { "ride", Research::EntryType::Ride },
        { "scenery", Research::EntryType::Scenery },
    });

    template<>
    inline DukValue ToDuk(duk_context* ctx, const ResearchItem& value)
    {
        DukObject obj(ctx);
        obj.Set("category", ResearchCategoryMap[value.category]);
        obj.Set("type", ResearchEntryTypeMap[value.type]);
        if (value.type == Research::EntryType::Ride)
        {
            obj.Set("rideType", value.baseRideType);
        }
        obj.Set("object", value.entryIndex);
        return obj.Take();
    }

    template<>
    Research::EntryType inline FromDuk(const DukValue& d)
    {
        if (d.type() == DukValue::STRING)
        {
            auto it = ResearchEntryTypeMap.find(d.as_string());
            if (it != ResearchEntryTypeMap.end())
            {
                return it->second;
            }
        }
        return Research::EntryType::Scenery;
    }

    template<>
    ResearchItem inline FromDuk(const DukValue& d)
    {
        ResearchItem result;
        result.baseRideType = 0;
        result.category = {}; // We ignore category because it will be derived from ride type
        result.flags = 0;
        result.type = FromDuk<Research::EntryType>(d["type"]);
        auto baseRideType = d["rideType"];
        if (baseRideType.type() == DukValue::NUMBER)
            result.baseRideType = baseRideType.as_uint();
        result.entryIndex = d["object"].as_uint();
        return result;
    }

    ScResearch::ScResearch(duk_context* ctx)
        : _context(ctx)
    {
    }

    uint8_t ScResearch::funding_get() const
    {
        return GetGameState().ResearchFundingLevel;
    }

    void ScResearch::funding_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        GetGameState().ResearchFundingLevel = std::clamp<uint8_t>(value, RESEARCH_FUNDING_NONE, RESEARCH_FUNDING_MAXIMUM);
    }

    std::vector<std::string> ScResearch::priorities_get() const
    {
        std::vector<std::string> result;
        for (auto i = EnumValue(ResearchCategory::Transport); i <= EnumValue(ResearchCategory::SceneryGroup); i++)
        {
            auto category = static_cast<ResearchCategory>(i);
            if (GetGameState().ResearchPriorities & EnumToFlag(category))
            {
                result.emplace_back(ResearchCategoryMap[category]);
            }
        }
        return result;
    }

    void ScResearch::priorities_set(const std::vector<std::string>& values)
    {
        ThrowIfGameStateNotMutable();

        auto priorities = 0;
        for (const auto& value : values)
        {
            auto category = ResearchCategoryMap.TryGet(value);
            if (category)
            {
                priorities |= EnumToFlag(*category);
            }
        }
        GetGameState().ResearchPriorities = priorities;
    }

    std::string ScResearch::stage_get() const
    {
        return std::string(ResearchStageMap[GetGameState().ResearchProgressStage]);
    }

    void ScResearch::stage_set(const std::string& value)
    {
        ThrowIfGameStateNotMutable();
        auto it = ResearchStageMap.find(value);
        if (it != ResearchStageMap.end())
        {
            GetGameState().ResearchProgressStage = it->second;
        }
    }

    uint16_t ScResearch::progress_get() const
    {
        return GetGameState().ResearchProgress;
    }

    void ScResearch::progress_set(uint16_t value)
    {
        ThrowIfGameStateNotMutable();
        GetGameState().ResearchProgress = value;
    }

    DukValue ScResearch::expectedMonth_get() const
    {
        const auto& gameState = GetGameState();
        if (gameState.ResearchProgressStage == RESEARCH_STAGE_INITIAL_RESEARCH || gameState.ResearchExpectedDay == 255)
            return ToDuk(_context, nullptr);
        return ToDuk(_context, gameState.ResearchExpectedMonth);
    }

    DukValue ScResearch::expectedDay_get() const
    {
        const auto& gameState = GetGameState();
        if (gameState.ResearchProgressStage == RESEARCH_STAGE_INITIAL_RESEARCH || gameState.ResearchExpectedDay == 255)
            return ToDuk(_context, nullptr);
        return ToDuk(_context, gameState.ResearchExpectedDay + 1);
    }

    DukValue ScResearch::lastResearchedItem_get() const
    {
        const auto& gameState = GetGameState();
        if (!gameState.ResearchLastItem)
            return ToDuk(_context, nullptr);
        return ToDuk(_context, *gameState.ResearchLastItem);
    }

    DukValue ScResearch::expectedItem_get() const
    {
        const auto& gameState = GetGameState();
        if (gameState.ResearchProgressStage == RESEARCH_STAGE_INITIAL_RESEARCH || !gameState.ResearchNextItem)
            return ToDuk(_context, nullptr);
        return ToDuk(_context, *gameState.ResearchNextItem);
    }

    std::vector<DukValue> ScResearch::inventedItems_get() const
    {
        std::vector<DukValue> result;
        for (auto& item : GetGameState().ResearchItemsInvented)
        {
            result.push_back(ToDuk(_context, item));
        }
        return result;
    }

    void ScResearch::inventedItems_set(const std::vector<DukValue>& value)
    {
        ThrowIfGameStateNotMutable();
        auto list = ConvertResearchList(value);
        GetGameState().ResearchItemsInvented = std::move(list);
        ResearchFix();
    }

    std::vector<DukValue> ScResearch::uninventedItems_get() const
    {
        std::vector<DukValue> result;
        for (auto& item : GetGameState().ResearchItemsUninvented)
        {
            result.push_back(ToDuk(_context, item));
        }
        return result;
    }

    void ScResearch::uninventedItems_set(const std::vector<DukValue>& value)
    {
        ThrowIfGameStateNotMutable();
        auto list = ConvertResearchList(value);
        GetGameState().ResearchItemsUninvented = std::move(list);
        ResearchFix();
    }

    bool ScResearch::isObjectResearched(const std::string& typez, ObjectEntryIndex index)
    {
        auto result = false;
        auto type = ScObject::StringToObjectType(typez);
        if (type)
        {
            result = ResearchIsInvented(*type, index);
        }
        else
        {
            duk_error(_context, DUK_ERR_ERROR, "Invalid object type.");
        }
        return result;
    }

    void ScResearch::Register(duk_context* ctx)
    {
        dukglue_register_property(ctx, &ScResearch::funding_get, &ScResearch::funding_set, "funding");
        dukglue_register_property(ctx, &ScResearch::priorities_get, &ScResearch::priorities_set, "priorities");
        dukglue_register_property(ctx, &ScResearch::stage_get, &ScResearch::stage_set, "stage");
        dukglue_register_property(ctx, &ScResearch::progress_get, &ScResearch::progress_set, "progress");
        dukglue_register_property(ctx, &ScResearch::expectedMonth_get, nullptr, "expectedMonth");
        dukglue_register_property(ctx, &ScResearch::expectedDay_get, nullptr, "expectedDay");
        dukglue_register_property(ctx, &ScResearch::lastResearchedItem_get, nullptr, "lastResearchedItem");
        dukglue_register_property(ctx, &ScResearch::expectedItem_get, nullptr, "expectedItem");
        dukglue_register_property(ctx, &ScResearch::inventedItems_get, &ScResearch::inventedItems_set, "inventedItems");
        dukglue_register_property(ctx, &ScResearch::uninventedItems_get, &ScResearch::uninventedItems_set, "uninventedItems");
        dukglue_register_method(ctx, &ScResearch::isObjectResearched, "isObjectResearched");
    }

    std::vector<ResearchItem> ScResearch::ConvertResearchList(const std::vector<DukValue>& value)
    {
        auto& objManager = GetContext()->GetObjectManager();
        std::vector<ResearchItem> result;
        for (auto& item : value)
        {
            auto researchItem = FromDuk<ResearchItem>(item);
            researchItem.flags = 0;
            if (researchItem.type == Research::EntryType::Ride)
            {
                auto rideEntry = GetRideEntryByIndex(researchItem.entryIndex);
                if (rideEntry != nullptr)
                {
                    researchItem.category = GetRideTypeDescriptor(researchItem.baseRideType).GetResearchCategory();
                    result.push_back(researchItem);
                }
            }
            else
            {
                auto sceneryGroup = objManager.GetLoadedObject(ObjectType::SceneryGroup, researchItem.entryIndex);
                if (sceneryGroup != nullptr)
                {
                    researchItem.baseRideType = 0;
                    researchItem.category = ResearchCategory::SceneryGroup;
                    result.push_back(researchItem);
                }
            }
        }
        return result;
    }
} // namespace OpenRCT2::Scripting

#endif
