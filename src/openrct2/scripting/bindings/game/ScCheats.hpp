/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../../../Cheats.h"
#    include "../../../GameState.h"
#    include "../../Duktape.hpp"
#    include "../../ScriptEngine.h"

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
            return GetGameState().Cheats.AllowArbitraryRideTypeChanges;
        }

        void allowArbitraryRideTypeChanges_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.AllowArbitraryRideTypeChanges = value;
        }

        bool allowTrackPlaceInvalidHeights_get()
        {
            return GetGameState().Cheats.AllowTrackPlaceInvalidHeights;
        }

        void allowTrackPlaceInvalidHeights_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.AllowTrackPlaceInvalidHeights = value;
        }

        bool buildInPauseMode_get()
        {
            return GetGameState().Cheats.BuildInPauseMode;
        }

        void buildInPauseMode_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.BuildInPauseMode = value;
        }

        bool disableAllBreakdowns_get()
        {
            return GetGameState().Cheats.DisableAllBreakdowns;
        }

        void disableAllBreakdowns_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.DisableAllBreakdowns = value;
        }

        bool disableBrakesFailure_get()
        {
            return GetGameState().Cheats.DisableBrakesFailure;
        }

        void disableBrakesFailure_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.DisableBrakesFailure = value;
        }

        bool disableClearanceChecks_get()
        {
            return GetGameState().Cheats.DisableClearanceChecks;
        }

        void disableClearanceChecks_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.DisableClearanceChecks = value;
        }

        bool disableLittering_get()
        {
            return GetGameState().Cheats.DisableLittering;
        }

        void disableLittering_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.DisableLittering = value;
        }

        bool disablePlantAging_get()
        {
            return GetGameState().Cheats.DisablePlantAging;
        }

        void disablePlantAging_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.DisablePlantAging = value;
        }

        bool allowRegularPathAsQueue_get()
        {
            return GetGameState().Cheats.AllowRegularPathAsQueue;
        }

        void allowRegularPathAsQueue_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.AllowRegularPathAsQueue = value;
        }

        bool allowSpecialColourSchemes_get()
        {
            return GetGameState().Cheats.AllowSpecialColourSchemes;
        }

        void allowSpecialColourSchemes_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.AllowSpecialColourSchemes = value;
        }

        bool disableRideValueAging_get()
        {
            return GetGameState().Cheats.DisableRideValueAging;
        }

        void disableRideValueAging_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.DisableRideValueAging = value;
        }

        bool disableSupportLimits_get()
        {
            return GetGameState().Cheats.DisableSupportLimits;
        }

        void disableSupportLimits_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.DisableSupportLimits = value;
        }

        bool disableTrainLengthLimit_get()
        {
            return GetGameState().Cheats.DisableTrainLengthLimit;
        }

        void disableTrainLengthLimit_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.DisableTrainLengthLimit = value;
        }

        bool disableVandalism_get()
        {
            return GetGameState().Cheats.DisableVandalism;
        }

        void disableVandalism_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.DisableVandalism = value;
        }

        bool enableAllDrawableTrackPieces_get()
        {
            return GetGameState().Cheats.EnableAllDrawableTrackPieces;
        }

        void enableAllDrawableTrackPieces_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.EnableAllDrawableTrackPieces = value;
        }

        bool enableChainLiftOnAllTrack_get()
        {
            return GetGameState().Cheats.EnableChainLiftOnAllTrack;
        }

        void enableChainLiftOnAllTrack_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.EnableChainLiftOnAllTrack = value;
        }

        bool fastLiftHill_get()
        {
            return GetGameState().Cheats.UnlockOperatingLimits;
        }

        void fastLiftHill_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.UnlockOperatingLimits = value;
        }

        bool freezeWeather_get()
        {
            return GetGameState().Cheats.FreezeWeather;
        }

        void freezeWeather_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.FreezeWeather = value;
        }

        bool ignoreResearchStatus_get()
        {
            return GetGameState().Cheats.IgnoreResearchStatus;
        }

        void ignoreResearchStatus_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.IgnoreResearchStatus = value;
        }

        bool ignoreRideIntensity_get()
        {
            return GetGameState().Cheats.IgnoreRideIntensity;
        }

        void ignoreRideIntensity_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.IgnoreRideIntensity = value;
        }

        bool neverendingMarketing_get()
        {
            return GetGameState().Cheats.NeverendingMarketing;
        }

        void neverendingMarketing_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.NeverendingMarketing = value;
        }

        bool sandboxMode_get()
        {
            return GetGameState().Cheats.SandboxMode;
        }

        void sandboxMode_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.SandboxMode = value;
        }

        bool showAllOperatingModes_get()
        {
            return GetGameState().Cheats.ShowAllOperatingModes;
        }

        void showAllOperatingModes_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.ShowAllOperatingModes = value;
        }

        bool showVehiclesFromOtherTrackTypes_get()
        {
            return GetGameState().Cheats.ShowVehiclesFromOtherTrackTypes;
        }

        void showVehiclesFromOtherTrackTypes_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            GetGameState().Cheats.ShowVehiclesFromOtherTrackTypes = value;
        }
    };
} // namespace OpenRCT2::Scripting

#endif
