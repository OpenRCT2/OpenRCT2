#ifndef _MAPGEN_H_
#define _MAPGEN_H_

typedef struct {
	// Base
	int mapSize;
	int height;
	int waterLevel;
	int floor;
	int wall;

	// Features (e.g. tree, rivers, lakes etc.)
	int trees;

	// Simplex Noise Parameters
	int simplex_low;
	int simplex_high;
	float simplex_base_freq;
	int simplex_octaves;
} mapgen_settings;

void mapgen_generate_blank(mapgen_settings *settings);
void mapgen_generate(mapgen_settings *settings);
void mapgen_generate_custom_simplex(mapgen_settings *settings);

#endif
