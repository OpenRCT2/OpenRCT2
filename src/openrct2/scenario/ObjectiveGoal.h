/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../entity/EntityList.h"
#include "../localisation/Date.h"
#include "../localisation/Localisation.h"
#include "../management/Award.h"
#include "../management/Finance.h"
#include "../management/Research.h"
#include "../object/Object.h"
#include "../ride/Ride.h"
#include "../ride/RideRatings.h"

enum class GoalType : uint8_t
{
    Goal,
    Restriction,
    Hybrid,
    OptionalGoal
};

enum class GoalID : uint8_t
{
    DummyGoal = 0,
    GuestNumGoal = 1,
    ParkValueGoal = 2,
    ParkRatingGoal = 3,
    ProfitGoal = 5,
    RepayLoanGoal = 11,
    ResearchCertainRide = 17,
    SpecificRideGoal = 18,
    FunGoal = 28,
    NumRidesGoal = 29,

};

enum class RideRequirement : uint8_t
{
    ExcitementRating,
    IntensityRating,
    NauseaRating,
    Length,
    AverageSpeed,
    HighestSpeed,
    RideTime,
    HighestDrop,
    NumberOfDrops,
    NumberOfInversions,
    AirTime,
    MaxPositiveVGs,
    MaxNegativeVgs,
    MaxLatGs,
    CustomersOnRide,
    CustomersPerHour,
    QueueTime,
    Holes

};

enum class ProfitTypeFlags : uint8_t
{
    RideTickets = 1 << 0,
    ParkEntry = 1 << 1,
    Food = 1 << 2,
    Merch = 1 << 3,
};

#ifdef __WARN_SUGGEST_FINAL_METHODS__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wsuggest-final-methods"
#    pragma GCC diagnostic ignored "-Wsuggest-final-types"
#endif

struct RideStatRequirement
{
    uint32_t minValue;
    uint32_t maxValue;
    RideRequirement requirement;

    RideStatRequirement(uint32_t _min, uint32_t _max, RideRequirement _req)
        : minValue(_min)
        , maxValue(_max)
        , requirement(_req){};

    bool Check(const Ride& ride);
};

class ObjectiveGoal;
using ObjectiveGoalPtr = std::shared_ptr<ObjectiveGoal>;
class ObjectiveGoal
{
protected:
    GoalID goalID;
    GoalType goalType;
    uint16_t warningDaysIndex, warningWeeksPeriod;
    bool trueOnLastCheck;
    bool countingDown = false;
    bool usesMoney;           // Doesn't need to be saved into savegames
    std::string longText;     // Doesn't need to be saved
    bool redoToString = true; // Doesn't need to be saved

public:
    ObjectiveGoal(
        GoalID _id, GoalType _type, bool _usesMoney = false, uint16_t _warningDayIndex = 0, uint16_t _warningWeekPeriod = 0);

    virtual bool CheckCondition() = 0;
    virtual bool CheckSpecialRequirements(rct_string_id& error) const;
    virtual bool CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const;
    virtual std::string ToString()
    {
        return std::string();
    }
    rct_string_id ToStringShort() const
    {
        return STR_NONE;
    }

    GoalID GetGoalID() const
    {
        return goalID;
    }
    GoalType GetGoalType() const
    {
        return goalType;
    }
    void SetGoalType(GoalType _goalType); // in cpp
    uint16_t GetWarningDaysIndex() const
    {
        return warningDaysIndex;
    }
    void SetWarningDaysIndex(uint16_t _warningDaysIndex)
    {
        warningDaysIndex = _warningDaysIndex;
    }
    uint16_t GetWarningWeeksPeriod() const
    {
        return warningWeeksPeriod;
    }
    void SetWarningWeeksPeriod(uint16_t _leewayPeriod)
    {
        warningWeeksPeriod = _leewayPeriod;
        redoToString = true;
    }
    bool GetTrueOnLastCheck() const
    {
        return trueOnLastCheck;
    }
    void SetTrueOnLastCheck(bool _trueOnLastCheck)
    {
        trueOnLastCheck = _trueOnLastCheck;
        redoToString = true;
    }
    bool GetCountingDown() const
    {
        return countingDown;
    }
    void SetCountingDown(bool _countingDown)
    {
        countingDown = _countingDown;
    }
    bool GetUsesMoney() const
    {
        return usesMoney;
    }
    virtual ~ObjectiveGoal() = default;
};

/// <summary>
/// Placeholder to hold data of future-added goals.
/// </summary>
class ObjectiveDummyGoal final : public ObjectiveGoal
{
public:
    uint32_t actualId;
    std::vector<uint32_t> contents;

    ObjectiveDummyGoal(uint32_t _actualId, std::vector<uint32_t> _contents)
        : ObjectiveGoal(GoalID::DummyGoal, GoalType::Goal)
        , actualId(_actualId)
        , contents(_contents){};
    bool CheckCondition() override
    {
        return false;
    };
};

// This is an abstract goal.
class ObjectiveValueGoal : public ObjectiveGoal
{
protected:
    uint32_t minValue;
    uint32_t maxValue;

    ObjectiveValueGoal(
        GoalID _goalID, uint32_t _minValue, uint32_t _maxValue, GoalType _goalType, uint16_t _leeWayPeriod, bool _usesMoney,
        uint16_t _warningDaysIndex)
        : ObjectiveGoal(_goalID, _goalType, _usesMoney, _warningDaysIndex, _leeWayPeriod)
        , minValue(_minValue)
        , maxValue(_maxValue){};

public:
    bool CheckSpecialRequirements(rct_string_id& error) const override;

    uint32_t GetMinValue()
    {
        return minValue;
    }

    void SetMinValue(uint32_t _minValue)
    {
        minValue = _minValue;
        redoToString = true;
    }
    uint32_t GetMaxValue()
    {
        return maxValue;
    }

    void SetMaxValue(uint32_t _maxValue)
    {
        maxValue = _maxValue;
        redoToString = true;
    }

    bool CheckGoalRestrictionWarningDays(
        bool _TooLow, bool _TooHigh, rct_string_id _goalAsWord, rct_string_id _valueStringFormat = STR_FORMAT_INTEGER);
    bool CheckValues(uint32_t value, rct_string_id _goalAsWord, rct_string_id _valueStringFormat = STR_FORMAT_INTEGER);
};

/// <summary>
/// Get a number of guests
/// </summary>
class ObjectiveGuestNumGoal final : public ObjectiveValueGoal
{
public:
    ObjectiveGuestNumGoal(uint16_t _guestNumMin, uint16_t _guestNumMax, GoalType _goalType, uint16_t _leeWayPeriod)
        : ObjectiveValueGoal(GoalID::GuestNumGoal, _guestNumMin, _guestNumMax, _goalType, _leeWayPeriod, false, 0){};
    bool CheckCondition() override;
};

/// <summary>
/// Get a certain park value
/// </summary>
class ObjectiveParkValueGoal final : public ObjectiveValueGoal
{
public:
    ObjectiveParkValueGoal(money32 _parkValueMin, money32 _parkValueMax, GoalType _goalType, uint16_t _leeWayPeriod)
        : ObjectiveValueGoal(GoalID::ParkValueGoal, _parkValueMin, _parkValueMax, _goalType, _leeWayPeriod, false, 0){};
    bool CheckCondition() override;
};

/// <summary>
/// Get a certain park rating
/// </summary>
class ObjectiveParkRatingGoal final : public ObjectiveValueGoal
{
public:
    ObjectiveParkRatingGoal(uint16_t _parkRatingMin, uint16_t _parkRatingMax, GoalType _goalType, uint16_t _leeWayPeriod)
        : ObjectiveValueGoal(GoalID::ParkRatingGoal, _parkRatingMin, _parkRatingMax, _goalType, _leeWayPeriod, false, 0){};
    bool CheckCondition() override;
};

/// <summary>
/// Get a certain profit
/// </summary>
class ObjectiveProfitGoal : public ObjectiveValueGoal
{
protected:
    uint8_t profitTypeFlags;

public:
    ObjectiveProfitGoal(
        uint8_t _profitTypeFlags, uint32_t _incomeMin, uint32_t _incomeMax, GoalType _goalType, uint16_t _leeWayPeriod)
        : ObjectiveValueGoal(GoalID::ProfitGoal, _incomeMin, _incomeMax, _goalType, _leeWayPeriod, true, 0)
        , profitTypeFlags(_profitTypeFlags){};

    bool CheckCondition() override;
    uint8_t GetProfitTypeFlags() const
    {
        return profitTypeFlags;
    }
    void SetProfitGoalFlags(uint8_t _profitTypeFlag)
    {
        profitTypeFlags = _profitTypeFlag;
        redoToString = true;
    }
    bool CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const override;
};

/// <summary>
/// Repay the loan
/// </summary>
class ObjectiveRepayLoanGoal final : public ObjectiveGoal
{
public:
    ObjectiveRepayLoanGoal()
        : ObjectiveGoal(GoalID::RepayLoanGoal, GoalType::Goal, true){};
    bool CheckCondition() override;
};

/// <summary>
/// Small class with for holding a baseridetype and entryindex
/// </summary>
class RideInObjective
{
protected:
    uint16_t baseRideType;
    ObjectEntryIndex entryIndex;
    RideInObjective(uint16_t _baseRideType, ObjectEntryIndex _entryIndex)
        : baseRideType(_baseRideType)
        , entryIndex(_entryIndex){};
    virtual void SetRedoToString() = 0;

public:
    uint16_t GetBaseRideType() const
    {
        return baseRideType;
    }
    virtual void SetBaseRideType(uint16_t _baseRideType)
    {
        baseRideType = _baseRideType;
        SetRedoToString();
    }
    uint16_t GetEntryIndex() const
    {
        return entryIndex;
    }
    void SetEntryIndex(ObjectEntryIndex _entryIndex)
    {
        entryIndex = _entryIndex;
        SetRedoToString();
    }
    virtual ~RideInObjective() = default;
};

/// <summary>
/// Unlock a ride through research
/// </summary>
class ObjectiveResearchRide : public ObjectiveGoal, public RideInObjective
{
public:
    ObjectiveResearchRide(uint16_t _baseRideType, ObjectEntryIndex _entryIndex)
        : ObjectiveGoal(GoalID::ResearchCertainRide, GoalType::Goal, true)
        , RideInObjective(_baseRideType, _entryIndex){};
    bool CheckCondition() override;
    bool CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const override;

    void SetRedoToString() override
    {
        redoToString = true;
    }
};

/// <summary>
/// Small class that holds a list of ridestatrequirements
/// </summary>
class RideRequirementsList
{
protected:
    std::vector<RideStatRequirement> requirements;
    virtual void SetRedoToString() = 0;

public:
    bool AddRequirement(RideStatRequirement req, rct_string_id& error);
    void ClearRequirements()
    {
        requirements.clear();
    };
    bool RemoveRequirement(uint8_t position);
    bool ReplaceRequirement(RideStatRequirement req, uint8_t pos, rct_string_id& error);
    RideStatRequirement GetRequirement(uint8_t _number)
    {
        return requirements[_number];
    }

    virtual std::vector<RideStatRequirement> GetAvailableRequirements() = 0;
    virtual ~RideRequirementsList() = default;
};

/// <summary>
/// Build a ride
/// </summary>
class ObjectiveSpecificRideGoal : public ObjectiveGoal, public RideInObjective, public RideRequirementsList
{
    bool mustBeOnlySuchRide;
    bool buildBestGoal;

public:
    ObjectiveSpecificRideGoal(uint16_t _baseRideType, ObjectEntryIndex _entryIndex, bool _unique, bool _buildBestGoal)
        : ObjectiveGoal(GoalID::SpecificRideGoal, GoalType::Goal, false)
        , RideInObjective(_baseRideType, _entryIndex)
        , mustBeOnlySuchRide(_unique)
        , buildBestGoal(_buildBestGoal){};
    bool CheckCondition() override;
    bool CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const override;
    bool GetMustBeOnlySuchRide() const
    {
        return mustBeOnlySuchRide;
    }
    void SetMustBeOnlySuchRide(bool _mustBeOnlySuchRide)
    {
        mustBeOnlySuchRide = _mustBeOnlySuchRide;
        redoToString = true;
    }
    bool GetBuildBestGoal() const
    {
        return buildBestGoal;
    }
    void SetBuildBestGoal(bool _buildBestGoal)
    {
        buildBestGoal = _buildBestGoal;
        redoToString = true;
    }
    void SetRedoToString() override
    {
        redoToString = true;
    }
    std::vector<RideStatRequirement> GetAvailableRequirements() override
    {
        return std::vector<RideStatRequirement>(); // TODO when UI is made, only allow types that haven't been added and fit
                                                   // with the ride chosen
    }
};

/// <summary>
/// Build a number of rides of a certain category
/// </summary>
class ObjectiveRidesGoal final : public ObjectiveGoal, public RideRequirementsList
{
protected:
    uint8_t rideCategory;
    uint8_t minNumber;
    uint8_t maxNumber;
    bool mustBeUniqueTypes;
    bool finishedExistingRides;

public:
    ObjectiveRidesGoal(uint8_t _rideCategory, uint8_t _minNum, uint8_t _maxNum, bool _unique, bool _finishedExistingRides)
        : ObjectiveGoal(GoalID::NumRidesGoal, GoalType::Goal)
        , rideCategory(_rideCategory)
        , minNumber(_minNum)
        , maxNumber(_maxNum)
        , mustBeUniqueTypes(_unique)
        , finishedExistingRides(_finishedExistingRides){};
    bool CheckCondition() override;
    bool CheckSpecialRequirements(rct_string_id& error) const override;
    bool CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const override;

    uint8_t GetRideCategory() const
    {
        return rideCategory;
    }
    void SetRideCategory(uint8_t _rideCategory)
    {
        rideCategory = _rideCategory;
        redoToString = true;
    }

    uint8_t GetMinNumber() const
    {
        return minNumber;
    }
    void SetMinNumber(uint8_t _minNumber)
    {
        minNumber = _minNumber;
        redoToString = true;
    }
    uint8_t GetMaxNumber() const
    {
        return maxNumber;
    }
    void SetMaxNumber(uint8_t _maxNumber)
    {
        maxNumber = _maxNumber;
        redoToString = true;
    }
    bool GetMustBeUniqueTypes() const
    {
        return mustBeUniqueTypes;
    }
    void SetMustBeUniqueTypes(bool _mustBeUniqueTypes)
    {
        mustBeUniqueTypes = _mustBeUniqueTypes;
        redoToString = true;
    }

    bool GetFinishedExistingRides() const
    {
        return finishedExistingRides;
    }
    void SetFinishedExistingRidess(bool _finishedExistingRides)
    {
        finishedExistingRides = _finishedExistingRides;
        redoToString = true;
    }
    void SetRedoToString() override
    {
        redoToString = true;
    }
    std::vector<RideStatRequirement> GetAvailableRequirements() override
    {
        return std::vector<RideStatRequirement>(); // TODO when UI is made, only allow types that haven't been added and fit
                                                   // with the category chosen
    }
};

/// <summary>
/// Have fun, no objective
/// </summary>
class ObjectiveFunGoal final : public ObjectiveGoal // would be fun if after an hour or so a window'd pop up "Are you having
                                                    // fun? Yes
                                                    // No", yes leads to winning, no to losing
{
public:
    ObjectiveFunGoal()
        : ObjectiveGoal(GoalID::FunGoal, GoalType::Goal){};
    bool CheckCondition() override
    {
        return false;
    }
};

#ifdef __WARN_SUGGEST_FINAL_METHODS__
#    pragma GCC diagnostic pop
#endif
