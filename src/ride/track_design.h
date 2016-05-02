#ifndef _TRACK_DESIGN_H_
#define _TRACK_DESIGN_H_

#include "../common.h"
#include "../object.h"
#include "../world/map.h"
#include "vehicle.h"

#define TRACK_PREVIEW_IMAGE_SIZE (370 * 217)

/* Maze Element entry   size: 0x04 */
typedef struct {
	union {
		uint32 all;
		struct {
			sint8 x;
			sint8 y;
			union {
				uint16 maze_entry;
				struct{
					uint8 unk_2;
					uint8 type;
				};
			};
		};
	};
} rct_td6_maze_element;

/* Track Element entry  size: 0x02 */
typedef struct {
	uint8 type;							// 0x00
	uint8 flags;						// 0x01
} rct_td6_track_element;

/* Track Entrance entry size: 0x06 */
typedef struct {
	sint8 z;							// 0x00
	uint8 direction;					// 0x01
	sint16 x;							// 0x02
	sint16 y;							// 0x04
} rct_td6_entrance_element;

/* Track Scenery entry  size: 0x16 */
typedef struct {
	rct_object_entry scenery_object;	// 0x00
	sint8 x;							// 0x10
	sint8 y;							// 0x11
	sint8 z;							// 0x12
	uint8 flags;						// 0x13 direction quadrant tertiary colour
	uint8 primary_colour;				// 0x14
	uint8 secondary_colour;				// 0x15
} rct_td6_scenery_element;

/**
 * Track design structure.
 * size: 0x4E72B
 */
typedef struct {
	uint8 type;										// 0x00
	uint8 vehicle_type;
	union{
		// After loading the track this is converted to
		// a cost but before its a flags register
		money32 cost;								// 0x02
		uint32 flags;								// 0x02
	};
	union{
		// After loading the track this is converted to
		// a flags register
		uint8 ride_mode;							// 0x06
		uint8 track_flags;							// 0x06
	};
	uint8 version_and_colour_scheme;				// 0x07 0b0000_VVCC
	rct_vehicle_colour vehicle_colours[32];	// 0x08
	union{
		uint8 pad_48;
		uint8 track_spine_colour_rct1;				// 0x48
	};
	union{
		uint8 entrance_style;						// 0x49
		uint8 track_rail_colour_rct1;				// 0x49
	};
	union{
		uint8 total_air_time;						// 0x4A
		uint8 track_support_colour_rct1;			// 0x4A
	};
	uint8 depart_flags;								// 0x4B
	uint8 number_of_trains;							// 0x4C
	uint8 number_of_cars_per_train;					// 0x4D
	uint8 min_waiting_time;							// 0x4E
	uint8 max_waiting_time;							// 0x4F
	uint8 var_50;
	sint8 max_speed;								// 0x51
	sint8 average_speed;							// 0x52
	uint16 ride_length;								// 0x53
	uint8 max_positive_vertical_g;					// 0x55
	sint8 max_negative_vertical_g;					// 0x56
	uint8 max_lateral_g;							// 0x57
	union {
		uint8 inversions;							// 0x58
		uint8 holes;								// 0x58
	};
	uint8 drops;									// 0x59
	uint8 highest_drop_height;						// 0x5A
	uint8 excitement;								// 0x5B
	uint8 intensity;								// 0x5C
	uint8 nausea;									// 0x5D
	money16 upkeep_cost;							// 0x5E
	uint8 track_spine_colour[4];					// 0x60
	uint8 track_rail_colour[4];						// 0x64
	uint8 track_support_colour[4];					// 0x68
	uint32 flags2;									// 0x6C
	rct_object_entry vehicle_object;				// 0x70
	uint8 space_required_x;							// 0x80
	uint8 space_required_y;							// 0x81
	uint8 vehicle_additional_colour[32];			// 0x82
	uint8 lift_hill_speed_num_circuits;				// 0xA2 0bCCCL_LLLL
	void *elements;									// 0xA3 (data starts here in file)
	size_t elementsSize;

	rct_td6_maze_element		*maze_elements;
	rct_td6_track_element		*track_elements;
	rct_td6_entrance_element	*entrance_elements;
	rct_td6_scenery_element		*scenery_elements;

	utf8 *name;
} rct_track_td6;

typedef struct{
	rct_track_td6 track_td6;
	uint8 preview[4][TRACK_PREVIEW_IMAGE_SIZE];		// 0xA3
} rct_track_design;

typedef struct {
	utf8 *name;
	utf8 *path;
} track_design_file_ref;

enum {
	TRACK_FLAGS2_CONTAINS_LOG_FLUME_REVERSER = (1 << 1),
	TRACK_FLAGS2_SIX_FLAGS_RIDE_DEPRECATED = (1 << 31)		// Not used anymore.
};

enum {
	TDPF_PLACE_SCENERY = 1 << 0,
};

enum {
	PTD_OPERATION_DRAW_OUTLINES,
	PTD_OPERATION_1,
	PTD_OPERATION_2,
	PTD_OPERATION_GET_PLACE_Z,
	PTD_OPERATION_4,
	PTD_OPERATION_GET_COST,
	PTD_OPERATION_CLEAR_OUTLINES
};

extern rct_map_element **gTrackSavedMapElements;

extern rct_track_td6 *gActiveTrackDesign;
extern money32 gTrackDesignCost;
extern uint8 gTrackDesignPlaceFlags;
extern bool gTrackDesignSceneryToggle;
extern rct_xyz16 gTrackPreviewMin;
extern rct_xyz16 gTrackPreviewMax;

rct_track_design *temp_track_get_info(char* path, uint8** preview);
void track_design_mirror(rct_track_td6 *td6);
rct_track_td6 *track_design_open(const utf8 *path);
void track_design_dispose(rct_track_td6 *td6);

int sub_6D01B3(rct_track_td6 *td6, uint8 bl, uint8 rideIndex, int x, int y, int z);

void track_design_index_create();
size_t track_design_index_get_count_for_ride(uint8 rideType, const char *entry);
size_t track_design_index_get_for_ride(track_design_file_ref **tdRefs, uint8 rideType, const char *entry);
utf8 *track_design_get_name_from_path(const utf8 *path);
bool track_design_index_rename(const utf8 *path, const utf8 *newName);
bool track_design_index_delete(const utf8 *path);
bool track_design_index_install(const utf8 *srcPath, const utf8 *destPath);

void game_command_place_track_design(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);
void game_command_place_maze_design(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);

///////////////////////////////////////////////////////////////////////////////
// Track design preview
///////////////////////////////////////////////////////////////////////////////
void draw_track_preview(rct_track_td6 *td6, uint8** preview);

///////////////////////////////////////////////////////////////////////////////
// Track design saving
///////////////////////////////////////////////////////////////////////////////
void track_design_save_reset_scenery();
void track_design_save_select_nearby_scenery(int rideIndex);
void track_design_save_toggle_map_element(int interactionType, int x, int y, rct_map_element *mapElement);
bool track_design_save(uint8 rideIndex);

#endif
