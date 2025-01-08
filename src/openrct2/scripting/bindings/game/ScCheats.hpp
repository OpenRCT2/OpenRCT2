/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
            dukglue_register_property(
                ctx, &ScCheats::neverendingMarketing_get, &ScCheats::neverendingMarketing_set, "neverendingMarketing");
            dukglue_register_property(ctx, &ScCheats::sandboxMode_get, &ScCheats::sandboxMode_set, "sandboxMode");
            dukglue_register_property(
                ctx, &ScCheats::showAllOperatingModes_get, &ScCheats::showAllOperatingModes_set, "showAllOperatingModes");
            dukglue_register_property(
                ctx, &ScCheats::showVehiclesFromOtherTrackTypes_get, &ScCheats::showVehiclesFromOtherTrackTypes_set,
                "showVehiclesFromOtherTrackTypes");
        }

    private:
        bool allowArbitraryRideTypeChanges_get()
        {
            return GetGameState().Cheats.allowArbitraryRideTypeChanges;
        }

        void allowArbitraryRideTypeChanges_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.allowArbitraryRideTypeChanges = value;
        }

        bool allowTrackPlaceInvalidHeights_get()
        {
            return GetGameState().Cheats.allowTrackPlaceInvalidHeights;
        }

        void allowTrackPlaceInvalidHeights_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.allowTrackPlaceInvalidHeights = value;
        }

        bool buildInPauseMode_get()
        {
            return GetGameState().Cheats.buildInPauseMode;
        }

        void buildInPauseMode_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.buildInPauseMode = value;
        }

        bool disableAllBreakdowns_get()
        {
            return GetGameState().Cheats.disableAllBreakdowns;
        }

        void disableAllBreakdowns_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.disableAllBreakdowns = value;
        }

        bool disableBrakesFailure_get()
        {
            return GetGameState().Cheats.disableBrakesFailure;
        }

        void disableBrakesFailure_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.disableBrakesFailure = value;
        }

        bool disableClearanceChecks_get()
        {
            return GetGameState().Cheats.disableClearanceChecks;
        }

        void disableClearanceChecks_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.disableClearanceChecks = value;
        }

        bool disableLittering_get()
        {
            return GetGameState().Cheats.disableLittering;
        }

        void disableLittering_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.disableLittering = value;
        }

        bool disablePlantAging_get()
        {
            return GetGameState().Cheats.disablePlantAging;
        }

        void disablePlantAging_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.disablePlantAging = value;
        }

        bool allowRegularPathAsQueue_get()
        {
            return GetGameState().Cheats.allowRegularPathAsQueue;
        }

        void allowRegularPathAsQueue_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.allowRegularPathAsQueue = value;
        }

        bool allowSpecialColourSchemes_get()
        {
            return GetGameState().Cheats.allowSpecialColourSchemes;
        }

        void allowSpecialColourSchemes_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.allowSpecialColourSchemes = value;
        }

        bool disableRideValueAging_get()
        {
            return GetGameState().Cheats.disableRideValueAging;
        }

        void disableRideValueAging_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.disableRideValueAging = value;
        }

        bool disableSupportLimits_get()
        {
            return GetGameState().Cheats.disableSupportLimits;
        }

        void disableSupportLimits_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.disableSupportLimits = value;
        }

        bool disableTrainLengthLimit_get()
        {
            return GetGameState().Cheats.disableTrainLengthLimit;
        }

        void disableTrainLengthLimit_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.disableTrainLengthLimit = value;
        }

        bool disableVandalism_get()
        {
            return GetGameState().Cheats.disableVandalism;
        }

        void disableVandalism_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.disableVandalism = value;
        }

        bool enableAllDrawableTrackPieces_get()
        {
            return GetGameState().Cheats.enableAllDrawableTrackPieces;
        }

        void enableAllDrawableTrackPieces_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.enableAllDrawableTrackPieces = value;
        }

        bool enableChainLiftOnAllTrack_get()
        {
            return GetGameState().Cheats.enableChainLiftOnAllTrack;
        }

        void enableChainLiftOnAllTrack_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.enableChainLiftOnAllTrack = value;
        }

        bool fastLiftHill_get()
        {
            return GetGameState().Cheats.unlockOperatingLimits;
        }

        void fastLiftHill_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.unlockOperatingLimits = value;
        }

        bool freezeWeather_get()
        {
            return GetGameState().Cheats.freezeWeather;
        }

        void freezeWeather_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.freezeWeather = value;
        }

        bool ignoreResearchStatus_get()
        {
            return GetGameState().Cheats.ignoreResearchStatus;
        }

        void ignoreResearchStatus_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.ignoreResearchStatus = value;
        }

        bool ignoreRideIntensity_get()
        {
            return GetGameState().Cheats.ignoreRideIntensity;
        }

        void ignoreRideIntensity_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.ignoreRideIntensity = value;
        }

        bool neverendingMarketing_get()
        {
            return GetGameState().Cheats.neverendingMarketing;
        }

        void neverendingMarketing_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.neverendingMarketing = value;
        }

        bool sandboxMode_get()
        {
            return GetGameState().Cheats.sandboxMode;
        }

        void sandboxMode_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.sandboxMode = value;
        }

        bool showAllOperatingModes_get()
        {
            return GetGameState().Cheats.showAllOperatingModes;
        }

        void showAllOperatingModes_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.showAllOperatingModes = value;
        }

        bool showVehiclesFromOtherTrackTypes_get()
        {
            return GetGameState().Cheats.showVehiclesFromOtherTrackTypes;
        }

        void showVehiclesFromOtherTrackTypes_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.showVehiclesFromOtherTrackTypes = value;
        }
    };
} // namespace OpenRCT2::Scripting

#endif
