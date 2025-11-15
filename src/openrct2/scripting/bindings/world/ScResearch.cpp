/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "ScResearch.hpp"

    #include "../../../Context.h"
    #include "../../../GameState.h"
    #include "../../../core/EnumMap.hpp"
    #include "../../../core/String.hpp"
    #include "../../../management/Research.h"
    #include "../../../ride/RideData.h"
    #include "../../ScriptEngine.h"
    #include "../object/ScObject.hpp"

using namespace OpenRCT2;

namespace OpenRCT2::Scripting
{
    static const EnumMap<uint8_t> ResearchStageMap(
        {
            { "initial_research", RESEARCH_STAGE_INITIAL_RESEARCH },
            { "designing", RESEARCH_STAGE_DESIGNING },
            { "completing_design", RESEARCH_STAGE_COMPLETING_DESIGN },
            { "unknown", RESEARCH_STAGE_UNKNOWN },
            { "finished_all", RESEARCH_STAGE_FINISHED_ALL },
        });

    static const EnumMap<ResearchCategory> ResearchCategoryMap(
        {
            { "transport", ResearchCategory::transport },
            { "gentle", ResearchCategory::gentle },
            { "rollercoaster", ResearchCategory::rollercoaster },
            { "thrill", ResearchCategory::thrill },
            { "water", ResearchCategory::water },
            { "shop", ResearchCategory::shop },
            { "scenery", ResearchCategory::sceneryGroup },
        });

    static const EnumMap<Research::EntryType> ResearchEntryTypeMap(
        {
            { "ride", Research::EntryType::ride },
            { "scenery", Research::EntryType::scenery },
        });

    static inline JSValue ResearchItemToJSValue(JSContext* ctx, const ResearchItem& value)
    {
        JSValue obj = JS_NewObject(ctx);
        JS_SetPropertyStr(ctx, obj, "category", JSFromStdString(ctx, ResearchCategoryMap[value.category]));
        JS_SetPropertyStr(ctx, obj, "type", JSFromStdString(ctx, ResearchEntryTypeMap[value.type]));
        if (value.type == Research::EntryType::ride)
        {
            JS_SetPropertyStr(ctx, obj, "rideType", JS_NewUint32(ctx, value.baseRideType));
        }
        JS_SetPropertyStr(ctx, obj, "object", JS_NewUint32(ctx, value.entryIndex));
        return obj;
    }

    static inline Research::EntryType GetResearchType(JSContext* ctx, JSValue value)
    {
        auto type = JSToOptionalStdString(ctx, value, "type");
        if (type.has_value())
        {
            auto it = ResearchEntryTypeMap.find(type.value());
            if (it != ResearchEntryTypeMap.end())
            {
                return it->second;
            }
        }
        return Research::EntryType::scenery;
    }

    static inline ResearchItem GetResearchItem(JSContext* ctx, JSValue value)
    {
        ResearchItem result;
        result.baseRideType = 0;
        result.category = {}; // We ignore category because it will be derived from ride type
        result.flags = 0;
        result.type = GetResearchType(ctx, value);
        result.entryIndex = JSToUint(ctx, value, "object");

        auto rideType = JSToOptionalUint(ctx, value, "rideType");
        if (rideType.has_value())
        {
            result.baseRideType = static_cast<uint8_t>(rideType.value());
        }
        return result;
    }

    JSValue ScResearch::funding_get(JSContext* ctx, JSValue thisVal)
    {
        return JS_NewUint32(ctx, getGameState().researchFundingLevel);
    }

    JSValue ScResearch::funding_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        JS_UNPACK_UINT32(val, ctx, value);
        getGameState().researchFundingLevel = std::clamp<uint8_t>(
            static_cast<uint8_t>(val), RESEARCH_FUNDING_NONE, RESEARCH_FUNDING_MAXIMUM);
        return JS_UNDEFINED;
    }

    JSValue ScResearch::priorities_get(JSContext* ctx, JSValue thisVal)
    {
        JSValue result = JS_NewArray(ctx);
        int64_t index = 0;
        for (auto i = EnumValue(ResearchCategory::transport); i <= EnumValue(ResearchCategory::sceneryGroup); i++)
        {
            auto category = static_cast<ResearchCategory>(i);
            if (getGameState().researchPriorities & EnumToFlag(category))
            {
                JS_SetPropertyInt64(ctx, result, index++, JSFromStdString(ctx, ResearchCategoryMap[category]));
            }
        }
        return result;
    }

    JSValue ScResearch::priorities_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        JS_UNPACK_ARRAY(values, ctx, value);

        auto priorities = 0;
        JSIterateArray(ctx, values, [&priorities](JSContext* ctx2, JSValue val) {
            auto item = JSToStdString(ctx2, val);
            auto category = ResearchCategoryMap.TryGet(item);
            if (category)
            {
                priorities |= EnumToFlag(*category);
            }
        });

        getGameState().researchPriorities = priorities;
        return JS_UNDEFINED;
    }

    JSValue ScResearch::stage_get(JSContext* ctx, JSValue thisVal)
    {
        return JSFromStdString(ctx, ResearchStageMap[getGameState().researchProgressStage]);
    }

    JSValue ScResearch::stage_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        JS_UNPACK_STR(stage, ctx, value);
        auto it = ResearchStageMap.find(stage);
        if (it != ResearchStageMap.end())
        {
            getGameState().researchProgressStage = it->second;
        }
        return JS_UNDEFINED;
    }

    JSValue ScResearch::progress_get(JSContext* ctx, JSValue thisVal)
    {
        return JS_NewUint32(ctx, getGameState().researchProgress);
    }

    JSValue ScResearch::progress_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        JS_UNPACK_UINT32(progress, ctx, value);
        getGameState().researchProgress = progress;
        return JS_UNDEFINED;
    }

    JSValue ScResearch::expectedMonth_get(JSContext* ctx, JSValue thisVal)
    {
        const auto& gameState = getGameState();
        if (gameState.researchProgressStage == RESEARCH_STAGE_INITIAL_RESEARCH || gameState.researchExpectedDay == 255)
            return JS_NULL;
        return JS_NewUint32(ctx, gameState.researchExpectedMonth);
    }

    JSValue ScResearch::expectedDay_get(JSContext* ctx, JSValue thisVal)
    {
        const auto& gameState = getGameState();
        if (gameState.researchProgressStage == RESEARCH_STAGE_INITIAL_RESEARCH || gameState.researchExpectedDay == 255)
            return JS_NULL;
        return JS_NewUint32(ctx, gameState.researchExpectedDay + 1);
    }

    JSValue ScResearch::lastResearchedItem_get(JSContext* ctx, JSValue thisVal)
    {
        const auto& gameState = getGameState();
        if (!gameState.researchLastItem)
            return JS_NULL;
        return ResearchItemToJSValue(ctx, *gameState.researchLastItem);
    }

    JSValue ScResearch::expectedItem_get(JSContext* ctx, JSValue thisVal)
    {
        const auto& gameState = getGameState();
        if (gameState.researchProgressStage == RESEARCH_STAGE_INITIAL_RESEARCH || !gameState.researchNextItem)
            return JS_NULL;
        return ResearchItemToJSValue(ctx, *gameState.researchNextItem);
    }

    JSValue ScResearch::inventedItems_get(JSContext* ctx, JSValue thisVal)
    {
        JSValue result = JS_NewArray(ctx);
        int64_t index = 0;
        for (auto& item : getGameState().researchItemsInvented)
        {
            JS_SetPropertyInt64(ctx, result, index++, ResearchItemToJSValue(ctx, item));
        }
        return result;
    }

    JSValue ScResearch::inventedItems_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        JS_UNPACK_ARRAY(array, ctx, value);
        auto list = ConvertResearchList(ctx, array);
        getGameState().researchItemsInvented = std::move(list);
        ResearchFix();
        return JS_UNDEFINED;
    }

    JSValue ScResearch::uninventedItems_get(JSContext* ctx, JSValue thisVal)
    {
        JSValue result = JS_NewArray(ctx);
        int64_t index = 0;
        for (auto& item : getGameState().researchItemsUninvented)
        {
            JS_SetPropertyInt64(ctx, result, index++, ResearchItemToJSValue(ctx, item));
        }
        return result;
    }

    JSValue ScResearch::uninventedItems_set(JSContext* ctx, JSValue thisVal, JSValue value)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        JS_UNPACK_ARRAY(array, ctx, value);
        auto list = ConvertResearchList(ctx, array);
        getGameState().researchItemsUninvented = std::move(list);
        ResearchFix();
        return JS_UNDEFINED;
    }

    JSValue ScResearch::isObjectResearched(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_STR(typez, ctx, argv[0]);
        JS_UNPACK_UINT32(index, ctx, argv[1]);

        auto type = objectTypeFromString(typez);
        if (type == ObjectType::none)
        {
            return JS_ThrowPlainError(ctx, "Invalid object type.");
        }

        auto result = ResearchIsInvented(type, static_cast<ObjectEntryIndex>(index));
        return JS_NewBool(ctx, result);
    }

    void ScResearch::Register(JSContext* ctx)
    {
        RegisterBaseStr(ctx, "Research");
    }

    JSValue ScResearch::New(JSContext* ctx)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("funding", ScResearch::funding_get, ScResearch::funding_set),
            JS_CGETSET_DEF("priorities", ScResearch::priorities_get, ScResearch::priorities_set),
            JS_CGETSET_DEF("stage", ScResearch::stage_get, ScResearch::stage_set),
            JS_CGETSET_DEF("progress", ScResearch::progress_get, ScResearch::progress_set),
            JS_CGETSET_DEF("expectedMonth", ScResearch::expectedMonth_get, nullptr),
            JS_CGETSET_DEF("expectedDay", ScResearch::expectedDay_get, nullptr),
            JS_CGETSET_DEF("lastResearchedItem", ScResearch::lastResearchedItem_get, nullptr),
            JS_CGETSET_DEF("expectedItem", ScResearch::expectedItem_get, nullptr),
            JS_CGETSET_DEF("inventedItems", ScResearch::inventedItems_get, ScResearch::inventedItems_set),
            JS_CGETSET_DEF("uninventedItems", ScResearch::uninventedItems_get, ScResearch::uninventedItems_set),
            JS_CFUNC_DEF("isObjectResearched", 2, ScResearch::isObjectResearched),
        };
        return MakeWithOpaque(ctx, funcs, nullptr);
    }

    std::vector<ResearchItem> ScResearch::ConvertResearchList(JSContext* ctx, JSValue value)
    {
        auto& objManager = GetContext()->GetObjectManager();
        std::vector<ResearchItem> result;
        JSIterateArray(ctx, value, [&result, &objManager](JSContext* ctx2, JSValue val) {
            auto researchItem = GetResearchItem(ctx2, val);
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
        });
        return result;
    }
} // namespace OpenRCT2::Scripting

#endif
