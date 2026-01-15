/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

namespace OpenRCT2::GameActions
{
    class CheatSetAction final : public GameActionBase<GameCommand::Cheat>
    {
        using ParametersRange = std::pair<std::pair<int64_t, int64_t>, std::pair<int64_t, int64_t>>;

    private:
        Network::CheatType_t _cheatType{ EnumValue(CheatType::count) };
        int64_t _param1{};
        int64_t _param2{};

    public:
        CheatSetAction() = default;
        CheatSetAction(CheatType cheatType, int64_t param1 = 0, int64_t param2 = 0);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;

    private:
        ParametersRange GetParameterRange(CheatType cheatType) const;
        void SetGrassLength(GameState_t& gameState, int32_t length) const;
        void WaterPlants() const;
        void FixVandalism() const;
        void RemoveLitter(GameState_t& gameState) const;
        void FixBrokenRides(GameState_t& gameState) const;
        void RenewRides(GameState_t& gameState) const;
        void ResetRideCrashStatus(GameState_t& gameState) const;
        void Set10MinuteInspection(GameState_t& gameState) const;
        void SetScenarioNoMoney(GameState_t& gameState, bool enabled) const;
        void SetMoney(GameState_t& gameState, money64 amount) const;
        void AddMoney(GameState_t& gameState, money64 amount) const;
        void ClearLoan(GameState_t& gameState) const;
        void GenerateGuests(int32_t count) const;
        void SetGuestParameter(int32_t parameter, int32_t value) const;
        void GiveObjectToGuests(int32_t object) const;
        void RemoveAllGuests(GameState_t& gameState) const;
        void SetStaffSpeed(uint8_t value) const;
        void OwnAllLand(GameState_t& gameState) const;
        void ParkSetOpen(bool isOpen, GameState_t& gameState) const;
        void CreateDucks(int count) const;
        void RemoveParkFences() const;
    };
} // namespace OpenRCT2::GameActions
