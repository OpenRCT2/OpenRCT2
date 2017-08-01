#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "_legacy.h"

#pragma warning(disable : 4295) // 'identifier': array is too small to include a terminating null character

/** rct2: 0x0098DA74 */
const rct_object_entry RequiredSelectedObjects[7] = {
        // Objects that are always required
        { 0x00000087, { "SCGTREES" }, 0 },      // Scenery: Trees
        { 0x00000087, { "SCGSHRUB" }, 0 },      // Scenery: Shrubs and Ornaments
        { 0x00000087, { "SCGGARDN" }, 0 },      // Scenery: Gardens
        { 0x00000087, { "SCGFENCE" }, 0 },      // Scenery: Fences and Walls
        { 0x00000087, { "SCGWALLS" }, 0 },      // Scenery: Walls and Roofs
        { 0x00000087, { "SCGPATHX" }, 0 },      // Scenery: Signs and Items for Footpaths
        { 0x00000085, { "TARMAC  " }, 0 },      // Footpath: Tarmac
};

const rct_object_entry DefaultSelectedObjects[26] = {
        // An initial default selection
        { 0x00000080, { "TWIST1  " }, 0 },      // Ride: Twist
        { 0x00000080, { "PTCT1   " }, 0 },      // Ride: Wooden Roller Coaster (Wooden Roller Coaster Trains)
        { 0x00000080, { "ZLDB    " }, 0 },      // Ride: Junior Roller Coaster (Ladybird Trains)
        { 0x00000080, { "LFB1    " }, 0 },      // Ride: Log Flume
        { 0x00000080, { "VCR     " }, 0 },      // Ride: Vintage Cars
        { 0x00000080, { "MGR1    " }, 0 },      // Ride: Merry-Go-Round
        { 0x00000080, { "TLT1    " }, 0 },      // Ride: Restroom
        { 0x00000080, { "ATM1    " }, 0 },      // Ride: Cash Machine
        { 0x00000080, { "FAID1   " }, 0 },      // Ride: First Aid Room
        { 0x00000080, { "INFOK   " }, 0 },      // Ride: Information Kiosk
        { 0x00000080, { "DRNKS   " }, 0 },      // Ride: Drinks Stall
        { 0x00000080, { "CNDYF   " }, 0 },      // Ride: Candyfloss Stall
        { 0x00000080, { "BURGB   " }, 0 },      // Ride: Burger Bar
        { 0x00000080, { "BALLN   " }, 0 },      // Ride: Balloon Stall
        { 0x00000080, { "ARRT1   " }, 0 },      // Ride: Corkscrew Roller Coaster
        { 0x00000080, { "RBOAT   " }, 0 },      // Ride: Rowing Boats
        { 0x00000088, { "PKENT1  " }, 0 },      // Park Entrance: Traditional Park Entrance
        { 0x00000089, { "WTRCYAN " }, 0 },      // Water: Natural Water
        { 0x00000085, { "TARMACB " }, 0 },      // Footpath: Brown Tarmac Footpath
        { 0x00000085, { "PATHSPCE" }, 0 },      // Footpath: Space Style Footpath
        { 0x00000085, { "PATHDIRT" }, 0 },      // Footpath: Dirt Footpath
        { 0x00000085, { "PATHCRZY" }, 0 },      // Footpath: Crazy Paving Footpath
        { 0x00000085, { "PATHASH " }, 0 },      // Footpath: Ash Footpath

        // The following are for all random map generation features to work out the box
        { 0x00000087, { "SCGJUNGL" }, 0 },      // Jungle Theming
        { 0x00000087, { "SCGSNOW " }, 0 },      // Snow and Ice Theming
        { 0x00000087, { "SCGWATER" }, 0 }       // Water Feature Theming
};