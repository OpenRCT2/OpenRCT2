#include "../drawing/drawing.h"
#include "colour.h"

rct_colour_map_a *ColourMapA = (rct_colour_map_a*)0x0141FC44;
rct_colour_map_b *ColourMapB = (rct_colour_map_b*)0x0141FD44;

void colours_init_maps()
{
	// Get colour maps from g1
	for (int i = 0; i < 32; i++) {
		rct_g1_element *g1Element = &g1Elements[4915 + i];

		uint8 *dstA = (uint8*)&ColourMapA[i];
		uint8 *dstB = (uint8*)&ColourMapB[i];
		for (int j = 0; j < 4; j++) {
			dstA[j] = g1Element->offset[0xF5 + j];
		}
		for (int j = 0; j < 4; j++) {
			dstA[j + 4] = g1Element->offset[0xF9 + j];
		}
		for (int j = 0; j < 4; j++) {
			dstB[j] = g1Element->offset[0xFD + j];
		}
	}
}
