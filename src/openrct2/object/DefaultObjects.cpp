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

// clang-format off
const std::string_view MinimumRequiredObjects[] = { "rct2.terrain_surface.grass", "rct2.terrain_edge.rock" };

const std::string_view DefaultSelectedObjects[] = {
    // An initial default selection
    "rct2.scenery_group.scgtrees", // Scenery: Trees
    "rct2.scenery_group.scgshrub", // Scenery: Shrubs and Ornaments
    "rct2.scenery_group.scggardn", // Scenery: Gardens
    "rct2.scenery_group.scgfence", // Scenery: Fences and Walls
    "rct2.scenery_group.scgwalls", // Scenery: Walls and Roofs
    "rct2.scenery_group.scgpathx", // Scenery: Signs and Items for Footpaths
    "rct2.ride.twist1",   // Ride: Twist
    "rct2.ride.ptct1",    // Ride: Wooden Roller Coaster (Wooden Roller Coaster Trains)
    "rct2.ride.zldb",     // Ride: Junior Roller Coaster (Ladybird Trains)
    "rct2.ride.lfb1",     // Ride: Log Flume
    "rct2.ride.vcr",      // Ride: Vintage Cars
    "rct2.ride.mgr1",     // Ride: Merry-Go-Round
    "rct2.ride.tlt1",     // Ride: Toilet
    "rct2.ride.atm1",     // Ride: Cash Machine
    "rct2.ride.faid1",    // Ride: First Aid Room
    "rct2.ride.infok",    // Ride: Information Kiosk
    "rct2.ride.drnks",    // Ride: Drinks Stall
    "rct2.ride.cndyf",    // Ride: Candyfloss Stall
    "rct2.ride.burgb",    // Ride: Burger Bar
    "rct2.ride.balln",    // Ride: Balloon Stall
    "rct2.ride.arrt1",    // Ride: Corkscrew Roller Coaster
    "rct2.ride.rboat",    // Ride: Rowing Boats
    "rct2.park_entrance.pkent1",   // Park Entrance: Traditional Park Entrance
    "rct2.water.wtrcyan",  // Water: Natural Water

    // The following are for all random map generation features to work out the box
    "rct2.scenery_group.scgjungl", // Jungle Theming
    "rct2.scenery_group.scgsnow",  // Snow and Ice Theming
    "rct2.scenery_group.scgwater", // Water Feature Theming

    // Surfaces
    "rct2.terrain_surface.grass",
    "rct2.terrain_surface.sand",
    "rct2.terrain_surface.dirt",
    "rct2.terrain_surface.rock",
    "rct2.terrain_surface.martian",
    "rct2.terrain_surface.chequerboard",
    "rct2.terrain_surface.grass_clumps",
    "rct2.terrain_surface.ice",
    "rct2.terrain_surface.grid_red",
    "rct2.terrain_surface.grid_yellow",
    "rct2.terrain_surface.grid_purple",
    "rct2.terrain_surface.grid_green",
    "rct2.terrain_surface.sand_red",
    "rct2.terrain_surface.sand_brown",

    // Edges
    "rct2.terrain_edge.rock",
    "rct2.terrain_edge.wood_red",
    "rct2.terrain_edge.wood_black",
    "rct2.terrain_edge.ice",

    // Stations
    "rct2.station.plain",
    "rct2.station.wooden",
    "rct2.station.canvas_tent",
    "rct2.station.castle_grey",
    "rct2.station.castle_brown",
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

    // Footpath surfaces
    "rct2.footpath_surface.tarmac",
    "rct2.footpath_surface.tarmac_brown",
    "rct2.footpath_surface.tarmac_red",
    "rct2.footpath_surface.dirt",
    "rct2.footpath_surface.crazy_paving",
    "rct2.footpath_surface.ash",
    "rct2.footpath_surface.queue_blue",
    "rct2.footpath_surface.queue_green",
    "rct2.footpath_surface.queue_red",
    "rct2.footpath_surface.queue_yellow",

    // Footpath railings
    "rct2.footpath_railings.bamboo_black",
    "rct2.footpath_railings.bamboo_brown",
    "rct2.footpath_railings.concrete",
    "rct2.footpath_railings.concrete_green",
    "rct2.footpath_railings.space",
    "rct2.footpath_railings.wood",
};

const std::string_view DesignerSelectedObjects[] = {
    // An initial default selection + all standard footpaths + all standard stations
    "rct2.scenery_group.scgtrees", // Scenery: Trees
    "rct2.scenery_group.scgshrub", // Scenery: Shrubs and Ornaments
    "rct2.scenery_group.scggardn", // Scenery: Gardens
    "rct2.scenery_group.scgfence", // Scenery: Fences and Walls
    "rct2.scenery_group.scgwalls", // Scenery: Walls and Roofs
    "rct2.scenery_group.scgpathx", // Scenery: Signs and Items for Footpaths
    "rct2.water.wtrcyan",          // Water: Natural Water
    "rct2.park_entrance.pkent1",   // Park Entrance: Traditional Park Entrance
    "rct2.terrain_surface.grass",
    "rct2.terrain_edge.rock",

    // Footpath surfaces
    "rct2.footpath_surface.tarmac",
    "rct2.footpath_surface.tarmac_brown",
    "rct2.footpath_surface.tarmac_red",
    "rct2.footpath_surface.tarmac_green",
    "rct2.footpath_surface.dirt",
    "rct2.footpath_surface.crazy_paving",
    "rct2.footpath_surface.ash",
    "rct2.footpath_surface.queue_blue",
    "rct2.footpath_surface.queue_green",
    "rct2.footpath_surface.queue_red",
    "rct2.footpath_surface.queue_yellow",

    // Footpath railings
    "rct2.footpath_railings.bamboo_black",
    "rct2.footpath_railings.bamboo_brown",
    "rct2.footpath_railings.concrete",
    "rct2.footpath_railings.concrete_green",
    "rct2.footpath_railings.space",
    "rct2.footpath_railings.wood",

    // Stations
    "rct2.station.plain",
    "rct2.station.wooden",
    "rct2.station.canvas_tent",
    "rct2.station.castle_grey",
    "rct2.station.castle_brown",
    "rct2.station.jungle",
    "rct2.station.log",
    "rct2.station.classical",
    "rct2.station.abstract",
    "rct2.station.snow",
    "rct2.station.pagoda",
    "rct2.station.space",
};

// clang-format on
