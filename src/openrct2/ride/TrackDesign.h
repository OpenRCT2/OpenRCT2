/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../actions/GameActionResult.h"
#include "../common.h"
#include "../object/Object.h"
#include "../rct12/RCT12.h"
#include "../rct2/RCT2.h"
#include "../world/Map.h"

#include <memory>

struct Ride;

#define TRACK_PREVIEW_IMAGE_SIZE (370 * 217)

struct TrackDesignState
{
    CoordsXYZ PreviewMin;
    CoordsXYZ PreviewMax;
    CoordsXYZ Origin;
    uint8_t PlaceOperation{};
    int16_t PlaceZ{};
    int16_t PlaceSceneryZ{};
    bool EntranceExitPlaced{};
    bool HasScenery{};
    bool PlaceScenery{};
    bool IsReplay{};
};

/* Track Entrance entry */
struct TrackDesignEntranceElement
{
    int8_t z;
    uint8_t direction;
    int16_t x;
    int16_t y;
    bool isExit;
};

struct TrackDesignSceneryElement
{
    ObjectEntryDescriptor scenery_object;
    CoordsXYZ loc;
    uint8_t flags;
    uint8_t primary_colour;
    uint8_t secondary_colour;

    bool IsQueue() const
    {
        return (flags & (1 << 7)) != 0;
    }
};

/**
 * Track design structure.
 */

/* Track Element entry  size: 0x03 */
struct TrackDesignTrackElement
{
    track_type_t type; // 0x00
    uint8_t flags;     // 0x02
};

/* Maze Element entry   size: 0x04 */
struct TrackDesignMazeElement
{
    union
    {
        uint32_t all;
        struct
        {
            int8_t x;
            int8_t y;
            union
            {
                uint16_t maze_entry;
                struct
                {
                    uint8_t direction;
                    uint8_t type;
                };
            };
        };
    };
};

class DataSerialiser;
enum class RideMode : uint8_t;

struct TrackDesign
{
    uint8_t type;
    uint8_t vehicle_type;
    money32 cost;
    uint32_t flags;
    RideMode ride_mode;
    uint8_t track_flags;
    uint8_t colour_scheme;
    std::array<rct_vehicle_colour, RCT2::Limits::MaxTrainsPerRide> vehicle_colours;
    uint8_t entrance_style;
    uint8_t total_air_time;
    uint8_t depart_flags;
    uint8_t number_of_trains;
    uint8_t number_of_cars_per_train;
    uint8_t min_waiting_time;
    uint8_t max_waiting_time;
    uint8_t operation_setting;
    int8_t max_speed;
    int8_t average_speed;
    uint16_t ride_length;
    uint8_t max_positive_vertical_g;
    int8_t max_negative_vertical_g;
    uint8_t max_lateral_g;
    uint8_t inversions;
    uint8_t holes;
    uint8_t drops;
    uint8_t highest_drop_height;
    uint8_t excitement;
    uint8_t intensity;
    uint8_t nausea;
    money16 upkeep_cost;
    uint8_t track_spine_colour[RCT12::Limits::NumColourSchemes];
    uint8_t track_rail_colour[RCT12::Limits::NumColourSchemes];
    uint8_t track_support_colour[RCT12::Limits::NumColourSchemes];
    uint32_t flags2;
    ObjectEntryDescriptor vehicle_object;
    uint8_t space_required_x;
    uint8_t space_required_y;
    uint8_t vehicle_additional_colour[RCT2::Limits::MaxTrainsPerRide];
    uint8_t lift_hill_speed;
    uint8_t num_circuits;

    std::vector<TrackDesignMazeElement> maze_elements;
    std::vector<TrackDesignTrackElement> track_elements;
    std::vector<TrackDesignEntranceElement> entrance_elements;
    std::vector<TrackDesignSceneryElement> scenery_elements;

    std::string name;

public:
    StringId CreateTrackDesign(TrackDesignState& tds, const Ride& ride);
    StringId CreateTrackDesignScenery(TrackDesignState& tds);
    void Serialise(DataSerialiser& stream);

private:
    uint8_t _saveDirection;
    StringId CreateTrackDesignTrack(TrackDesignState& tds, const Ride& ride);
    StringId CreateTrackDesignMaze(TrackDesignState& tds, const Ride& ride);
    CoordsXYE MazeGetFirstElement(const Ride& ride);
};

// Only written to in RCT2, not used in OpenRCT2. All of these are elements that had to be invented in RCT1.
enum : uint32_t
{
    TRACK_FLAGS_CONTAINS_VERTICAL_LOOP = (1 << 7),
    TRACK_FLAGS_CONTAINS_INLINE_TWIST = (1 << 17),
    TRACK_FLAGS_CONTAINS_HALF_LOOP = (1 << 18),
    TRACK_FLAGS_CONTAINS_CORKSCREW = (1 << 19),
    TRACK_FLAGS_CONTAINS_WATER_SPLASH = (1 << 27),
    TRACK_FLAGS_CONTAINS_BARREL_ROLL = (1 << 29),
    TRACK_FLAGS_CONTAINS_POWERED_LIFT = (1 << 30),
    TRACK_FLAGS_CONTAINS_LARGE_HALF_LOOP = (1u << 31),
};

enum : uint32_t
{
    TRACK_FLAGS2_CONTAINS_LOG_FLUME_REVERSER = (1 << 1),
    TRACK_FLAGS2_SIX_FLAGS_RIDE_DEPRECATED = (1u << 31) // Not used anymore.
};

enum
{
    TDPF_PLACE_SCENERY = 1 << 0,
};

enum
{
    TRACK_DESIGN_FLAG_SCENERY_UNAVAILABLE = (1 << 0),
    TRACK_DESIGN_FLAG_HAS_SCENERY = (1 << 1),
    TRACK_DESIGN_FLAG_VEHICLE_UNAVAILABLE = (1 << 2),
};

enum
{
    PTD_OPERATION_DRAW_OUTLINES,
    PTD_OPERATION_PLACE_QUERY,
    PTD_OPERATION_PLACE,
    PTD_OPERATION_GET_PLACE_Z,
    PTD_OPERATION_PLACE_GHOST,
    PTD_OPERATION_PLACE_TRACK_PREVIEW,
    PTD_OPERATION_REMOVE_GHOST,
};

static constexpr uint8_t PTD_OPERATION_FLAG_IS_REPLAY = (1 << 7);

enum
{
    MAZE_ELEMENT_TYPE_MAZE_TRACK = 0,
    MAZE_ELEMENT_TYPE_ENTRANCE = (1 << 3),
    MAZE_ELEMENT_TYPE_EXIT = (1 << 7)
};

static constexpr RideId PreviewRideId = RideId::FromUnderlying(0);

extern bool gTrackDesignSceneryToggle;

extern bool _trackDesignDrawingPreview;

extern bool _trackDesignPlaceStateSceneryUnavailable;
extern bool gTrackDesignSaveMode;
extern RideId gTrackDesignSaveRideIndex;

[[nodiscard]] std::unique_ptr<TrackDesign> TrackDesignImport(const utf8* path);

void TrackDesignMirror(TrackDesign* td6);

GameActions::Result TrackDesignPlace(TrackDesign* td6, uint32_t flags, bool placeScenery, Ride* ride, const CoordsXYZD& coords);
void TrackDesignPreviewRemoveGhosts(TrackDesign* td6, Ride* ride, const CoordsXYZD& coords);
void TrackDesignPreviewDrawOutlines(TrackDesignState& tds, TrackDesign* td6, Ride* ride, const CoordsXYZD& coords);
int32_t TrackDesignGetZPlacement(TrackDesign* td6, Ride* ride, const CoordsXYZD& coords);

///////////////////////////////////////////////////////////////////////////////
// Track design preview
///////////////////////////////////////////////////////////////////////////////
void TrackDesignDrawPreview(TrackDesign* td6, uint8_t* pixels);

///////////////////////////////////////////////////////////////////////////////
// Track design saving
///////////////////////////////////////////////////////////////////////////////
void track_design_save_init();
void track_design_save_reset_scenery();
bool track_design_save_contains_tile_element(const TileElement* tileElement);
void track_design_save_select_nearby_scenery(RideId rideIndex);
void track_design_save_select_tile_element(
    ViewportInteractionItem interactionType, const CoordsXY& loc, TileElement* tileElement, bool collect);

bool track_design_are_entrance_and_exit_placed();

extern std::vector<TrackDesignSceneryElement> _trackSavedTileElementsDesc;
extern std::vector<const TileElement*> _trackSavedTileElements;
