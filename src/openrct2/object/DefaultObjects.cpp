/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "DefaultObjects.h"
#include "Object.h"

/** rct2: 0x0098DA74 */
const rct_object_entry RequiredSelectedObjects[7] =
{
    // Objects that are always required
    { 0x00000087, { "SCGTREES" } },      // Scenery: Trees
    { 0x00000087, { "SCGSHRUB" } },      // Scenery: Shrubs and Ornaments
    { 0x00000087, { "SCGGARDN" } },      // Scenery: Gardens
    { 0x00000087, { "SCGFENCE" } },      // Scenery: Fences and Walls
    { 0x00000087, { "SCGWALLS" } },      // Scenery: Walls and Roofs
    { 0x00000087, { "SCGPATHX" } },      // Scenery: Signs and Items for Footpaths
    { 0x00000085, { "TARMAC  " } },      // Footpath: Tarmac
};

const rct_object_entry DefaultSelectedObjects[26] =
{
    // An initial default selection
    { 0x00000080, { "TWIST1  " } },      // Ride: Twist
    { 0x00000080, { "PTCT1   " } },      // Ride: Wooden Roller Coaster (Wooden Roller Coaster Trains)
    { 0x00000080, { "ZLDB    " } },      // Ride: Junior Roller Coaster (Ladybird Trains)
    { 0x00000080, { "LFB1    " } },      // Ride: Log Flume
    { 0x00000080, { "VCR     " } },      // Ride: Vintage Cars
    { 0x00000080, { "MGR1    " } },      // Ride: Merry-Go-Round
    { 0x00000080, { "TLT1    " } },      // Ride: Restroom
    { 0x00000080, { "ATM1    " } },      // Ride: Cash Machine
    { 0x00000080, { "FAID1   " } },      // Ride: First Aid Room
    { 0x00000080, { "INFOK   " } },      // Ride: Information Kiosk
    { 0x00000080, { "DRNKS   " } },      // Ride: Drinks Stall
    { 0x00000080, { "CNDYF   " } },      // Ride: Candyfloss Stall
    { 0x00000080, { "BURGB   " } },      // Ride: Burger Bar
    { 0x00000080, { "BALLN   " } },      // Ride: Balloon Stall
    { 0x00000080, { "ARRT1   " } },      // Ride: Corkscrew Roller Coaster
    { 0x00000080, { "RBOAT   " } },      // Ride: Rowing Boats
    { 0x00000088, { "PKENT1  " } },      // Park Entrance: Traditional Park Entrance
    { 0x00000089, { "WTRCYAN " } },      // Water: Natural Water
    { 0x00000085, { "TARMACB " } },      // Footpath: Brown Tarmac Footpath
    { 0x00000085, { "PATHSPCE" } },      // Footpath: Space Style Footpath
    { 0x00000085, { "PATHDIRT" } },      // Footpath: Dirt Footpath
    { 0x00000085, { "PATHCRZY" } },      // Footpath: Crazy Paving Footpath
    { 0x00000085, { "PATHASH " } },      // Footpath: Ash Footpath

    // The following are for all random map generation features to work out the box
    { 0x00000087, { "SCGJUNGL" } },      // Jungle Theming
    { 0x00000087, { "SCGSNOW " } },      // Snow and Ice Theming
    { 0x00000087, { "SCGWATER" } }       // Water Feature Theming
};
