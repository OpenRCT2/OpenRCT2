/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
    static const DukEnumMap<uint8_t> ResearchStageMap(
        {
            { "initial_research", RESEARCH_STAGE_INITIAL_RESEARCH },
            { "designing", RESEARCH_STAGE_DESIGNING },
            { "completing_design", RESEARCH_STAGE_COMPLETING_DESIGN },
            { "unknown", RESEARCH_STAGE_UNKNOWN },
            { "finished_all", RESEARCH_STAGE_FINISHED_ALL },
        });

    static const DukEnumMap<ResearchCategory> ResearchCategoryMap(
        {
            { "transport", ResearchCategory::transport },
            { "gentle", ResearchCategory::gentle },
            { "rollercoaster", ResearchCategory::rollercoaster },
            { "thrill", ResearchCategory::thrill },
            { "water", ResearchCategory::water },
            { "shop", ResearchCategory::shop },
            { "scenery", ResearchCategory::sceneryGroup },
        });

    static const DukEnumMap<Research::EntryType> ResearchEntryTypeMap(
        {
            { "ride", Research::EntryType::ride },
            { "scenery", Research::EntryType::scenery },
        });

    template<>
    inline DukValue ToDuk(duk_context* ctx, const ResearchItem& value)
    {
        DukObject obj(ctx);
        obj.Set("category", ResearchCategoryMap[value.category]);
        obj.Set("type", ResearchEntryTypeMap[value.type]);
        if (value.type == Research::EntryType::ride)
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
        return Research::EntryType::scenery;
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
        return getGameState().researchFundingLevel;
    }

    void ScResearch::funding_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        getGameState().researchFundingLevel = std::clamp<uint8_t>(value, RESEARCH_FUNDING_NONE, RESEARCH_FUNDING_MAXIMUM);
    }

    std::vector<std::string> ScResearch::priorities_get() const
    {
        std::vector<std::string> result;
        for (auto i = EnumValue(ResearchCategory::transport); i <= EnumValue(ResearchCategory::sceneryGroup); i++)
        {
            auto category = static_cast<ResearchCategory>(i);
            if (getGameState().researchPriorities & EnumToFlag(category))
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
        getGameState().researchPriorities = priorities;
    }

    std::string ScResearch::stage_get() const
    {
        return std::string(ResearchStageMap[getGameState().researchProgressStage]);
    }

    void ScResearch::stage_set(const std::string& value)
    {
        ThrowIfGameStateNotMutable();
        auto it = ResearchStageMap.find(value);
        if (it != ResearchStageMap.end())
        {
            getGameState().researchProgressStage = it->second;
        }
    }

    uint16_t ScResearch::progress_get() const
    {
        return getGameState().researchProgress;
    }

    void ScResearch::progress_set(uint16_t value)
    {
        ThrowIfGameStateNotMutable();
        getGameState().researchProgress = value;
    }

    DukValue ScResearch::expectedMonth_get() const
    {
        const auto& gameState = getGameState();
        if (gameState.researchProgressStage == RESEARCH_STAGE_INITIAL_RESEARCH || gameState.researchExpectedDay == 255)
            return ToDuk(_context, nullptr);
        return ToDuk(_context, gameState.researchExpectedMonth);
    }

    DukValue ScResearch::expectedDay_get() const
    {
        const auto& gameState = getGameState();
        if (gameState.researchProgressStage == RESEARCH_STAGE_INITIAL_RESEARCH || gameState.researchExpectedDay == 255)
            return ToDuk(_context, nullptr);
        return ToDuk(_context, gameState.researchExpectedDay + 1);
    }

    DukValue ScResearch::lastResearchedItem_get() const
    {
        const auto& gameState = getGameState();
        if (!gameState.researchLastItem)
            return ToDuk(_context, nullptr);
        return ToDuk(_context, *gameState.researchLastItem);
    }

    DukValue ScResearch::expectedItem_get() const
    {
        const auto& gameState = getGameState();
        if (gameState.researchProgressStage == RESEARCH_STAGE_INITIAL_RESEARCH || !gameState.researchNextItem)
            return ToDuk(_context, nullptr);
        return ToDuk(_context, *gameState.researchNextItem);
    }

    std::vector<DukValue> ScResearch::inventedItems_get() const
    {
        std::vector<DukValue> result;
        for (auto& item : getGameState().researchItemsInvented)
        {
            result.push_back(ToDuk(_context, item));
        }
        return result;
    }

    void ScResearch::inventedItems_set(const std::vector<DukValue>& value)
    {
        ThrowIfGameStateNotMutable();
        auto list = ConvertResearchList(value);
        getGameState().researchItemsInvented = std::move(list);
        ResearchFix();
    }

    std::vector<DukValue> ScResearch::uninventedItems_get() const
    {
        std::vector<DukValue> result;
        for (auto& item : getGameState().researchItemsUninvented)
        {
            result.push_back(ToDuk(_context, item));
        }
        return result;
    }

    void ScResearch::uninventedItems_set(const std::vector<DukValue>& value)
    {
        ThrowIfGameStateNotMutable();
        auto list = ConvertResearchList(value);
        getGameState().researchItemsUninvented = std::move(list);
        ResearchFix();
    }

    bool ScResearch::isObjectResearched(const std::string& typez, ObjectEntryIndex index)
    {
        auto result = false;
        auto type = objectTypeFromString(typez);
        if (type != ObjectType::none)
        {
            result = ResearchIsInvented(type, index);
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
            if (researchItem.type == Research::EntryType::ride)
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
                auto sceneryGroup = objManager.GetLoadedObject<SceneryGroupObject>(researchItem.entryIndex);
                if (sceneryGroup != nullptr)
                {
                    researchItem.baseRideType = 0;
                    researchItem.category = ResearchCategory::sceneryGroup;
                    result.push_back(researchItem);
                }
            }
        }
        return result;
    }
} // namespace OpenRCT2::Scripting

#endif
