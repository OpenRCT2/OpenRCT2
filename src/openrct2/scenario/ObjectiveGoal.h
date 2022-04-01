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
    ParkSizeGoal = 4,
    ProfitGoal = 5,
    RideTicketProfitGoal = 6,
    ParkEntryProfitGoal = 7,
    StallProfitGoal = 8,
    FoodProfitGoal = 9,
    MerchandiseProfitGoal = 10,
    RepayLoanGoal = 11,
    NoDebtGoal = 12,
    NoDeathsGoal = 13,
    NoDuplicateRidesGoal = 14,
    NoExtremeRides = 15,
    CompleteResearchGoal = 16,
    ResearchCertainRide = 17,
    SpecificRideGoal = 18,
    SpecificTrackedRideGoal = 19,
    TransportRidesGoal = 20,
    GentleRidesGoal = 21,
    CoasterGoal = 22,
    ThrillRidesGoal = 23,
    WaterRidesGoal = 24,
    AwardGoal = 25,
    NumPositiveAwardsGoal = 26,
    NoNegativeAwardsGoal = 27,
    FunGoal = 28,

};
const rct_string_id GoalShortStringIds[29] = {
    STR_DUMMY_GOAL_SHORT,
    STR_GUEST_NUM_GOAL_SHORT,
    STR_PARK_VALUE_GOAL_SHORT,
    STR_PARK_RATING_GOAL_SHORT,
    STR_PARK_SIZE_GOAL_SHORT,
    STR_PROFIT_GOAL_SHORT,
    STR_RIDE_TICKET_PROFIT_GOAL_SHORT,
    STR_PARK_ENTRY_PROFIT_GOAL_SHORT,
    STR_STALL_PROFIT_GOAL_SHORT,
    STR_FOOD_PROFIT_GOAL_SHORT,
    STR_MERCH_PROFIT_GOAL_SHORT,
    STR_REPAY_LOAN_GOAL_SHORT,
    STR_NO_DEBT_GOAL_SHORT,
    STR_NO_DEATHS_GOAL_SHORT,
    STR_NO_DUPLICATE_RIDES_GOAL_SHORT,
    STR_NO_EXTREME_RIDES_SHORT,
    STR_COMPLETE_RESEARCH_GOAL_SHORT,
    STR_RESEARCH_A_RIDE_SHORT,
    STR_BUILD_RIDE_GOAL_SHORT,
    STR_BUILD_TRACKED_RIDE_GOAL_SHORT,
    STR_NUM_TRANSPORT_RIDE_GOAL_SHORT,
    STR_NUM_GENTLE_RIDE_GOAL_SHORT,
    STR_NUM_COASTER_GOAL_SHORT,
    STR_NUM_THRILL_RIDE_GOAL_SHORT,
    STR_NUM_WATER_RIDE_GOAL_SHORT,
    STR_WIN_AWARD_GOAL_SHORT,
    STR_WIN_NUM_AWARDS_GOAL_SHORT,
    STR_NO_NEGATIVE_AWARDS_GOAL_SHORT,
    STR_HAVE_FUN_GOAL_SHORT,
};

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
    virtual std::string ToString() = 0;
    rct_string_id ToStringShort() const
    {
        return GoalShortStringIds[static_cast<uint8_t>(goalID)];
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
    std::string ToString() override;
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
    std::string ToString() override;
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
    std::string ToString() override;
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
    std::string ToString() override;
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
/// Expand the park to a size
/// </summary>
class ObjectiveParkSizeGoal final : public ObjectiveGoal
{
    uint32_t parkSizeGoal;

public:
    ObjectiveParkSizeGoal(uint32_t _parkSize, Sign _sign)
        : ObjectiveGoal(GoalID::ParkSizeGoal, GoalType::Goal, _sign)
        , parkSizeGoal(_parkSize)
    {
        if (sign == Sign::SmallerThan) // No need for warning days, so skipping the parent constructor's if type = restriction
                                       // is fine.
            goalType = GoalType::Restriction;
    };
    bool CheckCondition() override;
    bool CheckSpecialRequirements(rct_string_id& error) const override;
    std::string ToString() override;
    uint32_t GetParkSizeGoal() const
    {
        return parkSizeGoal;
    }
    void SetParkSizeGoal(uint32_t _parkSizeGoal)
    {
        parkSizeGoal = _parkSizeGoal;
        redoToString = true;
    }
    bool CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const override;
    void SetSign(Sign _sign) override
    {
        ObjectiveGoal::SetSign(_sign);
        if (sign == Sign::SmallerThan) // No need for warning days, so skipping the parent constructor's if type = restriction
                                       // is fine.
            goalType = GoalType::Restriction;
    }
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
    std::string ToString() override;
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
    std::string ToString() override;
};

/// <summary>
/// Get a monthly profit from park entry
/// </summary>
class ObjectiveParkEntryProfitGoal final : public ObjectiveProfitGoal
{
public:
    ObjectiveParkEntryProfitGoal(uint32_t _income, Sign _sign, GoalType _goalType, uint16_t _leeWayPeriod)
        : ObjectiveProfitGoal(_income, _sign, _goalType, _leeWayPeriod, GoalID::ParkEntryProfitGoal){};
    bool CheckCondition() override;
    bool CheckSpecialRequirements(rct_string_id& error) const override;
    std::string ToString() override;
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
    std::string ToString() override;
};

/// <summary>
/// Get a monthly profit from food sales
/// </summary>
class ObjectiveFoodProfitGoal final : public ObjectiveProfitGoal
{
public:
    ObjectiveFoodProfitGoal(uint32_t _income, Sign _sign, GoalType _goalType, uint16_t _leeWayPeriod)
        : ObjectiveProfitGoal(_income, _sign, _goalType, _leeWayPeriod, GoalID::FoodProfitGoal){};
    bool CheckCondition() override;
    std::string ToString() override;
};

/// <summary>
/// Get a monthly profit from merchandise sales
/// </summary>
class ObjectiveMerchandiseProfitGoal final : public ObjectiveProfitGoal
{
public:
    ObjectiveMerchandiseProfitGoal(uint32_t _income, Sign _sign, GoalType _goalType, uint16_t _leeWayPeriod)
        : ObjectiveProfitGoal(_income, _sign, _goalType, _leeWayPeriod, GoalID::MerchandiseProfitGoal){};
    bool CheckCondition() override;
    std::string ToString() override;
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
    std::string ToString() override;
};

/// <summary>
/// Never have debt
/// </summary>
class ObjectiveNoDebtGoal final : public ObjectiveGoal
{
public:
    ObjectiveNoDebtGoal()
        : ObjectiveGoal(GoalID::NoDebtGoal, GoalType::Restriction, Sign::None, true, 0, 1){};
    bool CheckCondition() override;
    std::string ToString() override;
};

/// <summary>
/// Do not let guests die
/// </summary>
class ObjectiveNoDeathsGoal final : public ObjectiveGoal
{
public:
    ObjectiveNoDeathsGoal()
        : ObjectiveGoal(GoalID::NoDeathsGoal, GoalType::Restriction){};
    bool CheckCondition() override;
    std::string ToString() override;
};

/// <summary>
/// Do not build more than 1 of each ride type
/// </summary>
class ObjectiveNoDuplicateRidesGoal final : public ObjectiveGoal
{
    uint8_t category; // 0trans, 1gentle, 2roller, 3thrill, 4water, 5stall, 6all rides, 7all

public:
    ObjectiveNoDuplicateRidesGoal(uint8_t _category = 6)
        : ObjectiveGoal(GoalID::NoDuplicateRidesGoal, GoalType::Restriction)
        , category(_category){};
    bool CheckCondition() override;
    bool CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const override;
    std::string ToString() override;

    uint8_t GetCategory() const
    {
        return category;
    }
    void SetCategory(uint8_t _category)
    {
        category = _category;
        redoToString = true;
    }
};

/// <summary>
/// Do not build rides with an extreme intensity
/// </summary>
class ObjectiveNoExtremeRidesGoal final : public ObjectiveGoal
{
public:
    ObjectiveNoExtremeRidesGoal()
        : ObjectiveGoal(GoalID::NoExtremeRides, GoalType::Hybrid, Sign::None, false, 1){};
    bool CheckCondition() override;
    bool CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const override;
    std::string ToString() override;
};

/// <summary>
/// Complete all research
/// </summary>
class ObjectiveCompleteResearchGoal final : public ObjectiveGoal
{
public:
    ObjectiveCompleteResearchGoal()
        : ObjectiveGoal(GoalID::CompleteResearchGoal, GoalType::Hybrid, Sign::None, true){};
    bool CheckCondition() override;
    bool CheckSpecialRequirements(rct_string_id& error) const override;
    std::string ToString() override;
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
    std::string ToString() override;
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
    std::string ToString() override;
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
/// Build a tracked ride
/// </summary>
class ObjectiveSpecificTrackedRideGoal final : public ObjectiveSpecificRideGoal
{
    uint8_t minDropCountGoal;
    uint8_t maxDropCountGoal;
    uint16_t minDropHeightGoal;
    uint16_t maxDropHeightGoal;
    uint16_t minLengthGoal;
    uint16_t maxLengthGoal;
    uint16_t minExcitementGoal;
    uint16_t maxExcitementGoal;
    uint16_t minIntensityGoal;
    uint16_t maxIntensityGoal;
    uint16_t minNauseaGoal;
    uint16_t maxNauseaGoal;
    uint16_t minSpeedGoal;
    uint16_t maxSpeedGoal;
    uint8_t minNumInversions;
    uint8_t maxNumInversions;
    std::vector<uint16_t> trackPiecesRequired;

public:
    ObjectiveSpecificTrackedRideGoal(
        uint16_t _baseRideType, ObjectEntryIndex _entryIndex, bool _unique, uint8_t _minDrops, uint8_t _maxDrops,
        uint16_t _minDropHeight, uint16_t _maxDropHeight, uint16_t _minLen, uint16_t _maxLen, float _minEx, float _maxEx,
        float _minIn, float _maxIn, float _minNau, float _maxNau, uint16_t _minSpeed, uint16_t _maxSpeed,
        uint8_t _minNumInversions, uint8_t _maxNumInversions, std::vector<uint16_t> _trackPiecesRequired = {})
        : ObjectiveSpecificRideGoal(_baseRideType, _entryIndex, _unique, GoalID::SpecificTrackedRideGoal)
        , minDropCountGoal(_minDrops)
        , maxDropCountGoal(_maxDrops)
        , minDropHeightGoal(_minDropHeight)
        , maxDropHeightGoal(_maxDropHeight)
        , minLengthGoal(_minLen)
        , maxLengthGoal(_maxLen)
        , minExcitementGoal(uint16_t(_minEx * 100))
        , maxExcitementGoal(uint16_t(_maxEx * 100))
        , minIntensityGoal(uint16_t(_minIn * 100))
        , maxIntensityGoal(uint16_t(_maxIn * 100))
        , minNauseaGoal(uint16_t(_minNau * 100))
        , maxNauseaGoal(uint16_t(_maxNau * 100))
        , minSpeedGoal(_minSpeed)
        , maxSpeedGoal(_maxSpeed)
        , minNumInversions(_minNumInversions)
        , maxNumInversions(_maxNumInversions)
        , trackPiecesRequired(_trackPiecesRequired){};
    bool CheckCondition() override;
    bool CheckSpecialRequirements(rct_string_id& error) const override;
    bool CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const override;
    std::string ToString() override;
    uint8_t GetMinDropCountGoal() const
    {
        return minDropCountGoal;
    }
    void SetMinDropCountGoal(uint8_t _minDropCountGoal)
    {
        minDropCountGoal = _minDropCountGoal;
        redoToString = true;
    }
    uint8_t GetMaxDropCountGoal() const
    {
        return maxDropCountGoal;
    }
    void SetMaxDropCountGoal(uint8_t _maxDropCountGoal)
    {
        maxDropCountGoal = _maxDropCountGoal;
        redoToString = true;
    }
    uint16_t GetMinDropHeightGoal() const
    {
        return minDropHeightGoal;
    }
    void SetMinDropHeightGoal(uint16_t _minDropHeightGoal)
    {
        minDropHeightGoal = _minDropHeightGoal;
        redoToString = true;
    }
    uint16_t GetMaxDropHeightGoal() const
    {
        return maxDropHeightGoal;
    }
    void SetMaxDropHeightGoal(uint16_t _maxDropHeightGoal)
    {
        maxDropHeightGoal = _maxDropHeightGoal;
        redoToString = true;
    }
    uint16_t GetMinLengthGoal() const
    {
        return minLengthGoal;
    }
    void SetMinLengthGoal(uint16_t _minLengthGoal)
    {
        minLengthGoal = _minLengthGoal;
        redoToString = true;
    }
    uint16_t GetMaxLengthGoal() const
    {
        return maxLengthGoal;
    }
    void SetMaxLengthGoal(uint16_t _maxLengthGoal)
    {
        maxLengthGoal = _maxLengthGoal;
        redoToString = true;
    }
    uint16_t GetMinExcitementGoal() const
    {
        return minExcitementGoal;
    }
    void SetMinExcitementGoal(uint16_t _minExcitementGoal)
    {
        minExcitementGoal = _minExcitementGoal;
        redoToString = true;
    }
    uint16_t GetMaxExcitementGoal() const
    {
        return maxExcitementGoal;
    }
    void SetMaxExcitementGoal(uint16_t _maxExcitementGoal)
    {
        maxExcitementGoal = _maxExcitementGoal;
        redoToString = true;
    }
    uint16_t GetMinIntensityGoal() const
    {
        return minIntensityGoal;
    }
    void SetMinIntensityGoal(uint16_t _minIntensityGoal)
    {
        minIntensityGoal = _minIntensityGoal;
        redoToString = true;
    }
    uint16_t GetMaxIntensityGoal() const
    {
        return maxIntensityGoal;
    }
    void SetMaxIntensityGoal(uint16_t _maxIntensityGoal)
    {
        maxIntensityGoal = _maxIntensityGoal;
        redoToString = true;
    }
    uint16_t GetMinNauseaGoal() const
    {
        return minNauseaGoal;
    }
    void SetMinNauseaGoal(uint16_t _minNauseaGoal)
    {
        minNauseaGoal = _minNauseaGoal;
        redoToString = true;
    }
    uint16_t GetMaxNauseaGoal() const
    {
        return maxNauseaGoal;
    }
    void SetMaxNauseaGoal(uint16_t _maxNauseaGoal)
    {
        maxNauseaGoal = _maxNauseaGoal;
        redoToString = true;
    }
    uint16_t GetMinSpeedGoal() const
    {
        return minSpeedGoal;
    }
    void SetMinSpeedGoal(uint16_t _minSpeedGoal)
    {
        minSpeedGoal = _minSpeedGoal;
        redoToString = true;
    }
    uint16_t GetMaxSpeedGoal() const
    {
        return maxSpeedGoal;
    }
    void SetMaxSpeedGoal(uint16_t _maxSpeedGoal)
    {
        maxSpeedGoal = _maxSpeedGoal;
        redoToString = true;
    }
    uint8_t GetMinNumInversions() const
    {
        return minNumInversions;
    }
    void SetMinNumInversions(uint8_t _minNumInversions)
    {
        minNumInversions = _minNumInversions;
        redoToString = true;
    }
    uint8_t GetMaxNumInversions() const
    {
        return maxNumInversions;
    }
    void SetMaxNumInversions(uint8_t _maxNumInversions)
    {
        maxNumInversions = _maxNumInversions;
        redoToString = true;
    }

    std::vector<uint16_t>* GetTrackPiecesRequired()
    {
        return &trackPiecesRequired;
    }
    void AddTrackPieceRequired(uint16_t p)
    {
        trackPiecesRequired.push_back(p);
        redoToString = true;
    }
    void RemoveTrackPieceRequired(uint16_t p)
    {
        trackPiecesRequired.erase(trackPiecesRequired.begin() + p);
        redoToString = true;
    }

    void SetBaseRideType(uint16_t _baseRideType) override
    {
        baseRideType = _baseRideType;
        trackPiecesRequired = std::vector<uint16_t>();
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
    std::string ToString() override;
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
    std::string ToString() override;
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
    std::string ToString() override;
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
    std::string ToString() override;
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
    std::string ToString() override;
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
/// Win an award
/// </summary>
class ObjectiveAwardGoal final : public ObjectiveGoal
{
    AwardType awardsGoal;
    bool atAnyTime;

public:
    ObjectiveAwardGoal(AwardType _award, bool _atAnyTime)
        : ObjectiveGoal(GoalID::AwardGoal, GoalType::Goal)
        , awardsGoal(_award)
        , atAnyTime(_atAnyTime){};
    bool CheckCondition() override;
    bool CheckSpecialRequirements(rct_string_id& error) const override;
    bool CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const override;
    std::string ToString() override;
    AwardType GetAwardsGoal() const
    {
        return awardsGoal;
    }
    void SetAwardsGoal(AwardType _awardsGoal)
    {
        awardsGoal = _awardsGoal;
        redoToString = true;
    }
    bool GetAtAnyTime() const
    {
        return atAnyTime;
    }
    void SetAtAnyTime(bool _atAnyTime)
    {
        atAnyTime = _atAnyTime;
        redoToString = true;
    }
};

/// <summary>
/// Win a number of postive awards
/// </summary>
class ObjectiveNumPositiveAwardsGoal final : public ObjectiveGoal
{
    uint8_t numAwards;
    std::vector<AwardType> typesAchieved;

public:
    ObjectiveNumPositiveAwardsGoal(uint8_t _NumAwards)
        : ObjectiveGoal(GoalID::NumPositiveAwardsGoal, GoalType::Goal)
        , numAwards(_NumAwards){};
    bool CheckCondition() override;
    std::string ToString() override;
    uint8_t GetNumAwards() const
    {
        return numAwards;
    }
    void SetNumAwards(uint8_t _numAwards)
    {
        numAwards = _numAwards;
        redoToString = true;
    }

    std::vector<AwardType>* GetTypesAchieved()
    {
        return &typesAchieved;
    }
    void AddTypesAchieved(AwardType p)
    {
        typesAchieved.push_back(p);
        redoToString = true;
    }
    void RemoveTypesAchieved(uint8_t p)
    {
        typesAchieved.erase(typesAchieved.begin() + p);
        redoToString = true;
    }
};

/// <summary>
/// Never win negative awards
/// </summary>
class ObjectiveNoNegativeAwardsGoal final : public ObjectiveGoal
{
public:
    ObjectiveNoNegativeAwardsGoal()
        : ObjectiveGoal(GoalID::NoNegativeAwardsGoal, GoalType::Restriction){};
    bool CheckCondition() override;
    bool CheckConflictingGoal(ObjectiveGoalPtr _otherGoal, rct_string_id& error) const override;
    std::string ToString() override;
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
    std::string ToString() override;
};
