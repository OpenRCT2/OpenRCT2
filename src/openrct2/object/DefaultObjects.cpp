/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "DefaultObjects.h"

#include "Object.h"

const std::string_view MinimumRequiredObjects[] = { "rct2.surface.grass", "rct2.edge.rock" };

const std::string_view DefaultSelectedObjects[] = {
    // An initial default selection
    "rct2.scgtrees", // Scenery: Trees
    "rct2.scgshrub", // Scenery: Shrubs and Ornaments
    "rct2.scggardn", // Scenery: Gardens
    "rct2.scgfence", // Scenery: Fences and Walls
    "rct2.scgwalls", // Scenery: Walls and Roofs
    "rct2.scgpathx", // Scenery: Signs and Items for Footpaths
    "rct2.tarmac",   // Footpath: Tarmac
    "rct2.twist1",   // Ride: Twist
    "rct2.ptct1",    // Ride: Wooden Roller Coaster (Wooden Roller Coaster Trains)
    "rct2.zldb",     // Ride: Junior Roller Coaster (Ladybird Trains)
    "rct2.lfb1",     // Ride: Log Flume
    "rct2.vcr",      // Ride: Vintage Cars
    "rct2.mgr1",     // Ride: Merry-Go-Round
    "rct2.tlt1",     // Ride: Restroom
    "rct2.atm1",     // Ride: Cash Machine
    "rct2.faid1",    // Ride: First Aid Room
    "rct2.infok",    // Ride: Information Kiosk
    "rct2.drnks",    // Ride: Drinks Stall
    "rct2.cndyf",    // Ride: Candyfloss Stall
    "rct2.burgb",    // Ride: Burger Bar
    "rct2.balln",    // Ride: Balloon Stall
    "rct2.arrt1",    // Ride: Corkscrew Roller Coaster
    "rct2.rboat",    // Ride: Rowing Boats
    "rct2.pkent1",   // Park Entrance: Traditional Park Entrance
    "rct2.wtrcyan",  // Water: Natural Water
    "rct2.tarmacb",  // Footpath: Brown Tarmac Footpath
    "rct2.pathspce", // Footpath: Space Style Footpath
    "rct2.pathdirt", // Footpath: Dirt Footpath
    "rct2.pathcrzy", // Footpath: Crazy Paving Footpath
    "rct2.pathash",  // Footpath: Ash Footpath

    // The following are for all random map generation features to work out the box
    "rct2.scgjungl", // Jungle Theming
    "rct2.scgsnow",  // Snow and Ice Theming
    "rct2.scgwater", // Water Feature Theming

    // Surfaces
    "rct2.surface.grass",
    "rct2.surface.sand",
    "rct2.surface.dirt",
    "rct2.surface.rock",
    "rct2.surface.martian",
    "rct2.surface.chequerboard",
    "rct2.surface.grassclumps",
    "rct2.surface.ice",
    "rct2.surface.gridred",
    "rct2.surface.gridyellow",
    "rct2.surface.gridpurple",
    "rct2.surface.gridgreen",
    "rct2.surface.sandred",
    "rct2.surface.sandbrown",

    // Edges
    "rct2.edge.rock",
    "rct2.edge.woodred",
    "rct2.edge.woodblack",
    "rct2.edge.ice",

    // Stations
    "rct2.station.plain",
    "rct2.station.wooden",
    "rct2.station.canvastent",
    "rct2.station.castlegrey",
    "rct2.station.castlebrown",
    "rct2.station.jungle",
    "rct2.station.log",
    "rct2.station.classical",
    "rct2.station.abstract",
    "rct2.station.snow",
    "rct2.station.pagoda",
    "rct2.station.space",

    // Music
    "rct2.music.dodgems",
    "rct2.music.fairground",
    "rct2.music.roman",
    "rct2.music.oriental",
    "rct2.music.martian",
    "rct2.music.jungle",
    "rct2.music.egyptian",
    "rct2.music.toyland",
    "rct2.music.space",
    "rct2.music.horror",
    "rct2.music.techno",
    "rct2.music.gentle",
    "rct2.music.summer",
    "rct2.music.water",
    "rct2.music.wildwest",
    "rct2.music.jurassic",
    "rct2.music.rock1",
    "rct2.music.ragtime",
    "rct2.music.fantasy",
    "rct2.music.rock2",
    "rct2.music.ice",
    "rct2.music.snow",
    "rct2.music.medieval",
    "rct2.music.urban",
    "rct2.music.organ",
    "rct2.music.mechanical",
    "rct2.music.modern",
    "rct2.music.pirate",
    "rct2.music.rock3",
    "rct2.music.candy",
};

const std::string_view DesignerSelectedObjects[] = {
    // An initial default selection + all standard footpaths + all standard stations
    "rct2.scgtrees", // Scenery: Trees
    "rct2.scgshrub", // Scenery: Shrubs and Ornaments
    "rct2.scggardn", // Scenery: Gardens
    "rct2.scgfence", // Scenery: Fences and Walls
    "rct2.scgwalls", // Scenery: Walls and Roofs
    "rct2.scgpathx", // Scenery: Signs and Items for Footpaths
    "rct2.wtrcyan",  // Water: Natural Water
    "rct2.pkent1",   // Park Entrance: Traditional Park Entrance
    "rct2.tarmac",   // Footpath: Tarmac
    "rct2.tarmacg",  // Footpath: Green Tarmac Footpath
    "rct2.tarmacb",  // Footpath: Brown Tarmac Footpath
    "rct2.pathspce", // Footpath: Space Style Footpath
    "rct2.pathcrzy", // Footpath: Crazy Paving Footpath
    "rct2.pathdirt", // Footpath: Dirt Footpath
    "rct2.pathash",  // Footpath: Ash Footpath
    "rct2.station.plain",
    "rct2.station.wooden",
    "rct2.station.canvastent",
    "rct2.station.castlegrey",
    "rct2.station.castlebrown",
    "rct2.station.jungle",
    "rct2.station.log",
    "rct2.station.classical",
    "rct2.station.abstract",
    "rct2.station.snow",
    "rct2.station.pagoda",
    "rct2.station.space",
};
