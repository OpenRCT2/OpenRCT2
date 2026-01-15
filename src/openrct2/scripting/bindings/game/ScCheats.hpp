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
    #include "../../Duktape.hpp"
    #include "../../ScriptEngine.h"

namespace OpenRCT2::Scripting
{
    class ScCheats
    {
    public:
        static void Register(duk_context* ctx)
        {
            dukglue_register_property(
                ctx, &ScCheats::allowArbitraryRideTypeChanges_get, &ScCheats::allowArbitraryRideTypeChanges_set,
                "allowArbitraryRideTypeChanges");
            dukglue_register_property(
                ctx, &ScCheats::allowTrackPlaceInvalidHeights_get, &ScCheats::allowTrackPlaceInvalidHeights_set,
                "allowTrackPlaceInvalidHeights");
            dukglue_register_property(
                ctx, &ScCheats::buildInPauseMode_get, &ScCheats::buildInPauseMode_set, "buildInPauseMode");
            dukglue_register_property(
                ctx, &ScCheats::disableAllBreakdowns_get, &ScCheats::disableAllBreakdowns_set, "disableAllBreakdowns");
            dukglue_register_property(
                ctx, &ScCheats::disableBrakesFailure_get, &ScCheats::disableBrakesFailure_set, "disableBrakesFailure");
            dukglue_register_property(
                ctx, &ScCheats::disableClearanceChecks_get, &ScCheats::disableClearanceChecks_set, "disableClearanceChecks");
            dukglue_register_property(
                ctx, &ScCheats::disableLittering_get, &ScCheats::disableLittering_set, "disableLittering");
            dukglue_register_property(
                ctx, &ScCheats::disablePlantAging_get, &ScCheats::disablePlantAging_set, "disablePlantAging");
            dukglue_register_property(
                ctx, &ScCheats::allowRegularPathAsQueue_get, &ScCheats::allowRegularPathAsQueue_set, "allowRegularPathAsQueue");
            dukglue_register_property(
                ctx, &ScCheats::allowSpecialColourSchemes_get, &ScCheats::allowSpecialColourSchemes_set,
                "allowSpecialColourSchemes");
            dukglue_register_property(
                ctx, &ScCheats::disableRideValueAging_get, &ScCheats::disableRideValueAging_set, "disableRideValueAging");
            dukglue_register_property(
                ctx, &ScCheats::disableSupportLimits_get, &ScCheats::disableSupportLimits_set, "disableSupportLimits");
            dukglue_register_property(
                ctx, &ScCheats::disableTrainLengthLimit_get, &ScCheats::disableTrainLengthLimit_set, "disableTrainLengthLimit");
            dukglue_register_property(
                ctx, &ScCheats::disableVandalism_get, &ScCheats::disableVandalism_set, "disableVandalism");
            dukglue_register_property(
                ctx, &ScCheats::enableAllDrawableTrackPieces_get, &ScCheats::enableAllDrawableTrackPieces_set,
                "enableAllDrawableTrackPieces");
            dukglue_register_property(
                ctx, &ScCheats::enableChainLiftOnAllTrack_get, &ScCheats::enableChainLiftOnAllTrack_set,
                "enableChainLiftOnAllTrack");
            dukglue_register_property(ctx, &ScCheats::fastLiftHill_get, &ScCheats::fastLiftHill_set, "fastLiftHill");
            dukglue_register_property(ctx, &ScCheats::freezeWeather_get, &ScCheats::freezeWeather_set, "freezeWeather");
            dukglue_register_property(
                ctx, &ScCheats::ignoreResearchStatus_get, &ScCheats::ignoreResearchStatus_set, "ignoreResearchStatus");
            dukglue_register_property(
                ctx, &ScCheats::ignoreRideIntensity_get, &ScCheats::ignoreRideIntensity_set, "ignoreRideIntensity");
            dukglue_register_property(ctx, &ScCheats::ignoreRidePrice_get, &ScCheats::ignoreRidePrice_set, "ignoreRidePrice");
            dukglue_register_property(
                ctx, &ScCheats::neverendingMarketing_get, &ScCheats::neverendingMarketing_set, "neverendingMarketing");
            dukglue_register_property(
                ctx, &ScCheats::forcedParkRating_get, &ScCheats::forcedParkRating_set, "forcedParkRating");
            dukglue_register_property(ctx, &ScCheats::sandboxMode_get, &ScCheats::sandboxMode_set, "sandboxMode");
            dukglue_register_property(
                ctx, &ScCheats::showAllOperatingModes_get, &ScCheats::showAllOperatingModes_set, "showAllOperatingModes");
            dukglue_register_property(
                ctx, &ScCheats::showVehiclesFromOtherTrackTypes_get, &ScCheats::showVehiclesFromOtherTrackTypes_set,
                "showVehiclesFromOtherTrackTypes");
            dukglue_register_property(
                ctx, &ScCheats::makeAllDestructible_get, &ScCheats::makeAllDestructible_set, "makeAllDestructible");
        }

    private:
        bool allowArbitraryRideTypeChanges_get()
        {
            return getGameState().cheats.allowArbitraryRideTypeChanges;
        }

        void allowArbitraryRideTypeChanges_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().cheats.allowArbitraryRideTypeChanges = value;
        }

        bool allowTrackPlaceInvalidHeights_get()
        {
            return getGameState().cheats.allowTrackPlaceInvalidHeights;
        }

        void allowTrackPlaceInvalidHeights_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().cheats.allowTrackPlaceInvalidHeights = value;
        }

        bool buildInPauseMode_get()
        {
            return getGameState().cheats.buildInPauseMode;
        }

        void buildInPauseMode_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().cheats.buildInPauseMode = value;
        }

        bool disableAllBreakdowns_get()
        {
            return getGameState().cheats.disableAllBreakdowns;
        }

        void disableAllBreakdowns_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().cheats.disableAllBreakdowns = value;
        }

        bool disableBrakesFailure_get()
        {
            return getGameState().cheats.disableBrakesFailure;
        }

        void disableBrakesFailure_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().cheats.disableBrakesFailure = value;
        }

        bool disableClearanceChecks_get()
        {
            return getGameState().cheats.disableClearanceChecks;
        }

        void disableClearanceChecks_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().cheats.disableClearanceChecks = value;
        }

        bool disableLittering_get()
        {
            return getGameState().cheats.disableLittering;
        }

        void disableLittering_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().cheats.disableLittering = value;
        }

        bool disablePlantAging_get()
        {
            return getGameState().cheats.disablePlantAging;
        }

        void disablePlantAging_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().cheats.disablePlantAging = value;
        }

        bool allowRegularPathAsQueue_get()
        {
            return getGameState().cheats.allowRegularPathAsQueue;
        }

        void allowRegularPathAsQueue_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().cheats.allowRegularPathAsQueue = value;
        }

        bool allowSpecialColourSchemes_get()
        {
            return getGameState().cheats.allowSpecialColourSchemes;
        }

        void allowSpecialColourSchemes_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().cheats.allowSpecialColourSchemes = value;
        }

        bool disableRideValueAging_get()
        {
            return getGameState().cheats.disableRideValueAging;
        }

        void disableRideValueAging_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().cheats.disableRideValueAging = value;
        }

        bool disableSupportLimits_get()
        {
            return getGameState().cheats.disableSupportLimits;
        }

        void disableSupportLimits_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().cheats.disableSupportLimits = value;
        }

        bool disableTrainLengthLimit_get()
        {
            return getGameState().cheats.disableTrainLengthLimit;
        }

        void disableTrainLengthLimit_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().cheats.disableTrainLengthLimit = value;
        }

        bool disableVandalism_get()
        {
            return getGameState().cheats.disableVandalism;
        }

        void disableVandalism_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().cheats.disableVandalism = value;
        }

        bool enableAllDrawableTrackPieces_get()
        {
            return getGameState().cheats.enableAllDrawableTrackPieces;
        }

        void enableAllDrawableTrackPieces_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().cheats.enableAllDrawableTrackPieces = value;
        }

        bool enableChainLiftOnAllTrack_get()
        {
            return getGameState().cheats.enableChainLiftOnAllTrack;
        }

        void enableChainLiftOnAllTrack_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().cheats.enableChainLiftOnAllTrack = value;
        }

        bool fastLiftHill_get()
        {
            return getGameState().cheats.unlockOperatingLimits;
        }

        void fastLiftHill_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().cheats.unlockOperatingLimits = value;
        }

        bool freezeWeather_get()
        {
            return getGameState().cheats.freezeWeather;
        }

        void freezeWeather_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().cheats.freezeWeather = value;
        }

        bool ignoreResearchStatus_get()
        {
            return getGameState().cheats.ignoreResearchStatus;
        }

        void ignoreResearchStatus_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().cheats.ignoreResearchStatus = value;
        }

        bool ignoreRideIntensity_get()
        {
            return getGameState().cheats.ignoreRideIntensity;
        }

        void ignoreRideIntensity_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().cheats.ignoreRideIntensity = value;
        }

        bool ignoreRidePrice_get()
        {
            return getGameState().cheats.ignorePrice;
        }

        void ignoreRidePrice_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().cheats.ignorePrice = value;
        }

        bool neverendingMarketing_get()
        {
            return getGameState().cheats.neverendingMarketing;
        }

        void neverendingMarketing_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().cheats.neverendingMarketing = value;
        }

        int32_t forcedParkRating_get()
        {
            return getGameState().cheats.forcedParkRating;
        }

        void forcedParkRating_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            int32_t adjusted = std::max(-1, std::min(value, 999));
            getGameState().cheats.forcedParkRating = adjusted;
            Park::SetForcedRating(adjusted);
        }

        bool sandboxMode_get()
        {
            return getGameState().cheats.sandboxMode;
        }

        void sandboxMode_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().cheats.sandboxMode = value;
        }

        bool showAllOperatingModes_get()
        {
            return getGameState().cheats.showAllOperatingModes;
        }

        void showAllOperatingModes_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().cheats.showAllOperatingModes = value;
        }

        bool showVehiclesFromOtherTrackTypes_get()
        {
            return getGameState().cheats.showVehiclesFromOtherTrackTypes;
        }

        void showVehiclesFromOtherTrackTypes_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().cheats.showVehiclesFromOtherTrackTypes = value;
        }

        bool makeAllDestructible_get()
        {
            return getGameState().cheats.makeAllDestructible;
        }

        void makeAllDestructible_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            getGameState().cheats.makeAllDestructible = value;
        }
    };
} // namespace OpenRCT2::Scripting

#endif
