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

// RCT
const utf8 * const ScenarioTitlesRCT1[] = {
	"Forest Frontiers",
	"Dynamite Dunes",
	"Leafy Lake",
	"Diamond Heights",
	"Evergreen Gardens",
	"Bumbly Beach",
	"Trinity Islands",
	"Katie's Dreamland",
	"Pokey Park",
	"White Water Park",
	"Millennium Mines",
	"Karts & Coasters",
	"Mel's World",
	"Mystic Mountain",
	"Pacific Pyramids",
	"Crumbly Woods",
	"Paradise Pier",
	"Lightning Peaks",
	"Ivory Towers",
	"Rainbow Valley",
	"Thunder Rock",
	"Mega Park",
};

// RCT: Added Attractions
const utf8 * const ScenarioTitlesRCT1AA[] = {
	"Whispering Cliffs",
	"Three Monkeys Park",
	"Canary Mines",
	"Barony Bridge",
	"Funtopia",
	"Haunted Harbor",
	"Fun Fortress",
	"Future World",
	"Gentle Glen",
	"Jolly Jungle",
	"Hydro Hills",
	"Sprightly Park",
	"Magic Quarters",
	"Fruit Farm",
	"Butterfly Dam",
	"Coaster Canyon",
	"Thunderstorm Park",
	"Harmonic Hills",
	"Roman Village",
	"Swamp Cove",
	"Adrenaline Heights",
	"Utopia",
	"Rotting Heights",
	"Fiasco Forest",
	"Pickle Park",
	"Giggle Downs",
	"Mineral Park",
	"Coaster Crazy",
	"Urban Park",
	"Geoffrey Gardens",
};

// RCT: Loopy Landscapes
const utf8 * const ScenarioTitlesRCT1LL[] = {
	"Iceberg Islands",
	"Volcania",
	"Arid Heights",
	"Razor Rocks",
	"Crater Lake",
	"Vertigo Views",
	"Paradise Pier 2",
	"Dragon's Cove",
	"Good Knight Park",
	"Wacky Warren",
	"Grand Glacier",
	"Crazy Craters",
	"Dusty Desert",
	"Woodworm Park",
	"Icarus Park",
	"Sunny Swamps",
	"Frightmare Hills",
	"Thunder Rocks",
	"Octagon Park",
	"Pleasure Island",
	"Icicle Worlds",
	"Tiny Towers",
	"Southern Sands",
	"Nevermore Park",
	"Pacifica",
	"Urban Jungle",
	"Terror Town",
	"Megaworld Park",
	"Venus Ponds",
	"Micro Park",
};

// RCT2
const utf8 * const ScenarioTitlesRCT2[] = {
	"Crazy Castle",
	"Electric Fields",
	"Factory Capers",
	"Amity Airfield",
	"Botany Breakers",
	"Bumbly Bazaar",
	"Dusty Greens",
	"Fungus Woods",
	"Gravity Gardens",
	"Infernal Views",
	"Alpine Adventures",
	"Extreme Heights",
	"Ghost Town",
	"Lucky Lake",
	"Rainbow Summit",
};

// RCT2: Wacky Worlds
const utf8 * const ScenarioTitlesRCT2WW[] = {
	"Africa - Victoria Falls",
	"Asia - Great Wall of China Tourism Enhancement",
	"North America - Grand Canyon",
	"South America - Rio Carnival",
	"Africa - African Diamond Mine",
	"Asia - Maharaja Palace",
	"Australasia - Ayers Rock",
	"Europe - European Cultural Festival",
	"North America - Rollercoaster Heaven",
	"South America - Inca Lost City",
	"Africa - Oasis",
	"Antarctic - Ecological Salvage",
	"Asia - Japanese Coastal Reclaim",
	"Australasia - Fun at the Beach",
	"Europe - Renovation",
	"N. America - Extreme Hawaiian Island",
	"South America - Rain Forest Plateau",
};

// RCT2: Time Twister
const utf8 * const ScenarioTitlesRCT2TT[] = {
	"Dark Age - Robin Hood",
	"Prehistoric - After the Asteroid",
	"Roaring Twenties - Prison Island",
	"Rock 'n' Roll - Flower Power",
	"Dark Age - Castle",
	"Future - First Encounters",
	"Mythological - Animatronic Film Set",
	"Prehistoric - Jurassic Safari",
	"Roaring Twenties - Schneider Cup",
	"Future - Future World",
	"Mythological - Cradle of Civilisation",
	"Prehistoric - Stone Age",
	"Roaring Twenties - Skyscrapers",
	"Rock 'n' Roll - Rock 'n' Roll",
};

// Real parks
const utf8 * const ScenarioTitlesRealParks[] = {
	"Alton Towers",
	"Heide-Park",
	"Blackpool Pleasure Beach",
	"Six Flags Belgium",
	"Six Flags Great Adventure",
	"Six Flags Holland",
	"Six Flags Magic Mountain",
	"Six Flags over Texas",
};

// Other parks
const utf8 * const ScenarioTitlesRCT2BuildYourOwnParks[] = {
	"Build your own Six Flags Belgium",
	"Build your own Six Flags Great Adventure",
	"Build your own Six Flags Holland",
	"Build your own Six Flags Magic Mountain",
	"Build your own Six Flags Park",
	"Build your own Six Flags over Texas",
};

const struct {
	int count;
	const utf8 * const * titles;
} ScenarioTitlesBySource[] = {
	{ countof(ScenarioTitlesRCT1),						ScenarioTitlesRCT1				},
	{ countof(ScenarioTitlesRCT1AA),					ScenarioTitlesRCT1AA			},
	{ countof(ScenarioTitlesRCT1LL),					ScenarioTitlesRCT1LL			},
	{ countof(ScenarioTitlesRCT2),						ScenarioTitlesRCT2				},
	{ countof(ScenarioTitlesRCT2WW),					ScenarioTitlesRCT2WW			},
	{ countof(ScenarioTitlesRCT2TT),					ScenarioTitlesRCT2TT			},
	{ countof(ScenarioTitlesRealParks),					ScenarioTitlesRealParks			},
};

bool scenario_get_index_and_source(const utf8 *name, scenario_source *source, sint32 *index)
{
	sint32 currentIndex = 0;
	for (int i = 0; i < countof(ScenarioTitlesBySource); i++) {
		for (int j = 0; j < ScenarioTitlesBySource[i].count; j++) {
			if (_strcmpi(name, ScenarioTitlesBySource[i].titles[j]) == 0) {
				*source = (scenario_source)i;
				*index = currentIndex;
				return true;
			}
			currentIndex++;
		}
	}

	*source = SCENARIO_SOURCE_OTHER;
	*index = -1;
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
