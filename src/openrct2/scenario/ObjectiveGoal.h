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
#include "../ride/RideRatings.h"

enum class GoalType : uint8_t
{
    Goal,
    Restriction,
    Hybrid,
    OptionalGoal
};

enum class Sign : uint8_t
{
    None,
    BiggerThan,
    SmallerThan
};

enum class GoalID : uint8_t
{
    DummyGoal = 0,
    GuestNumGoal = 1,
    ParkValueGoal = 2,
    ParkRatingGoal = 3,
    ProfitGoal = 5,
    RideTicketProfitGoal = 6,
    StallProfitGoal = 8,
    RepayLoanGoal = 11,
    ResearchCertainRide = 17,
    SpecificRideGoal = 18,
    TransportRidesGoal = 20,
    GentleRidesGoal = 21,
    CoasterGoal = 22,
    ThrillRidesGoal = 23,
    WaterRidesGoal = 24,
    FunGoal = 28,

};

#ifdef __WARN_SUGGEST_FINAL_METHODS__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wsuggest-final-methods"
#    pragma GCC diagnostic ignored "-Wsuggest-final-types"
#endif

class ObjectiveGoal;
using ObjectiveGoalPtr = std::shared_ptr<ObjectiveGoal>;
class ObjectiveGoal
{
protected:
    GoalID goalID;
    GoalType goalType;
    Sign sign;
    uint16_t warningDaysIndex, warningWeeksPeriod;
    bool trueOnLastCheck;
    bool countingDown = false;
    bool usesMoney;           // Doesn't need to be saved into savegames
    std::string longText;     // Doesn't need to be saved.
    bool redoToString = true; // Doesn't need to be saved

public:
    ObjectiveGoal(
        GoalID _id, GoalType _type, Sign _sign = Sign::None, bool _usesMoney = false, uint16_t _warningDayIndex = 0,
        uint16_t _warningWeekPeriod = 0);

    virtual bool CheckCondition() = 0;
    bool CheckGoalRestrictionWarningDays(
        bool _test, rct_string_id _goalAsWord, uint64_t _value, rct_string_id _valueStringFormat = STR_FORMAT_INTEGER);
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
    Sign GetSign() const
    {
        return sign;
    }
    virtual void SetSign(Sign _sign)
    {
        sign = _sign;
        redoToString = true;
    }
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

/// <summary>
/// Get a number of guests
/// </summary>
class ObjectiveGuestNumGoal final : public ObjectiveGoal
{
    uint16_t guestNumGoal;

public:
    ObjectiveGuestNumGoal(uint16_t _guestNum, Sign _sign, GoalType _goalType, uint16_t _leeWayPeriod)
        : ObjectiveGoal(GoalID::GuestNumGoal, _goalType, _sign, false, 0, _leeWayPeriod)
        , guestNumGoal(_guestNum){};
    bool CheckCondition() override;
    uint16_t GetGuestNumGoal() const
    {
        return guestNumGoal;
    }
    void SetGuestNumGoal(uint16_t _guestNumGoal)
    {
        guestNumGoal = _guestNumGoal;
        redoToString = true;
    }
    bool CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const override;
};

/// <summary>
/// Get a certain park value
/// </summary>
class ObjectiveParkValueGoal final : public ObjectiveGoal
{
    money64 parkValueGoal;

public:
    ObjectiveParkValueGoal(money64 _parkValue, Sign _sign, GoalType _goalType, uint16_t _leeWayPeriod)
        : ObjectiveGoal(GoalID::ParkValueGoal, _goalType, _sign, false, 0, _leeWayPeriod)
        , parkValueGoal(_parkValue){};
    bool CheckCondition() override;
    money64 GetParkValueGoal() const
    {
        return parkValueGoal;
    }
    void SetParkValueGoal(money64 _parkValueGoal)
    {
        parkValueGoal = _parkValueGoal;
        redoToString = true;
    }
    bool CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const override;
};

/// <summary>
/// Get a certain park rating
/// </summary>
class ObjectiveParkRatingGoal final : public ObjectiveGoal
{
    uint16_t parkRatingGoal;

public:
    ObjectiveParkRatingGoal(uint16_t _parkRating, Sign _sign, GoalType _goalType, uint16_t _leeWayPeriod)
        : ObjectiveGoal(GoalID::ParkRatingGoal, _goalType, _sign, false, 0, _leeWayPeriod)
        , parkRatingGoal(_parkRating){};
    bool CheckCondition() override;
    uint16_t GetParkRatingGoal() const
    {
        return parkRatingGoal;
    }
    void SetParkRatingGoal(uint16_t _parkRatingGoal)
    {
        parkRatingGoal = _parkRatingGoal;
        redoToString = true;
    }
    bool CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const override;
};

/// <summary>
/// Get a monthly total profit
/// </summary>
class ObjectiveProfitGoal : public ObjectiveGoal
{
protected:
    uint32_t profitGoal;

public:
    ObjectiveProfitGoal(uint32_t _income, Sign _sign, GoalType _goalType, uint16_t _leeWayPeriod, GoalID _id)
        : ObjectiveGoal(_id, _goalType, _sign, true, 0, _leeWayPeriod)
        , profitGoal(_income){};

    ObjectiveProfitGoal(uint32_t _income, Sign _sign, GoalType _goalType, uint16_t _leeWayPeriod)
        : ObjectiveGoal(GoalID::ProfitGoal, _goalType, _sign, true, 0, _leeWayPeriod)
        , profitGoal(_income){};

    bool CheckCondition() override;
    uint32_t GetProfitGoal() const
    {
        return profitGoal;
    }
    void SetProfitGoal(uint32_t _profitGoal)
    {
        profitGoal = _profitGoal;
        redoToString = true;
    }
    bool CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const override;
};

/// <summary>
/// Get a monthly profit from ride tickets
/// </summary>
class ObjectiveRideTicketProfitGoal final : public ObjectiveProfitGoal
{
public:
    ObjectiveRideTicketProfitGoal(uint32_t _income, Sign _sign, GoalType _goalType, uint16_t _leeWayPeriod)
        : ObjectiveProfitGoal(_income, _sign, _goalType, _leeWayPeriod, GoalID::RideTicketProfitGoal){};
    bool CheckCondition() override;
    bool CheckSpecialRequirements(rct_string_id& error) const override;
};

/// <summary>
/// Get a monthly profit from stall sales
/// </summary>
class ObjectiveStallProfitGoal final : public ObjectiveProfitGoal
{
public:
    ObjectiveStallProfitGoal(uint32_t _income, Sign _sign, GoalType _goalType, uint16_t _leeWayPeriod)
        : ObjectiveProfitGoal(_income, _sign, _goalType, _leeWayPeriod, GoalID::StallProfitGoal){};
    bool CheckCondition() override;
};

/// <summary>
/// Repay the loan
/// </summary>
class ObjectiveRepayLoanGoal final : public ObjectiveGoal
{
public:
    ObjectiveRepayLoanGoal()
        : ObjectiveGoal(GoalID::RepayLoanGoal, GoalType::Goal, Sign::None, true){};
    bool CheckCondition() override;
};

/// <summary>
/// Unlock a ride through research
/// </summary>
class ObjectiveResearchRide : public ObjectiveGoal
{
protected:
    uint16_t baseRideType;
    ObjectEntryIndex entryIndex;

    ObjectiveResearchRide(uint16_t _baseRideType, ObjectEntryIndex _entryIndex, GoalID _id)
        : ObjectiveGoal(_id, GoalType::Goal)
        , baseRideType(_baseRideType)
        , entryIndex(_entryIndex){};

public:
    ObjectiveResearchRide(uint16_t _baseRideType, ObjectEntryIndex _entryIndex)
        : ObjectiveGoal(GoalID::ResearchCertainRide, GoalType::Goal, Sign::None, true)
        , baseRideType(_baseRideType)
        , entryIndex(_entryIndex){};
    bool CheckCondition() override;
    bool CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const override;
    uint16_t GetBaseRideType() const
    {
        return baseRideType;
    }
    virtual void SetBaseRideType(uint16_t _baseRideType)
    {
        baseRideType = _baseRideType;
        redoToString = true;
    }
    uint16_t GetEntryIndex() const
    {
        return entryIndex;
    }
    void SetEntryIndex(ObjectEntryIndex _entryIndex)
    {
        entryIndex = _entryIndex;
        redoToString = true;
    }
};

/// <summary>
/// Build a ride
/// </summary>
class ObjectiveSpecificRideGoal : public ObjectiveResearchRide
{
protected:
    bool mustBeOnlySuchRide;
    bool buildBestGoal;

    ObjectiveSpecificRideGoal(uint16_t _baseRideType, ObjectEntryIndex _entryIndex, bool _unique, GoalID _id)
        : ObjectiveResearchRide(_baseRideType, _entryIndex, _id)
        , mustBeOnlySuchRide(_unique)
        , buildBestGoal(false){};

public:
    ObjectiveSpecificRideGoal(uint16_t _baseRideType, ObjectEntryIndex _entryIndex, bool _unique, bool _buildBestGoal)
        : ObjectiveResearchRide(_baseRideType, _entryIndex, GoalID::SpecificRideGoal)
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
};

/// <summary>
/// Build a number of thrill rides
/// </summary>
class ObjectiveThrillRidesGoal : public ObjectiveGoal
{
protected:
    uint8_t minNumRidesGoal;
    uint8_t maxNumRidesGoal;
    bool mustBeUniqueTypes;

    ObjectiveThrillRidesGoal(uint8_t _minNum, uint8_t _maxNum, bool _unique, GoalID _id)
        : ObjectiveGoal(_id, GoalType::Goal)
        , minNumRidesGoal(_minNum)
        , maxNumRidesGoal(_maxNum)
        , mustBeUniqueTypes(_unique){};

public:
    ObjectiveThrillRidesGoal(uint8_t _minNum, uint8_t _maxNum, bool _unique)
        : ObjectiveGoal(GoalID::ThrillRidesGoal, GoalType::Goal)
        , minNumRidesGoal(_minNum)
        , maxNumRidesGoal(_maxNum)
        , mustBeUniqueTypes(_unique){};
    bool CheckCondition() override;
    bool CheckSpecialRequirements(rct_string_id& error) const override;
    uint8_t GetMinNumRidesGoal() const
    {
        return minNumRidesGoal;
    }
    void SetMinNumRidesGoal(uint8_t _minNumRidesGoal)
    {
        minNumRidesGoal = _minNumRidesGoal;
        redoToString = true;
    }
    uint8_t GetMaxNumRidesGoal() const
    {
        return maxNumRidesGoal;
    }
    void SetMaxNumRidesGoal(uint8_t _maxNumRidesGoal)
    {
        maxNumRidesGoal = _maxNumRidesGoal;
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
};

/// <summary>
/// Build a number of gentle rides
/// </summary>
class ObjectiveGentleRidesGoal : public ObjectiveThrillRidesGoal
{
protected:
    bool finishedExistingRides;

    ObjectiveGentleRidesGoal(uint8_t _minNum, uint8_t _maxNum, bool _unique, bool _finishedExistingRides, GoalID _id)
        : ObjectiveThrillRidesGoal(_minNum, _maxNum, _unique, _id)
        , finishedExistingRides(_finishedExistingRides){};

public:
    ObjectiveGentleRidesGoal(uint8_t _minNum, uint8_t _maxNum, bool _unique, bool _finishedExistingRides)
        : ObjectiveThrillRidesGoal(_minNum, _maxNum, _unique, GoalID::GentleRidesGoal)
        , finishedExistingRides(_finishedExistingRides){};
    bool CheckCondition() override;
    bool CheckSpecialRequirements(rct_string_id& error) const override;
    bool CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const override;
    bool GetFinishedExistingRides() const
    {
        return finishedExistingRides;
    }
    void SetFinishedExistingRidess(bool _finishedExistingRides)
    {
        finishedExistingRides = _finishedExistingRides;
        redoToString = true;
    }
};

/// <summary>
/// Build a number of transport rides
/// </summary>
class ObjectiveTransportRidesGoal : public ObjectiveGentleRidesGoal
{
protected:
    uint16_t minRideLengthGoal;
    uint16_t maxRideLengthGoal;

    ObjectiveTransportRidesGoal(
        uint8_t _minNum, uint8_t _maxNum, uint16_t _minLen, uint16_t _maxLen, bool _unique, bool _finishedExistingRides,
        GoalID _id)
        : ObjectiveGentleRidesGoal(_minNum, _maxNum, _unique, _finishedExistingRides, _id)
        , minRideLengthGoal(_minLen)
        , maxRideLengthGoal(_maxLen){};

public:
    ObjectiveTransportRidesGoal(
        uint8_t _minNum, uint8_t _maxNum, uint16_t _minLen, uint16_t _maxLen, bool _unique, bool _finishedExistingRides)
        : ObjectiveGentleRidesGoal(_minNum, _maxNum, _unique, _finishedExistingRides, GoalID::TransportRidesGoal)
        , minRideLengthGoal(_minLen)
        , maxRideLengthGoal(_maxLen){};
    bool CheckCondition() override;
    bool CheckSpecialRequirements(rct_string_id& error) const override;
    uint16_t GetMinRideLengthGoal() const
    {
        return minRideLengthGoal;
    }
    void SetMinRideLengthGoal(uint16_t _minRideLengthGoal)
    {
        minRideLengthGoal = _minRideLengthGoal;
        redoToString = true;
    }
    uint16_t GetMaxRideLengthGoal() const
    {
        return maxRideLengthGoal;
    }
    void SetMaxRideLengthGoal(uint16_t _maxRideLengthGoal)
    {
        maxRideLengthGoal = _maxRideLengthGoal;
        redoToString = true;
    }
};

/// <summary>
/// Build a number of waterrides
/// </summary>
class ObjectiveWaterRidesGoal : public ObjectiveTransportRidesGoal
{
protected:
    uint16_t minRideExcitementGoal;
    uint16_t maxRideExcitementGoal;

    ObjectiveWaterRidesGoal(
        uint8_t _minNum, uint8_t _maxNum, uint16_t _minLen, uint16_t _maxLen, float _minEx, float _maxEx, bool _unique,
        bool _finishedExistingRides, GoalID _id)
        : ObjectiveTransportRidesGoal(_minNum, _maxNum, _minLen, _maxLen, _unique, _finishedExistingRides, _id)
        , minRideExcitementGoal(uint16_t(_minEx * 100))
        , maxRideExcitementGoal(uint16_t(_maxEx * 100)){};

public:
    ObjectiveWaterRidesGoal(
        uint8_t _minNum, uint8_t _maxNum, uint16_t _minLen, uint16_t _maxLen, float _minEx, float _maxEx, bool _unique,
        bool _finishedExistingRides)
        : ObjectiveTransportRidesGoal(
            _minNum, _maxNum, _minLen, _maxLen, _unique, _finishedExistingRides, GoalID::WaterRidesGoal)
        , minRideExcitementGoal(uint16_t(_minEx * 100))
        , maxRideExcitementGoal(uint16_t(_maxEx * 100)){};
    bool CheckCondition() override;
    bool CheckSpecialRequirements(rct_string_id& error) const override;
    uint16_t GetMinRideExcitementGoal() const
    {
        return minRideExcitementGoal;
    }
    void SetMinRideExcitementGoal(uint16_t _minRideExcitementGoal)
    {
        minRideExcitementGoal = _minRideExcitementGoal;
        redoToString = true;
    }
    uint16_t GetMaxRideExcitementGoal() const
    {
        return maxRideExcitementGoal;
    }
    void SetMaxRideExcitementGoal(uint16_t _maxRideExcitementGoal)
    {
        maxRideExcitementGoal = _maxRideExcitementGoal;
        redoToString = true;
    }
};

/// <summary>
/// Build a number of roller coasters
/// </summary>
class ObjectiveCoasterGoal final : public ObjectiveWaterRidesGoal
{
protected:
    uint16_t minRideIntensityGoal;
    uint16_t maxRideIntensityGoal;
    uint16_t minRideNauseaGoal;
    uint16_t maxRideNauseaGoal;

public:
    ObjectiveCoasterGoal(
        uint8_t _minNum, uint8_t _maxNum, uint16_t _minLen, uint16_t _maxLen, float _minEx, float _maxEx, float _minIn,
        float _maxIn, float _minNau, float _maxNau, bool _unique, bool _finishedExistingRides)
        : ObjectiveWaterRidesGoal(
            _minNum, _maxNum, _minLen, _maxLen, _minEx, _maxEx, _unique, _finishedExistingRides, GoalID::CoasterGoal)
        , minRideIntensityGoal(uint16_t(_minIn * 100))
        , maxRideIntensityGoal(uint16_t(_maxIn * 100))
        , minRideNauseaGoal(uint16_t(_minNau * 1000))
        , maxRideNauseaGoal(uint16_t(_maxNau * 1000)){};
    bool CheckCondition() override;
    bool CheckSpecialRequirements(rct_string_id& error) const override;
    bool CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const override;
    uint16_t GetMinRideIntensityGoal() const
    {
        return minRideIntensityGoal;
    }
    void SetMinRideIntensityGoal(uint16_t _minRideIntensityGoal)
    {
        minRideIntensityGoal = _minRideIntensityGoal;
        redoToString = true;
    }
    uint16_t GetMaxRideIntensityGoal() const
    {
        return maxRideIntensityGoal;
    }
    void SetMaxRideIntensityGoal(uint16_t _maxRideIntensityGoal)
    {
        maxRideIntensityGoal = _maxRideIntensityGoal;
        redoToString = true;
    }
    uint16_t GetMinRideNauseaGoal() const
    {
        return minRideNauseaGoal;
    }
    void SetMinRideNauseaGoal(uint16_t _minRideNauseaGoal)
    {
        minRideNauseaGoal = _minRideNauseaGoal;
        redoToString = true;
    }
    uint16_t GetMaxRideNauseaGoal() const
    {
        return maxRideNauseaGoal;
    }
    void SetMaxRideNauseaGoal(uint16_t _maxRideNauseaGoal)
    {
        maxRideNauseaGoal = _maxRideNauseaGoal;
        redoToString = true;
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
