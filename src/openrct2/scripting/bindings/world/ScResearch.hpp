/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../../../Context.h"
#    include "../../../common.h"
#    include "../../../core/String.hpp"
#    include "../../../management/Research.h"
#    include "../../../ride/RideData.h"
#    include "../../Duktape.hpp"
#    include "../../ScriptEngine.h"
#    include "../object/ScObject.hpp"

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
        { "scenery_group", ResearchCategory::SceneryGroup },
    });

    static const DukEnumMap<Research::EntryType> ResearchEntryTypeMap({
        { "ride", Research::EntryType::Ride },
        { "scenery", Research::EntryType::Scenery },
    });

    template<> inline DukValue ToDuk(duk_context* ctx, const ResearchItem& value)
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

    template<> Research::EntryType inline FromDuk(const DukValue& d)
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

    template<> ResearchItem inline FromDuk(const DukValue& d)
    {
        ResearchItem result;
        result.baseRideType = 0;
        result.category = ResearchCategory::Transport;
        result.flags = 0;
        result.type = FromDuk<Research::EntryType>(d["type"]);
        auto baseRideType = d["rideType"];
        if (baseRideType.type() == DukValue::NUMBER)
            result.baseRideType = baseRideType.as_int();
        result.entryIndex = d["object"].as_int();
        return result;
    }

    class ScResearch
    {
    private:
        duk_context* _context;

    public:
        ScResearch(duk_context* ctx)
            : _context(ctx)
        {
        }

        uint8_t funding_get() const
        {
            return gResearchFundingLevel;
        }

        void funding_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            gResearchFundingLevel = value;
        }

        uint8_t priorities_get() const
        {
            return gResearchPriorities;
        }

        void priorities_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            gResearchPriorities = value;
        }

        std::string stage_get() const
        {
            return std::string(ResearchStageMap[gResearchProgressStage]);
        }

        void stage_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            auto it = ResearchStageMap.find(value);
            if (it != ResearchStageMap.end())
            {
                gResearchProgressStage = it->second;
            }
        }

        uint16_t progress_get() const
        {
            return gResearchProgress;
        }

        void progress_set(uint16_t value)
        {
            ThrowIfGameStateNotMutable();
            gResearchProgress = value;
        }

        DukValue expectedMonth_get() const
        {
            if (gResearchProgressStage == RESEARCH_STAGE_INITIAL_RESEARCH || gResearchExpectedDay == 255)
                return ToDuk(_context, nullptr);
            return ToDuk(_context, gResearchExpectedMonth);
        }

        DukValue expectedDay_get() const
        {
            if (gResearchProgressStage == RESEARCH_STAGE_INITIAL_RESEARCH || gResearchExpectedDay == 255)
                return ToDuk(_context, nullptr);
            return ToDuk(_context, gResearchExpectedDay);
        }

        DukValue lastResearchedItem_get() const
        {
            if (!gResearchLastItem)
                return ToDuk(_context, nullptr);
            return ToDuk(_context, *gResearchLastItem);
        }

        DukValue expectedItem_get() const
        {
            if (gResearchProgressStage == RESEARCH_STAGE_INITIAL_RESEARCH || !gResearchNextItem)
                return ToDuk(_context, nullptr);
            return ToDuk(_context, *gResearchNextItem);
        }

        std::vector<DukValue> inventedItems_get() const
        {
            std::vector<DukValue> result;
            for (auto& item : gResearchItemsInvented)
            {
                result.push_back(ToDuk(_context, item));
            }
            return result;
        }

        void inventedItems_set(const std::vector<DukValue>& value)
        {
            auto list = ConvertResearchList(value);
            gResearchItemsInvented = std::move(list);
            ResearchFix();
        }

        std::vector<DukValue> uninventedItems_get() const
        {
            std::vector<DukValue> result;
            for (auto& item : gResearchItemsUninvented)
            {
                result.push_back(ToDuk(_context, item));
            }
            return result;
        }

        void uninventedItems_set(const std::vector<DukValue>& value)
        {
            auto list = ConvertResearchList(value);
            gResearchItemsUninvented = std::move(list);
            ResearchFix();
        }

        bool isObjectResearched(const std::string& typez, ObjectEntryIndex index)
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

        static void Register(duk_context* ctx)
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
            dukglue_register_property(
                ctx, &ScResearch::uninventedItems_get, &ScResearch::uninventedItems_set, "uninventedItems");
            dukglue_register_method(ctx, &ScResearch::isObjectResearched, "isObjectResearched");
        }

        static std::vector<ResearchItem> ConvertResearchList(const std::vector<DukValue>& value)
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
    };

} // namespace OpenRCT2::Scripting

#endif
