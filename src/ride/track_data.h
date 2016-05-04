#include "../common.h"
#include "track.h"
#include "track_paint.h"

// 0x009968BB, 0x009968BC, 0x009968BD, 0x009968BF, 0x009968C1, 0x009968C3
extern const rct_track_coordinates TrackCoordinates[256];
// 0x009972BB, 0x009972BC, 0x009972BD, 0x009972BF, 0x009972C1, 0x009972C3
extern const rct_track_coordinates* FlatTrackCoordinates;

extern const uint64 RideTypePossibleTrackConfigurations[91];
extern const rct_preview_track *TrackBlocks[256];
extern const rct_preview_track *FlatRideTrackBlocks[255];

extern const uint8 TrackPieceLengths[256];

typedef struct {
	uint16 next;
	uint16 previous;
} track_curve_chain;

extern const track_curve_chain gTrackCurveChain[256];
extern const track_curve_chain gFlatRideTrackCurveChain[256];

extern const TRACK_PAINT_FUNCTION_GETTER RideTypeTrackPaintFunctions[91];
extern const uint32 RideTypeTrackPaintFunctionsOld[91];

typedef struct {
	bool starts_diagonal;
	uint8 slope_start;
	uint8 bank_start;
	uint8 track_curve;
	uint8 slope_end;
	uint8 bank_end;
	uint8 track_element;
} track_descriptor;

extern const track_descriptor gTrackDescriptors[142];
