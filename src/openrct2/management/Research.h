/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../localisation/StringIdType.h"
#include "../object/ObjectTypes.h"
#include "../ride/RideTypes.h"

#include <optional>

struct RideObjectEntry;
struct ScenerySelection;

namespace OpenRCT2
{
    struct GameState_t;
}

namespace OpenRCT2::Research
{
    enum class EntryType : uint8_t
    {
        Scenery = 0,
        Ride = 1,
    };
}

enum
{
    RESEARCH_ENTRY_FLAG_FIRST_OF_TYPE = (1 << 0),
    RESEARCH_ENTRY_FLAG_SCENERY_SET_ALWAYS_RESEARCHED = (1 << 5),
    RESEARCH_ENTRY_FLAG_RIDE_ALWAYS_RESEARCHED = (1 << 6),
};

enum class ResearchCategory : uint8_t
{
    Transport = 0,
    Gentle = 1,
    Rollercoaster = 2,
    Thrill = 3,
    Water = 4,
    Shop = 5,
    SceneryGroup = 6,
};

struct ResearchItem
{
    union
    {
        uint32_t rawValue;
        struct
        {
            ObjectEntryIndex entryIndex;
            uint8_t baseRideType;
            OpenRCT2::Research::EntryType type; // 0: scenery entry, 1: ride entry
        };
    };
    uint8_t flags;
    ResearchCategory category;

    bool IsNull() const;
    void SetNull();
    bool Exists() const;
    bool IsAlwaysResearched() const;
    StringId GetName() const;
    StringId GetCategoryInventionString() const;
    StringId GetCategoryName() const;

    ResearchItem() = default;
    constexpr ResearchItem(uint32_t _rawValue, ResearchCategory _category, uint8_t _flags)
        : rawValue(_rawValue)
        , flags(_flags)
        , category(_category)
    {
    }
    ResearchItem(
        OpenRCT2::Research::EntryType _type, ObjectEntryIndex _entryIndex, uint8_t _baseRideType, ResearchCategory _category,
        uint8_t _flags)
        : entryIndex(_entryIndex)
        , baseRideType(_baseRideType)
        , type(_type)
        , flags(_flags)
        , category(_category)
    {
    }

    bool operator==(const ResearchItem& rhs) const;
};

enum
{
    RESEARCH_FUNDING_NONE,
    RESEARCH_FUNDING_MINIMUM,
    RESEARCH_FUNDING_NORMAL,
    RESEARCH_FUNDING_MAXIMUM,

    RESEARCH_FUNDING_COUNT
};

enum
{
    RESEARCH_STAGE_INITIAL_RESEARCH,
    RESEARCH_STAGE_DESIGNING,
    RESEARCH_STAGE_COMPLETING_DESIGN,
    RESEARCH_STAGE_UNKNOWN,
    RESEARCH_STAGE_FINISHED_ALL
};

extern bool gSilentResearch;

extern const StringId kResearchFundingLevelNames[4];

void ResearchResetItems(OpenRCT2::GameState_t& gameState);
void ResearchUpdateUncompletedTypes();
void ResearchUpdate();
void ResearchResetCurrentItem();
void ResearchPopulateListRandom();

void ResearchFinishItem(const ResearchItem& researchItem);
void ResearchInsert(ResearchItem&& item, bool researched);
void ResearchRemove(const ResearchItem& researchItem);

bool ResearchInsertRideEntry(ride_type_t rideType, ObjectEntryIndex entryIndex, ResearchCategory category, bool researched);
void ResearchInsertRideEntry(ObjectEntryIndex entryIndex, bool researched);
bool ResearchInsertSceneryGroupEntry(ObjectEntryIndex entryIndex, bool researched);

bool ResearchIsInvented(ObjectType objectType, ObjectEntryIndex index);
void RideTypeSetInvented(ride_type_t rideType);
void RideEntrySetInvented(ObjectEntryIndex rideEntryIndex);
void ScenerySetInvented(const ScenerySelection& sceneryItem);
void ScenerySetNotInvented(const ScenerySelection& sceneryItem);
bool RideTypeIsInvented(ride_type_t rideType);
bool RideEntryIsInvented(ObjectEntryIndex rideEntryIndex);
bool SceneryGroupIsInvented(int32_t sgIndex);
void SceneryGroupSetInvented(int32_t sgIndex);
bool SceneryIsInvented(const ScenerySelection& sceneryItem);
void SetAllSceneryItemsInvented();
void SetAllSceneryItemsNotInvented();
void SetAllSceneryGroupsNotInvented();
void SetEveryRideTypeInvented();
void SetEveryRideTypeNotInvented();
void SetEveryRideEntryInvented();
void SetEveryRideEntryNotInvented();
void ResearchRemoveFlags();
void ResearchFix();

void ResearchItemsMakeAllUnresearched();
void ResearchItemsMakeAllResearched();
void ResearchItemsShuffle();
/**
 * Determines if a newly invented ride entry should be listed as a new ride
 * or as a new vehicle for a pre-existing ride.
 */
void ResearchDetermineFirstOfType();
