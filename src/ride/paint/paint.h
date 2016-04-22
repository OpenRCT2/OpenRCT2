#ifndef _RIDE_PAINT_H_H
#define _RIDE_PAINT_H_H

#include "../track_paint.h"

namespace Facility { void Draw(rct_ride * ride, uint8 trackType, uint8 trackSequence, uint8 direction, rct_map_element *mapElement, TileDrawingContext * context); }
namespace Maze { void Draw(rct_ride * ride, uint8 trackType, uint8 trackSequence, uint8 direction, rct_map_element *mapElement, TileDrawingContext * context); }
namespace Shop { void Draw(rct_ride * ride, uint8 trackType, uint8 trackSequence, uint8 direction, rct_map_element *mapElement, TileDrawingContext * context); }
namespace TopSpin { void Draw(rct_ride * ride, uint8 trackType, uint8 trackSequence, uint8 direction, rct_map_element *mapElement, TileDrawingContext * context); }

#endif
