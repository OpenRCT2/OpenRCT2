/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../../../Cheats.h"
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
            return gCheatsAllowArbitraryRideTypeChanges;
        }

        void allowArbitraryRideTypeChanges_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            gCheatsAllowArbitraryRideTypeChanges = value;
        }

        bool allowTrackPlaceInvalidHeights_get()
        {
            return gCheatsAllowTrackPlaceInvalidHeights;
        }

        void allowTrackPlaceInvalidHeights_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            gCheatsAllowTrackPlaceInvalidHeights = value;
        }

        bool buildInPauseMode_get()
        {
            return gCheatsBuildInPauseMode;
        }

        void buildInPauseMode_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            gCheatsBuildInPauseMode = value;
        }

        bool disableAllBreakdowns_get()
        {
            return gCheatsDisableAllBreakdowns;
        }

        void disableAllBreakdowns_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            gCheatsDisableAllBreakdowns = value;
        }

        bool disableBrakesFailure_get()
        {
            return gCheatsDisableBrakesFailure;
        }

        void disableBrakesFailure_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            gCheatsDisableBrakesFailure = value;
        }

        bool disableClearanceChecks_get()
        {
            return gCheatsDisableClearanceChecks;
        }

        void disableClearanceChecks_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            gCheatsDisableClearanceChecks = value;
        }

        bool disableLittering_get()
        {
            return gCheatsDisableLittering;
        }

        void disableLittering_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            gCheatsDisableLittering = value;
        }

        bool disablePlantAging_get()
        {
            return gCheatsDisablePlantAging;
        }

        void disablePlantAging_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            gCheatsDisablePlantAging = value;
        }

        bool disableRideValueAging_get()
        {
            return gCheatsDisableRideValueAging;
        }

        void disableRideValueAging_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            gCheatsDisableRideValueAging = value;
        }

        bool disableSupportLimits_get()
        {
            return gCheatsDisableSupportLimits;
        }

        void disableSupportLimits_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            gCheatsDisableSupportLimits = value;
        }

        bool disableTrainLengthLimit_get()
        {
            return gCheatsDisableTrainLengthLimit;
        }

        void disableTrainLengthLimit_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            gCheatsDisableTrainLengthLimit = value;
        }

        bool disableVandalism_get()
        {
            return gCheatsDisableVandalism;
        }

        void disableVandalism_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            gCheatsDisableVandalism = value;
        }

        bool enableAllDrawableTrackPieces_get()
        {
            return gCheatsEnableAllDrawableTrackPieces;
        }

        void enableAllDrawableTrackPieces_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            gCheatsEnableAllDrawableTrackPieces = value;
        }

        bool enableChainLiftOnAllTrack_get()
        {
            return gCheatsEnableChainLiftOnAllTrack;
        }

        void enableChainLiftOnAllTrack_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            gCheatsEnableChainLiftOnAllTrack = value;
        }

        bool fastLiftHill_get()
        {
            return gCheatsUnlockOperatingLimits;
        }

        void fastLiftHill_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            gCheatsUnlockOperatingLimits = value;
        }

        bool freezeWeather_get()
        {
            return gCheatsFreezeWeather;
        }

        void freezeWeather_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            gCheatsFreezeWeather = value;
        }

        bool ignoreResearchStatus_get()
        {
            return gCheatsIgnoreResearchStatus;
        }

        void ignoreResearchStatus_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            gCheatsIgnoreResearchStatus = value;
        }

        bool ignoreRideIntensity_get()
        {
            return gCheatsIgnoreRideIntensity;
        }

        void ignoreRideIntensity_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            gCheatsIgnoreRideIntensity = value;
        }

        bool neverendingMarketing_get()
        {
            return gCheatsNeverendingMarketing;
        }

        void neverendingMarketing_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            gCheatsNeverendingMarketing = value;
        }

        bool sandboxMode_get()
        {
            return gCheatsSandboxMode;
        }

        void sandboxMode_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            gCheatsSandboxMode = value;
        }

        bool showAllOperatingModes_get()
        {
            return gCheatsShowAllOperatingModes;
        }

        void showAllOperatingModes_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            gCheatsShowAllOperatingModes = value;
        }

        bool showVehiclesFromOtherTrackTypes_get()
        {
            return gCheatsShowVehiclesFromOtherTrackTypes;
        }

        void showVehiclesFromOtherTrackTypes_set(bool value)
        {
            ThrowIfGameStateNotMutable();
            gCheatsShowVehiclesFromOtherTrackTypes = value;
        }
    };
} // namespace OpenRCT2::Scripting

#endif
