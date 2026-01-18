/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include "../../../Cheats.h"
    #include "../../../GameState.h"
    #include "../../../world/Park.h"
    #include "../../ScriptEngine.h"

namespace OpenRCT2::Scripting
{
    class ScCheats;
    extern ScCheats gScCheats;

    class ScCheats final : public ScBase
    {
    public:
        JSValue New(JSContext* ctx)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF(
                    "allowArbitraryRideTypeChanges", &ScCheats::allowArbitraryRideTypeChanges_get,
                    &ScCheats::allowArbitraryRideTypeChanges_set),
                JS_CGETSET_DEF(
                    "allowTrackPlaceInvalidHeights", &ScCheats::allowTrackPlaceInvalidHeights_get,
                    &ScCheats::allowTrackPlaceInvalidHeights_set),
                JS_CGETSET_DEF("buildInPauseMode", &ScCheats::buildInPauseMode_get, &ScCheats::buildInPauseMode_set),
                JS_CGETSET_DEF(
                    "disableAllBreakdowns", &ScCheats::disableAllBreakdowns_get, &ScCheats::disableAllBreakdowns_set),
                JS_CGETSET_DEF(
                    "disableBrakesFailure", &ScCheats::disableBrakesFailure_get, &ScCheats::disableBrakesFailure_set),
                JS_CGETSET_DEF(
                    "disableClearanceChecks", &ScCheats::disableClearanceChecks_get, &ScCheats::disableClearanceChecks_set),
                JS_CGETSET_DEF("disableLittering", &ScCheats::disableLittering_get, &ScCheats::disableLittering_set),
                JS_CGETSET_DEF("disablePlantAging", &ScCheats::disablePlantAging_get, &ScCheats::disablePlantAging_set),
                JS_CGETSET_DEF(
                    "allowRegularPathAsQueue", &ScCheats::allowRegularPathAsQueue_get, &ScCheats::allowRegularPathAsQueue_set),
                JS_CGETSET_DEF(
                    "allowSpecialColourSchemes", &ScCheats::allowSpecialColourSchemes_get,
                    &ScCheats::allowSpecialColourSchemes_set),
                JS_CGETSET_DEF(
                    "disableRideValueAging", &ScCheats::disableRideValueAging_get, &ScCheats::disableRideValueAging_set),
                JS_CGETSET_DEF(
                    "disableSupportLimits", &ScCheats::disableSupportLimits_get, &ScCheats::disableSupportLimits_set),
                JS_CGETSET_DEF(
                    "disableTrainLengthLimit", &ScCheats::disableTrainLengthLimit_get, &ScCheats::disableTrainLengthLimit_set),
                JS_CGETSET_DEF("disableVandalism", &ScCheats::disableVandalism_get, &ScCheats::disableVandalism_set),
                JS_CGETSET_DEF(
                    "enableAllDrawableTrackPieces", &ScCheats::enableAllDrawableTrackPieces_get,
                    &ScCheats::enableAllDrawableTrackPieces_set),
                JS_CGETSET_DEF(
                    "enableChainLiftOnAllTrack", &ScCheats::enableChainLiftOnAllTrack_get,
                    &ScCheats::enableChainLiftOnAllTrack_set),
                JS_CGETSET_DEF("fastLiftHill", &ScCheats::fastLiftHill_get, &ScCheats::fastLiftHill_set),
                JS_CGETSET_DEF("freezeWeather", &ScCheats::freezeWeather_get, &ScCheats::freezeWeather_set),
                JS_CGETSET_DEF(
                    "ignoreResearchStatus", &ScCheats::ignoreResearchStatus_get, &ScCheats::ignoreResearchStatus_set),
                JS_CGETSET_DEF("ignoreRideIntensity", &ScCheats::ignoreRideIntensity_get, &ScCheats::ignoreRideIntensity_set),
                JS_CGETSET_DEF("ignoreRidePrice", &ScCheats::ignoreRidePrice_get, &ScCheats::ignoreRidePrice_set),
                JS_CGETSET_DEF(
                    "neverendingMarketing", &ScCheats::neverendingMarketing_get, &ScCheats::neverendingMarketing_set),
                JS_CGETSET_DEF("forcedParkRating", &ScCheats::forcedParkRating_get, &ScCheats::forcedParkRating_set),
                JS_CGETSET_DEF("sandboxMode", &ScCheats::sandboxMode_get, &ScCheats::sandboxMode_set),
                JS_CGETSET_DEF(
                    "showAllOperatingModes", &ScCheats::showAllOperatingModes_get, &ScCheats::showAllOperatingModes_set),
                JS_CGETSET_DEF(
                    "showVehiclesFromOtherTrackTypes", &ScCheats::showVehiclesFromOtherTrackTypes_get,
                    &ScCheats::showVehiclesFromOtherTrackTypes_set),
                JS_CGETSET_DEF("makeAllDestructible", &ScCheats::makeAllDestructible_get, &ScCheats::makeAllDestructible_set)
            };
            return MakeWithOpaque(ctx, funcs, nullptr);
        }

        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "Cheats");
        }

    private:
        static JSValue allowArbitraryRideTypeChanges_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, getGameState().cheats.allowArbitraryRideTypeChanges);
        }

        static JSValue allowArbitraryRideTypeChanges_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value)
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().cheats.allowArbitraryRideTypeChanges = valueBool;

            return JS_UNDEFINED;
        }

        static JSValue allowTrackPlaceInvalidHeights_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, getGameState().cheats.allowTrackPlaceInvalidHeights);
        }

        static JSValue allowTrackPlaceInvalidHeights_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value)
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().cheats.allowTrackPlaceInvalidHeights = valueBool;

            return JS_UNDEFINED;
        }

        static JSValue buildInPauseMode_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, getGameState().cheats.buildInPauseMode);
        }

        static JSValue buildInPauseMode_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value)
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().cheats.buildInPauseMode = valueBool;

            return JS_UNDEFINED;
        }

        static JSValue disableAllBreakdowns_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, getGameState().cheats.disableAllBreakdowns);
        }

        static JSValue disableAllBreakdowns_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value)
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().cheats.disableAllBreakdowns = valueBool;

            return JS_UNDEFINED;
        }

        static JSValue disableBrakesFailure_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, getGameState().cheats.disableBrakesFailure);
        }

        static JSValue disableBrakesFailure_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value)
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().cheats.disableBrakesFailure = valueBool;

            return JS_UNDEFINED;
        }

        static JSValue disableClearanceChecks_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, getGameState().cheats.disableClearanceChecks);
        }

        static JSValue disableClearanceChecks_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value)
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().cheats.disableClearanceChecks = valueBool;

            return JS_UNDEFINED;
        }

        static JSValue disableLittering_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, getGameState().cheats.disableLittering);
        }

        static JSValue disableLittering_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().cheats.disableLittering = valueBool;

            return JS_UNDEFINED;
        }

        static JSValue disablePlantAging_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, getGameState().cheats.disablePlantAging);
        }

        static JSValue disablePlantAging_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().cheats.disablePlantAging = valueBool;

            return JS_UNDEFINED;
        }

        static JSValue allowRegularPathAsQueue_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, getGameState().cheats.allowRegularPathAsQueue);
        }

        static JSValue allowRegularPathAsQueue_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().cheats.allowRegularPathAsQueue = valueBool;

            return JS_UNDEFINED;
        }

        static JSValue allowSpecialColourSchemes_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, getGameState().cheats.allowSpecialColourSchemes);
        }

        static JSValue allowSpecialColourSchemes_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().cheats.allowSpecialColourSchemes = valueBool;

            return JS_UNDEFINED;
        }

        static JSValue disableRideValueAging_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, getGameState().cheats.disableRideValueAging);
        }

        static JSValue disableRideValueAging_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().cheats.disableRideValueAging = valueBool;

            return JS_UNDEFINED;
        }

        static JSValue disableSupportLimits_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, getGameState().cheats.disableSupportLimits);
        }

        static JSValue disableSupportLimits_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().cheats.disableSupportLimits = valueBool;

            return JS_UNDEFINED;
        }

        static JSValue disableTrainLengthLimit_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, getGameState().cheats.disableTrainLengthLimit);
        }

        static JSValue disableTrainLengthLimit_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().cheats.disableTrainLengthLimit = valueBool;

            return JS_UNDEFINED;
        }

        static JSValue disableVandalism_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, getGameState().cheats.disableVandalism);
        }

        static JSValue disableVandalism_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().cheats.disableVandalism = valueBool;

            return JS_UNDEFINED;
        }

        static JSValue enableAllDrawableTrackPieces_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, getGameState().cheats.enableAllDrawableTrackPieces);
        }

        static JSValue enableAllDrawableTrackPieces_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().cheats.enableAllDrawableTrackPieces = valueBool;

            return JS_UNDEFINED;
        }

        static JSValue enableChainLiftOnAllTrack_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, getGameState().cheats.enableChainLiftOnAllTrack);
        }

        static JSValue enableChainLiftOnAllTrack_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().cheats.enableChainLiftOnAllTrack = valueBool;

            return JS_UNDEFINED;
        }

        static JSValue fastLiftHill_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, getGameState().cheats.unlockOperatingLimits);
        }

        static JSValue fastLiftHill_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().cheats.unlockOperatingLimits = valueBool;

            return JS_UNDEFINED;
        }

        static JSValue freezeWeather_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, getGameState().cheats.freezeWeather);
        }

        static JSValue freezeWeather_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().cheats.freezeWeather = valueBool;

            return JS_UNDEFINED;
        }

        static JSValue ignoreResearchStatus_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, getGameState().cheats.ignoreResearchStatus);
        }

        static JSValue ignoreResearchStatus_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value)
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().cheats.ignoreResearchStatus = valueBool;

            return JS_UNDEFINED;
        }

        static JSValue ignoreRideIntensity_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, getGameState().cheats.ignoreRideIntensity);
        }

        static JSValue ignoreRideIntensity_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value)
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().cheats.ignoreRideIntensity = valueBool;

            return JS_UNDEFINED;
        }

        static JSValue ignoreRidePrice_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, getGameState().cheats.ignorePrice);
        }

        static JSValue ignoreRidePrice_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().cheats.ignorePrice = valueBool;

            return JS_UNDEFINED;
        }

        static JSValue neverendingMarketing_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, getGameState().cheats.neverendingMarketing);
        }

        static JSValue neverendingMarketing_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().cheats.neverendingMarketing = valueBool;

            return JS_UNDEFINED;
        }

        static JSValue forcedParkRating_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewInt32(ctx, getGameState().cheats.forcedParkRating);
        }

        static JSValue forcedParkRating_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_INT32(valueInt, ctx, value);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            int32_t adjusted = std::max(-1, std::min(valueInt, 999));
            getGameState().cheats.forcedParkRating = adjusted;
            Park::SetForcedRating(adjusted);

            return JS_UNDEFINED;
        }

        static JSValue sandboxMode_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, getGameState().cheats.sandboxMode);
        }

        static JSValue sandboxMode_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().cheats.sandboxMode = valueBool;

            return JS_UNDEFINED;
        }

        static JSValue showAllOperatingModes_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, getGameState().cheats.showAllOperatingModes);
        }

        static JSValue showAllOperatingModes_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().cheats.showAllOperatingModes = valueBool;

            return JS_UNDEFINED;
        }

        static JSValue showVehiclesFromOtherTrackTypes_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, getGameState().cheats.showVehiclesFromOtherTrackTypes);
        }

        static JSValue showVehiclesFromOtherTrackTypes_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().cheats.showVehiclesFromOtherTrackTypes = valueBool;

            return JS_UNDEFINED;
        }

        static JSValue makeAllDestructible_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, getGameState().cheats.makeAllDestructible);
        }

        static JSValue makeAllDestructible_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_BOOL(valueBool, ctx, value);
            JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
            getGameState().cheats.makeAllDestructible = valueBool;

            return JS_UNDEFINED;
        }
    };
} // namespace OpenRCT2::Scripting

#endif
