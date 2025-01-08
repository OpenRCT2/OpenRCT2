/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

class CheatSetAction final : public GameActionBase<GameCommand::Cheat>
{
    using ParametersRange = std::pair<std::pair<int64_t, int64_t>, std::pair<int64_t, int64_t>>;

private:
    NetworkCheatType_t _cheatType{ EnumValue(CheatType::Count) };
    int64_t _param1{};
    int64_t _param2{};

public:
    CheatSetAction() = default;
    CheatSetAction(CheatType cheatType, int64_t param1 = 0, int64_t param2 = 0);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    OpenRCT2::GameActions::Result Query() const override;
    OpenRCT2::GameActions::Result Execute() const override;

private:
    ParametersRange GetParameterRange(CheatType cheatType) const;
    void SetGrassLength(int32_t length) const;
    void WaterPlants() const;
    void FixVandalism() const;
    void RemoveLitter() const;
    void FixBrokenRides() const;
    void RenewRides() const;
    void ResetRideCrashStatus() const;
    void Set10MinuteInspection() const;
    void SetScenarioNoMoney(bool enabled) const;
    void SetMoney(money64 amount) const;
    void AddMoney(money64 amount) const;
    void ClearLoan() const;
    void GenerateGuests(int32_t count) const;
    void SetGuestParameter(int32_t parameter, int32_t value) const;
    void GiveObjectToGuests(int32_t object) const;
    void RemoveAllGuests() const;
    void SetStaffSpeed(uint8_t value) const;
    void OwnAllLand() const;
    void ParkSetOpen(bool isOpen) const;
    void CreateDucks(int count) const;
    void RemoveParkFences() const;
};
