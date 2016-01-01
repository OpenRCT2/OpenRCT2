#include "scenario.h"
#include "util/util.h"

typedef struct {
	const utf8 *original;
	const utf8 *alternative;
} scenario_alias;

const scenario_alias ScenarioAliases[] = {
	{ "Katie's Dreamland",		"Katie's World"			},
	{ "Pokey Park",				"Dinky Park"			},
	{ "White Water Park",		"Aqua Park"				},
	{ "Mystic Mountain",		"Mothball Mountain"		},
	{ "Paradise Pier",			"Big Pier"				},
	{ "Paradise Pier 2",		"Big Pier 2"			},
};

typedef struct {
	const utf8 *title;
	const uint8 category;
} scenario_title_desc;

// RCT
const scenario_title_desc ScenarioTitlesRCT1[] = {
	{ "Forest Frontiers",		SCENARIO_CATEGORY_BEGINNER		},
	{ "Dynamite Dunes",			SCENARIO_CATEGORY_BEGINNER		},
	{ "Leafy Lake",				SCENARIO_CATEGORY_BEGINNER		},
	{ "Diamond Heights",		SCENARIO_CATEGORY_BEGINNER		},
	{ "Evergreen Gardens",		SCENARIO_CATEGORY_BEGINNER		},
	{ "Bumbly Beach",			SCENARIO_CATEGORY_BEGINNER		},
	{ "Trinity Islands",		SCENARIO_CATEGORY_CHALLENGING	},
	{ "Katie's Dreamland",		SCENARIO_CATEGORY_CHALLENGING	},
	{ "Pokey Park",				SCENARIO_CATEGORY_CHALLENGING	},
	{ "White Water Park",		SCENARIO_CATEGORY_CHALLENGING	},
	{ "Millennium Mines",		SCENARIO_CATEGORY_CHALLENGING	},
	{ "Karts & Coasters",		SCENARIO_CATEGORY_CHALLENGING	},
	{ "Mel's World",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Mystic Mountain",		SCENARIO_CATEGORY_CHALLENGING	},
	{ "Pacific Pyramids",		SCENARIO_CATEGORY_CHALLENGING	},
	{ "Crumbly Woods",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Paradise Pier",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Lightning Peaks",		SCENARIO_CATEGORY_EXPERT		},
	{ "Ivory Towers",			SCENARIO_CATEGORY_EXPERT		},
	{ "Rainbow Valley",			SCENARIO_CATEGORY_EXPERT		},
	{ "Thunder Rock",			SCENARIO_CATEGORY_EXPERT		},
	{ "Mega Park",				SCENARIO_CATEGORY_OTHER			},
};

// RCT: Added Attractions
const scenario_title_desc ScenarioTitlesRCT1AA[] = {
	{ "Whispering Cliffs",		SCENARIO_CATEGORY_BEGINNER		},
	{ "Three Monkeys Park",		SCENARIO_CATEGORY_BEGINNER		},
	{ "Canary Mines",			SCENARIO_CATEGORY_BEGINNER		},
	{ "Barony Bridge",			SCENARIO_CATEGORY_BEGINNER		},
	{ "Funtopia",				SCENARIO_CATEGORY_BEGINNER		},
	{ "Haunted Harbor",			SCENARIO_CATEGORY_BEGINNER		},
	{ "Fun Fortress",			SCENARIO_CATEGORY_BEGINNER		},
	{ "Future World",			SCENARIO_CATEGORY_BEGINNER		},
	{ "Gentle Glen",			SCENARIO_CATEGORY_BEGINNER		},
	{ "Jolly Jungle",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Hydro Hills",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Sprightly Park",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Magic Quarters",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Fruit Farm",				SCENARIO_CATEGORY_CHALLENGING	},
	{ "Butterfly Dam",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Coaster Canyon",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Thunderstorm Park",		SCENARIO_CATEGORY_CHALLENGING	},
	{ "Harmonic Hills",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Roman Village",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Swamp Cove",				SCENARIO_CATEGORY_CHALLENGING	},
	{ "Adrenaline Heights",		SCENARIO_CATEGORY_CHALLENGING	},
	{ "Utopia",					SCENARIO_CATEGORY_CHALLENGING	},
	{ "Rotting Heights",		SCENARIO_CATEGORY_EXPERT		},
	{ "Fiasco Forest",			SCENARIO_CATEGORY_EXPERT		},
	{ "Pickle Park",			SCENARIO_CATEGORY_EXPERT		},
	{ "Giggle Downs",			SCENARIO_CATEGORY_EXPERT		},
	{ "Mineral Park",			SCENARIO_CATEGORY_EXPERT		},
	{ "Coaster Crazy",			SCENARIO_CATEGORY_EXPERT		},
	{ "Urban Park",				SCENARIO_CATEGORY_EXPERT		},
	{ "Geoffrey Gardens",		SCENARIO_CATEGORY_EXPERT		},
};

// RCT: Loopy Landscapes
const scenario_title_desc ScenarioTitlesRCT1LL[] = {
	{ "Iceberg Islands",		SCENARIO_CATEGORY_BEGINNER		},
	{ "Volcania",				SCENARIO_CATEGORY_BEGINNER		},
	{ "Arid Heights",			SCENARIO_CATEGORY_BEGINNER		},
	{ "Razor Rocks",			SCENARIO_CATEGORY_BEGINNER		},
	{ "Crater Lake",			SCENARIO_CATEGORY_BEGINNER		},
	{ "Vertigo Views",			SCENARIO_CATEGORY_BEGINNER		},
	{ "Paradise Pier 2",		SCENARIO_CATEGORY_CHALLENGING	},
	{ "Dragon's Cove",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Good Knight Park",		SCENARIO_CATEGORY_CHALLENGING	},
	{ "Wacky Warren",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Grand Glacier",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Crazy Craters",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Dusty Desert",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Woodworm Park",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Icarus Park",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Sunny Swamps",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Frightmare Hills",		SCENARIO_CATEGORY_CHALLENGING	},
	{ "Thunder Rocks",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Octagon Park",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Pleasure Island",		SCENARIO_CATEGORY_CHALLENGING	},
	{ "Icicle Worlds",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Tiny Towers",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Southern Sands",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Nevermore Park",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Pacifica",				SCENARIO_CATEGORY_CHALLENGING	},
	{ "Urban Jungle",			SCENARIO_CATEGORY_EXPERT		},
	{ "Terror Town",			SCENARIO_CATEGORY_EXPERT		},
	{ "Megaworld Park",			SCENARIO_CATEGORY_EXPERT		},
	{ "Venus Ponds",			SCENARIO_CATEGORY_EXPERT		},
	{ "Micro Park",				SCENARIO_CATEGORY_EXPERT		},
};

// RCT2
const scenario_title_desc ScenarioTitlesRCT2[] = {
	{ "Crazy Castle",			SCENARIO_CATEGORY_BEGINNER		},
	{ "Electric Fields",		SCENARIO_CATEGORY_BEGINNER		},
	{ "Factory Capers",			SCENARIO_CATEGORY_BEGINNER		},
	{ "Amity Airfield",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Botany Breakers",		SCENARIO_CATEGORY_CHALLENGING	},
	{ "Bumbly Bazaar",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Dusty Greens",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Fungus Woods",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Gravity Gardens",		SCENARIO_CATEGORY_CHALLENGING	},
	{ "Infernal Views",			SCENARIO_CATEGORY_CHALLENGING	},
	{ "Alpine Adventures",		SCENARIO_CATEGORY_EXPERT		},
	{ "Extreme Heights",		SCENARIO_CATEGORY_EXPERT		},
	{ "Ghost Town",				SCENARIO_CATEGORY_EXPERT		},
	{ "Lucky Lake",				SCENARIO_CATEGORY_EXPERT		},
	{ "Rainbow Summit",			SCENARIO_CATEGORY_EXPERT		},
};

// RCT2: Wacky Worlds
const scenario_title_desc ScenarioTitlesRCT2WW[] = {
	{ "Africa - Victoria Falls",							SCENARIO_CATEGORY_BEGINNER		},
	{ "Asia - Great Wall of China Tourism Enhancement",		SCENARIO_CATEGORY_BEGINNER		},
	{ "North America - Grand Canyon",						SCENARIO_CATEGORY_BEGINNER		},
	{ "South America - Rio Carnival",						SCENARIO_CATEGORY_BEGINNER		},
	{ "Africa - African Diamond Mine",						SCENARIO_CATEGORY_CHALLENGING	},
	{ "Asia - Maharaja Palace",								SCENARIO_CATEGORY_CHALLENGING	},
	{ "Australasia - Ayers Rock",							SCENARIO_CATEGORY_CHALLENGING	},
	{ "Europe - European Cultural Festival",				SCENARIO_CATEGORY_CHALLENGING	},
	{ "North America - Rollercoaster Heaven",				SCENARIO_CATEGORY_CHALLENGING	},
	{ "South America - Inca Lost City",						SCENARIO_CATEGORY_CHALLENGING	},
	{ "Africa - Oasis",										SCENARIO_CATEGORY_EXPERT		},
	{ "Antarctic - Ecological Salvage",						SCENARIO_CATEGORY_EXPERT		},
	{ "Asia - Japanese Coastal Reclaim",					SCENARIO_CATEGORY_EXPERT		},
	{ "Australasia - Fun at the Beach",						SCENARIO_CATEGORY_EXPERT		},
	{ "Europe - Renovation",								SCENARIO_CATEGORY_EXPERT		},
	{ "N. America - Extreme Hawaiian Island",				SCENARIO_CATEGORY_EXPERT		},
	{ "South America - Rain Forest Plateau",				SCENARIO_CATEGORY_EXPERT		},
};

// RCT2: Time Twister
const scenario_title_desc ScenarioTitlesRCT2TT[] = {
	{ "Dark Age - Robin Hood",								SCENARIO_CATEGORY_BEGINNER		},
	{ "Prehistoric - After the Asteroid",					SCENARIO_CATEGORY_BEGINNER		},
	{ "Roaring Twenties - Prison Island",					SCENARIO_CATEGORY_BEGINNER		},
	{ "Rock 'n' Roll - Flower Power",						SCENARIO_CATEGORY_BEGINNER		},
	{ "Dark Age - Castle",									SCENARIO_CATEGORY_CHALLENGING	},
	{ "Future - First Encounters",							SCENARIO_CATEGORY_CHALLENGING	},
	{ "Mythological - Animatronic Film Set",				SCENARIO_CATEGORY_CHALLENGING	},
	{ "Prehistoric - Jurassic Safari",						SCENARIO_CATEGORY_CHALLENGING	},
	{ "Roaring Twenties - Schneider Cup",					SCENARIO_CATEGORY_CHALLENGING	},
	{ "Future - Future World",								SCENARIO_CATEGORY_EXPERT		},
	{ "Mythological - Cradle of Civilisation",				SCENARIO_CATEGORY_EXPERT		},
	{ "Prehistoric - Stone Age",							SCENARIO_CATEGORY_EXPERT		},
	{ "Roaring Twenties - Skyscrapers",						SCENARIO_CATEGORY_EXPERT		},
	{ "Rock 'n' Roll - Rock 'n' Roll",						SCENARIO_CATEGORY_EXPERT		},
};

// Real parks
const scenario_title_desc ScenarioTitlesRealParks[] = {
	{ "Alton Towers",				SCENARIO_CATEGORY_REAL	},
	{ "Heide-Park",					SCENARIO_CATEGORY_REAL	},
	{ "Blackpool Pleasure Beach",	SCENARIO_CATEGORY_REAL	},
	{ "Six Flags Belgium",			SCENARIO_CATEGORY_REAL	},
	{ "Six Flags Great Adventure",	SCENARIO_CATEGORY_REAL	},
	{ "Six Flags Holland",			SCENARIO_CATEGORY_REAL	},
	{ "Six Flags Magic Mountain",	SCENARIO_CATEGORY_REAL	},
	{ "Six Flags over Texas",		SCENARIO_CATEGORY_REAL	},
};

// Other parks
const scenario_title_desc ScenarioTitlesRCT2BuildYourOwnParks[] = {
	{ "Build your own Six Flags Belgium",			SCENARIO_CATEGORY_OTHER	},
	{ "Build your own Six Flags Great Adventure",	SCENARIO_CATEGORY_OTHER	},
	{ "Build your own Six Flags Holland",			SCENARIO_CATEGORY_OTHER	},
	{ "Build your own Six Flags Magic Mountain",	SCENARIO_CATEGORY_OTHER	},
	{ "Build your own Six Flags Park",				SCENARIO_CATEGORY_OTHER	},
	{ "Build your own Six Flags over Texas",		SCENARIO_CATEGORY_OTHER	},
};

const struct {
	int count;
	const scenario_title_desc * const titles;
} ScenarioTitlesBySource[] = {
	{ countof(ScenarioTitlesRCT1),						ScenarioTitlesRCT1				},
	{ countof(ScenarioTitlesRCT1AA),					ScenarioTitlesRCT1AA			},
	{ countof(ScenarioTitlesRCT1LL),					ScenarioTitlesRCT1LL			},
	{ countof(ScenarioTitlesRCT2),						ScenarioTitlesRCT2				},
	{ countof(ScenarioTitlesRCT2WW),					ScenarioTitlesRCT2WW			},
	{ countof(ScenarioTitlesRCT2TT),					ScenarioTitlesRCT2TT			},
	{ countof(ScenarioTitlesRealParks),					ScenarioTitlesRealParks			},
};

bool scenario_get_source_desc(const utf8 *name, source_desc *outDesc)
{
	assert(outDesc != NULL);

	sint32 currentIndex = 0;
	for (int i = 0; i < countof(ScenarioTitlesBySource); i++) {
		for (int j = 0; j < ScenarioTitlesBySource[i].count; j++) {
			const scenario_title_desc *desc = &ScenarioTitlesBySource[i].titles[j];
			if (_strcmpi(name, desc->title) == 0) {
				outDesc->source = i;
				outDesc->index = currentIndex;
				outDesc->category = desc->category;
				return true;
			}
			currentIndex++;
		}
	}

	outDesc->source = SCENARIO_SOURCE_OTHER;
	outDesc->index = -1;
	outDesc->category = SCENARIO_CATEGORY_OTHER;
	return false;
}

void scenario_normalise_name(utf8 *name)
{
	size_t nameLength = strlen(name);

	// Strip "RCT(1|2)?" prefix off scenario names.
	if (nameLength >= 3 && (name[0] == 'R' && name[1] == 'C' && name[2] == 'T')) {
		if (nameLength >= 4 && (name[3] == '1' || name[3] == '2')) {
			log_verbose("Stripping RCT/1/2 from name: %s", name);
			safe_strncpy(name, name + 4, 64);
		} else {
			safe_strncpy(name, name + 3, 64);
		}
	}
	
	// Trim (for the sake of the above and WW / TT scenarios
	safe_strtrimleft(name, name, 64);

	// American scenario titles should be converted to British name
	// Don't worry, names will be translated using language packs later
	for (int i = 0; i < countof(ScenarioAliases); i++) {
		if (strcmp(ScenarioAliases[i].alternative, name) == 0) {
			log_verbose("Found alias: %s; will treat as: %s", name, ScenarioAliases[i].original);
			safe_strncpy(name, ScenarioAliases[i].original, 64);
		}
	}
}
