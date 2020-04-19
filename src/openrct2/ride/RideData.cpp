/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

/**
 * Whether a particular ride has a running track or not. Will probably end up
 * being used in various places in the game.
 *
 * Data source is 0x0097E3AC
 *
 * Generating function is here
 * https://gist.github.com/kevinburke/eaeb1d8149a6eef0dcc1
 */

#include "RideData.h"

#include "../Cheats.h"
#include "../audio/audio.h"
#include "../interface/Colour.h"
#include "../localisation/Localisation.h"
#include "../sprites.h"
#include "Ride.h"
#include "ShopItem.h"
#include "Track.h"
#include "coaster/meta/AirPoweredVerticalCoaster.h"
#include "coaster/meta/BobsleighCoaster.h"
#include "coaster/meta/CompactInvertedCoaster.h"
#include "coaster/meta/CorkscrewRollerCoaster.h"
#include "coaster/meta/FlyingRollerCoaster.h"
#include "coaster/meta/GigaCoaster.h"
#include "coaster/meta/HeartlineTwisterCoaster.h"
#include "coaster/meta/InvertedHairpinCoaster.h"
#include "coaster/meta/InvertedImpulseCoaster.h"
#include "coaster/meta/InvertedRollerCoaster.h"
#include "coaster/meta/JuniorRollerCoaster.h"
#include "coaster/meta/LIMLaunchedRollerCoaster.h"
#include "coaster/meta/LayDownRollerCoaster.h"
#include "coaster/meta/LoopingRollerCoaster.h"
#include "coaster/meta/MineRide.h"
#include "coaster/meta/MineTrainCoaster.h"
#include "coaster/meta/MiniRollerCoaster.h"
#include "coaster/meta/MiniSuspendedCoaster.h"
#include "coaster/meta/MultiDimensionRollerCoaster.h"
#include "coaster/meta/ReverseFreefallCoaster.h"
#include "coaster/meta/ReverserRollerCoaster.h"
#include "coaster/meta/SideFrictionRollerCoaster.h"
#include "coaster/meta/SpiralRollerCoaster.h"
#include "coaster/meta/StandUpRollerCoaster.h"
#include "coaster/meta/SteelWildMouse.h"
#include "coaster/meta/Steeplechase.h"
#include "coaster/meta/SuspendedSwingingCoaster.h"
#include "coaster/meta/TwisterRollerCoaster.h"
#include "coaster/meta/VerticalDropCoaster.h"
#include "coaster/meta/VirginiaReel.h"
#include "coaster/meta/WaterCoaster.h"
#include "coaster/meta/WoodenRollerCoaster.h"
#include "coaster/meta/WoodenWildMouse.h"
#include "gentle/meta/CarRide.h"
#include "gentle/meta/Circus.h"
#include "gentle/meta/CrookedHouse.h"
#include "gentle/meta/Dodgems.h"
#include "gentle/meta/FerrisWheel.h"
#include "gentle/meta/FlyingSaucers.h"
#include "gentle/meta/GhostTrain.h"
#include "gentle/meta/HauntedHouse.h"
#include "gentle/meta/Maze.h"
#include "gentle/meta/MerryGoRound.h"
#include "gentle/meta/MiniGolf.h"
#include "gentle/meta/MiniHelicopters.h"
#include "gentle/meta/MonorailCycles.h"
#include "gentle/meta/ObservationTower.h"
#include "gentle/meta/SpaceRings.h"
#include "gentle/meta/SpiralSlide.h"
#include "shops/meta/CashMachine.h"
#include "shops/meta/DrinkStall.h"
#include "shops/meta/FirstAid.h"
#include "shops/meta/FoodStall.h"
#include "shops/meta/InformationKiosk.h"
#include "shops/meta/Shop.h"
#include "shops/meta/Toilets.h"
#include "thrill/meta/3DCinema.h"
#include "thrill/meta/Enterprise.h"
#include "thrill/meta/GoKarts.h"
#include "thrill/meta/LaunchedFreefall.h"
#include "thrill/meta/MagicCarpet.h"
#include "thrill/meta/MotionSimulator.h"
#include "thrill/meta/RotoDrop.h"
#include "thrill/meta/SwingingInverterShip.h"
#include "thrill/meta/SwingingShip.h"
#include "thrill/meta/TopSpin.h"
#include "thrill/meta/Twist.h"
#include "transport/meta/Chairlift.h"
#include "transport/meta/Lift.h"
#include "transport/meta/MiniatureRailway.h"
#include "transport/meta/Monorail.h"
#include "transport/meta/SuspendedMonorail.h"
#include "water/meta/BoatHire.h"
#include "water/meta/DinghySlide.h"
#include "water/meta/LogFlume.h"
#include "water/meta/RiverRafts.h"
#include "water/meta/RiverRapids.h"
#include "water/meta/SplashBoats.h"
#include "water/meta/SubmarineRide.h"

#include <iterator>

// clang-format off
const bool hasRunningTrack[RIDE_TYPE_COUNT] = {
    true,   // 0 Spiral Roller coaster
    true,   // 1 Stand Up Coaster
    true,   // 2 Suspended Swinging
    true,   // 3 Inverted
    true,   // 4 Steel Mini Coaster
    true,   // 5 Mini Railway
    true,   // 6 Monorail
    true,   // 7 Mini Suspended Coaster
    false,  // 8 Boat Hire
    true,   // 9 Wooden Wild Mine/Mouse
    true,   // a Steeplechase/Motorbike/Soap Box Derby
    true,   // b Car Ride
    true,   // c Launched Freefall
    true,   // d Bobsleigh Coaster
    true,   // e Observation Tower
    true,   // f Looping Roller Coaster
    true,   // 10 Dinghy Slide
    true,   // 11 Mine Train Coaster
    true,   // 12 Chairlift
    true,   // 13 Corkscrew Roller Coaster
    false,  // 14 Maze
    false,  // 15 Spiral Slide
    true,   // 16 Go Karts
    true,   // 17 Log Flume
    true,   // 18 River Rapids
    false,  // 19 Dodgems
    false,  // 1a Pirate Ship
    false,  // 1b Swinging Inverter Ship
    false,  // 1c Food Stall
    false,  // 1d (none)
    false,  // 1e Drink Stall
    false,  // 1f (none)
    false,  // 20 Shop (all types)
    false,  // 21 Merry Go Round
    false,  // 22 Balloon Stall (maybe)
    false,  // 23 Information Kiosk
    false,  // 24 Bathroom
    false,  // 25 Ferris Wheel
    false,  // 26 Motion Simulator
    false,  // 27 3D Cinema
    false,  // 28 Topspin
    false,  // 29 Space Rings
    true,   // 2a Reverse Freefall Coaster
    true,   // 2b Elevator
    true,   // 2c Vertical Drop Roller Coaster
    false,  // 2d ATM
    false,  // 2e Twist
    false,  // 2f Haunted House
    false,  // 30 First Aid
    false,  // 31 Circus Show
    true,   // 32 Ghost Train
    true,   // 33 Twister Roller Coaster
    true,   // 34 Wooden Roller Coaster
    true,   // 35 Side-Friction Roller Coaster
    true,   // 36 Wild Mouse
    true,   // 37 Multi Dimension Coaster
    true,   // 38 (none)
    true,   // 39 Flying Roller Coaster
    true,   // 3a (none)
    true,   // 3b Virginia Reel
    true,   // 3c Splash Boats
    true,   // 3d Mini Helicopters
    true,   // 3e Lay-down Roller Coaster
    true,   // 3f Suspended Monorail
    true,   // 40 (none)
    true,   // 41 Reverser Roller Coaster
    true,   // 42 Heartline Twister Roller Coaster
    true,   // 43 Mini Golf
    true,   // 44 Giga Coaster
    true,   // 45 Roto-Drop
    false,  // 46 Flying Saucers
    false,  // 47 Crooked House
    true,   // 48 Monorail Cycles
    true,   // 49 Compact Inverted Coaster
    true,   // 4a Water Coaster
    true,   // 4b Air Powered Vertical Coaster
    true,   // 4c Inverted Hairpin Coaster
    false,  // 4d Magic Carpet
    false,  // 4e Submarine Ride
    true,   // 4f River Rafts
    false,  // 50 (none)
    false,  // 51 Enterprise
    false,  // 52 (none)
    false,  // 53 (none)
    false,  // 54 (none)
    true,   // 55 (none)
    true,   // 56 Inverted Impulse Coaster
    true,   // 57 Mini Roller Coaster
    true,   // 58 Mine Ride
    true,   // 59 (none)
    true,   // 5a LIM Launched Roller Coaster
};

/**
 * Data about ride running costs. This is widely adjusted by the upkeep
 * function, so values that don't make much sense here (a roller coaster having
 * cheaper upkeep than a car ride) are fixed later on.
 *
 * data generation script: https://gist.github.com/kevinburke/6bcf4a8fcc95faad7bac
 */
const uint8_t initialUpkeepCosts[RIDE_TYPE_COUNT] = {
    41, // 00 Spiral Roller coaster
    40, // 01 Stand Up Coaster
    40, // 02 Suspended Swinging
    40, // 03 Inverted
    40, // 04 Steel Mini Coaster
    60, // 05 Mini Railway
    65, // 06 Monorail
    40, // 07 Mini Suspended Coaster
    50, // 08 Boat Hire
    40, // 09 Wooden Wild Mine/Mouse
    40, // 0a Steeplechase/Motorbike/Soap Box Derby
    70, // 0b Car Ride
    50, // 0c Launched Freefall
    40, // 0d Bobsleigh Coaster
    50, // 0e Observation Tower
    40, // 0f Looping Roller Coaster
    40, // 10 Dinghy Slide
    40, // 11 Mine Train Coaster
    60, // 12 Chairlift
    40, // 13 Corkscrew Roller Coaster
    50, // 14 Maze
    50, // 15 Spiral Slide
    50, // 16 Go Karts
    80, // 17 Log Flume
    82, // 18 River Rapids
    50, // 19 Dodgems
    50, // 1a Pirate Ship
    50, // 1b Swinging Inverter Ship
    50, // 1c Food Stall
    50, // 1d (none)
    50, // 1e Drink Stall
    50, // 1f (none)
    50, // 20 Shop (all types)
    50, // 21 Merry Go Round
    50, // 22 Balloon Stall (maybe)
    50, // 23 Information Kiosk
    50, // 24 Bathroom
    50, // 25 Ferris Wheel
    50, // 26 Motion Simulator
    50, // 27 3D Cinema
    50, // 28 Topspin
    50, // 29 Space Rings
    80, // 2a Reverse Freefall Coaster
    50, // 2b Elevator
    44, // 2c Vertical Drop Roller Coaster
    40, // 2d ATM
    50, // 2e Twist
    50, // 2f Haunted House
    45, // 30 First Aid
    50, // 31 Circus Show
    80, // 32 Ghost Train
    43, // 33 Twister Roller Coaster
    40, // 34 Wooden Roller Coaster
    39, // 35 Side-Friction Roller Coaster
    40, // 36 Wild Mouse
    75, // 37 Multi Dimension Coaster
    75, // 38 (none)
    49, // 39 Flying Roller Coaster
    49, // 3a (none)
    39, // 3b Virginia Reel
    70, // 3c Splash Boats
    70, // 3d Mini Helicopters
    49, // 3e Lay-down Roller Coaster
    70, // 3f Suspended Monorail
    49, // 40 (none)
    39, // 41 Reverser Roller Coaster
    47, // 42 Heartline Twister Roller Coaster
    30, // 43 Mini Golf
    10, // 44 Giga Coaster
    50, // 45 Roto-Drop
    90, // 46 Flying Saucers
    30, // 47 Crooked House
    47, // 48 Monorail Cycles
    40, // 49 Compact Inverted Coaster
    60, // 4a Water Coaster
    90, // 4b Air Powered Vertical Coaster
    40, // 4c Inverted Hairpin Coaster
    50, // 4d Magic Carpet
    50, // 4e Submarine Ride
    50, // 4f River Rafts
    50, // 50 (none)
    50, // 51 Enterprise
    50, // 52 (none)
    50, // 53 (none)
    50, // 54 (none)
    40, // 55 (none)
    180,// 56 Inverted Impulse Coaster
    35, // 57 Mini Roller Coaster
    50, // 58 Mine Ride
    42, // 59 (none)
    42, // 5a LIM Launched Roller Coaster
};

const uint8_t costPerTrackPiece[RIDE_TYPE_COUNT] = {
    80, // 00 Spiral Roller coaster
    80, // 01 Stand Up Coaster
    80, // 02 Suspended Swinging
    80, // 03 Inverted
    80, // 04 Steel Mini Coaster
    0,  // 05 Mini Railway
    0,  // 06 Monorail
    80, // 07 Mini Suspended Coaster
    0,  // 08 Boat Hire
    80, // 09 Wooden Wild Mine/Mouse
    80, // 0a Steeplechase/Motorbike/Soap Box Derby
    0,  // 0b Car Ride
    0,  // 0c Launched Freefall
    80, // 0d Bobsleigh Coaster
    0,  // 0e Observation Tower
    80, // 0f Looping Roller Coaster
    80, // 10 Dinghy Slide
    80, // 11 Mine Train Coaster
    0,  // 12 Chairlift
    80, // 13 Corkscrew Roller Coaster
    0,  // 14 Maze
    0,  // 15 Spiral Slide
    0,  // 16 Go Karts
    0,  // 17 Log Flume
    0,  // 18 River Rapids
    0,  // 19 Dodgems
    0,  // 1a Pirate Ship
    0,  // 1b Swinging Inverter Ship
    0,  // 1c Food Stall
    0,  // 1d (none)
    0,  // 1e Drink Stall
    0,  // 1f (none)
    0,  // 20 Shop (all types)
    0,  // 21 Merry Go Round
    0,  // 22 Balloon Stall (maybe)
    0,  // 23 Information Kiosk
    0,  // 24 Bathroom
    0,  // 25 Ferris Wheel
    0,  // 26 Motion Simulator
    0,  // 27 3D Cinema
    0,  // 28 Topspin
    0,  // 29 Space Rings
    0,  // 2a Reverse Freefall Coaster
    0,  // 2b Elevator
    80, // 2c Vertical Drop Roller Coaster
    0,  // 2d ATM
    0,  // 2e Twist
    0,  // 2f Haunted House
    0,  // 30 First Aid
    0,  // 31 Circus Show
    0,  // 32 Ghost Train
    80, // 33 Twister Roller Coaster
    80, // 34 Wooden Roller Coaster
    80, // 35 Side-Friction Roller Coaster
    80, // 36 Wild Mouse
    90, // 37 Multi Dimension Coaster
    90, // 38 (none)
    90, // 39 Flying Roller Coaster
    90, // 3a (none)
    80, // 3b Virginia Reel
    0,  // 3c Splash Boats
    0,  // 3d Mini Helicopters
    90, // 3e Lay-down Roller Coaster
    0,  // 3f Suspended Monorail
    90, // 40 (none)
    80, // 41 Reverser Roller Coaster
    80, // 42 Heartline Twister Roller Coaster
    80, // 43 Mini Golf
    80, // 44 Giga Coaster
    0,  // 45 Roto-Drop
    0,  // 46 Flying Saucers
    0,  // 47 Crooked House
    0,  // 48 Monorail Cycles
    80, // 49 Compact Inverted Coaster
    80, // 4a Water Coaster
    0,  // 4b Air Powered Vertical Coaster
    80, // 4c Inverted Hairpin Coaster
    0,  // 4d Magic Carpet
    0,  // 4e Submarine Ride
    0,  // 4f River Rafts
    0,  // 50 (none)
    0,  // 51 Enterprise
    0,  // 52 (none)
    0,  // 53 (none)
    0,  // 54 (none)
    80, // 55 (none)
    80, // 56 Inverted Impulse Coaster
    80, // 57 Mini Roller Coaster
    80, // 58 Mine Ride
    80, // 59 (none)
    80, // 5a LIM Launched Roller Coaster
};

/**
 * Data initially at 0x0097E3B4
 */
const uint8_t costPerVehicle[RIDE_TYPE_COUNT] = {
    10, // 00 Spiral Roller coaster
    10, // 01 Stand Up Coaster
    20, // 02 Suspended Swinging
    13, // 03 Inverted
    8,  // 04 Steel Mini Coaster
    10, // 05 Mini Railway
    10, // 06 Monorail
    10, // 07 Mini Suspended Coaster
    4,  // 08 Boat Hire
    9,  // 09 Wooden Wild Mine/Mouse
    10, // 0a Steeplechase/Motorbike/Soap Box Derby
    8,  // 0b Car Ride
    10, // 0c Launched Freefall
    10, // 0d Bobsleigh Coaster
    10, // 0e Observation Tower
    10, // 0f Looping Roller Coaster
    4,  // 10 Dinghy Slide
    10, // 11 Mine Train Coaster
    4,  // 12 Chairlift
    11, // 13 Corkscrew Roller Coaster
    0,  // 14 Maze
    0,  // 15 Spiral Slide
    8,  // 16 Go Karts
    9,  // 17 Log Flume
    10, // 18 River Rapids
    5,  // 19 Dodgems
    0,  // 1a Pirate Ship
    0,  // 1b Swinging Inverter Ship
    0,  // 1c Food Stall
    0,  // 1d (none)
    0,  // 1e Drink Stall
    0,  // 1f (none)
    0,  // 20 Shop (all types)
    0,  // 21 Merry Go Round
    0,  // 22 Balloon Stall (maybe)
    0,  // 23 Information Kiosk
    0,  // 24 Bathroom
    0,  // 25 Ferris Wheel
    0,  // 26 Motion Simulator
    0,  // 27 3D Cinema
    0,  // 28 Topspin
    0,  // 29 Space Rings
    0,  // 2a Reverse Freefall Coaster
    10, // 2b Elevator
    11, // 2c Vertical Drop Roller Coaster
    0,  // 2d ATM
    0,  // 2e Twist
    0,  // 2f Haunted House
    0,  // 30 First Aid
    0,  // 31 Circus Show
    8,  // 32 Ghost Train
    11, // 33 Twister Roller Coaster
    10, // 34 Wooden Roller Coaster
    10, // 35 Side-Friction Roller Coaster
    9,  // 36 Wild Mouse
    11, // 37 Multi Dimension Coaster
    11, // 38 (none)
    11, // 39 Flying Roller Coaster
    11, // 3a (none)
    10, // 3b Virginia Reel
    9,  // 3c Splash Boats
    8,  // 3d Mini Helicopters
    11, // 3e Lay-down Roller Coaster
    10, // 3f Suspended Monorail
    11, // 40 (none)
    10, // 41 Reverser Roller Coaster
    11, // 42 Heartline Twister Roller Coaster
    11, // 43 Mini Golf
    12, // 44 Giga Coaster
    10, // 45 Roto-Drop
    5,  // 46 Flying Saucers
    0,  // 47 Crooked House
    8,  // 48 Monorail Cycles
    13, // 49 Compact Inverted Coaster
    8,  // 4a Water Coaster
    0,  // 4b Air Powered Vertical Coaster
    9,  // 4c Inverted Hairpin Coaster
    0,  // 4d Magic Carpet
    4,  // 4e Submarine Ride
    9,  // 4f River Rafts
    0,  // 50 (none)
    0,  // 51 Enterprise
    0,  // 52 (none)
    0,  // 53 (none)
    0,  // 54 (none)
    11, // 55 (none)
    11, // 56 Inverted Impulse Coaster
    8,  // 57 Mini Roller Coaster
    10, // 58 Mine Ride
    9,  // 59 (none)
    9,  // 5a LIM Launched Roller Coaster
};

/**
 * Data at 0x0097E3B6, originally set to either be 3 or 0 and replaced here by
 * a boolean table. This may be exactly the same as hasRunningTrack above.
 */
const bool chargeUpkeepForTrainLength[RIDE_TYPE_COUNT] = {
    true,   // 00 Spiral Roller coaster
    true,   // 01 Stand Up Coaster
    true,   // 02 Suspended Swinging
    true,   // 03 Inverted
    true,   // 04 Steel Mini Coaster
    true,   // 05 Mini Railway
    true,   // 06 Monorail
    true,   // 07 Mini Suspended Coaster
    false,  // 08 Boat Hire
    true,   // 09 Wooden Wild Mine/Mouse
    true,   // 0a Steeplechase/Motorbike/Soap Box Derby
    true,   // 0b Car Ride
    false,  // 0c Launched Freefall
    true,   // 0d Bobsleigh Coaster
    false,  // 0e Observation Tower
    true,   // 0f Looping Roller Coaster
    true,   // 10 Dinghy Slide
    true,   // 11 Mine Train Coaster
    true,   // 12 Chairlift
    true,   // 13 Corkscrew Roller Coaster
    false,  // 14 Maze
    false,  // 15 Spiral Slide
    false,  // 16 Go Karts
    false,  // 17 Log Flume
    false,  // 18 River Rapids
    false,  // 19 Dodgems
    false,  // 1a Pirate Ship
    false,  // 1b Swinging Inverter Ship
    false,  // 1c Food Stall
    false,  // 1d (none)
    false,  // 1e Drink Stall
    false,  // 1f (none)
    false,  // 20 Shop (all types)
    false,  // 21 Merry Go Round
    false,  // 22 Balloon Stall (maybe)
    false,  // 23 Information Kiosk
    false,  // 24 Bathroom
    false,  // 25 Ferris Wheel
    false,  // 26 Motion Simulator
    false,  // 27 3D Cinema
    false,  // 28 Topspin
    false,  // 29 Space Rings
    false,  // 2a Reverse Freefall Coaster
    false,  // 2b Elevator
    true,   // 2c Vertical Drop Roller Coaster
    false,  // 2d ATM
    false,  // 2e Twist
    false,  // 2f Haunted House
    false,  // 30 First Aid
    false,  // 31 Circus Show
    true,   // 32 Ghost Train
    true,   // 33 Twister Roller Coaster
    true,   // 34 Wooden Roller Coaster
    true,   // 35 Side-Friction Roller Coaster
    true,   // 36 Wild Mouse
    true,   // 37 Multi Dimension Coaster
    true,   // 38 (none)
    true,   // 39 Flying Roller Coaster
    true,   // 3a (none)
    true,   // 3b Virginia Reel
    false,  // 3c Splash Boats
    true,   // 3d Mini Helicopters
    true,   // 3e Lay-down Roller Coaster
    true,   // 3f Suspended Monorail
    true,   // 40 (none)
    true,   // 41 Reverser Roller Coaster
    true,   // 42 Heartline Twister Roller Coaster
    true,   // 43 Mini Golf
    true,   // 44 Giga Coaster
    false,  // 45 Roto-Drop
    false,  // 46 Flying Saucers
    false,  // 47 Crooked House
    true,   // 48 Monorail Cycles
    true,   // 49 Compact Inverted Coaster
    true,   // 4a Water Coaster
    false,  // 4b Air Powered Vertical Coaster
    true,   // 4c Inverted Hairpin Coaster
    false,  // 4d Magic Carpet
    false,  // 4e Submarine Ride
    false,  // 4f River Rafts
    false,  // 50 (none)
    false,  // 51 Enterprise
    false,  // 52 (none)
    false,  // 53 (none)
    false,  // 54 (none)
    true,   // 55 (none)
    true,   // 56 Inverted Impulse Coaster
    true,   // 57 Mini Roller Coaster
    true,   // 58 Mine Ride
    true,   // 59 (none)
    true,   // 5a LIM Launched Roller Coaster
};

/* Data at 0x0097E3B8 */
const uint8_t costPerStation[RIDE_TYPE_COUNT] = {
    10, // 00 Spiral Roller coaster
    10, // 01 Stand Up Coaster
    10, // 02 Suspended Swinging
    10, // 03 Inverted
    5,  // 04 Steel Mini Coaster
    5,  // 05 Mini Railway
    10, // 06 Monorail
    10, // 07 Mini Suspended Coaster
    0,  // 08 Boat Hire
    10, // 09 Wooden Wild Mine/Mouse
    10, // 0a Steeplechase/Motorbike/Soap Box Derby
    5,  // 0b Car Ride
    0,  // 0c Launched Freefall
    10, // 0d Bobsleigh Coaster
    0,  // 0e Observation Tower
    10, // 0f Looping Roller Coaster
    10, // 10 Dinghy Slide
    10, // 11 Mine Train Coaster
    10, // 12 Chairlift
    10, // 13 Corkscrew Roller Coaster
    0,  // 14 Maze
    0,  // 15 Spiral Slide
    0,  // 16 Go Karts
    10, // 17 Log Flume
    10, // 18 River Rapids
    0,  // 19 Dodgems
    0,  // 1a Pirate Ship
    0,  // 1b Swinging Inverter Ship
    0,  // 1c Food Stall
    0,  // 1d (none)
    0,  // 1e Drink Stall
    0,  // 1f (none)
    0,  // 20 Shop (all types)
    0,  // 21 Merry Go Round
    0,  // 22 Balloon Stall (maybe)
    0,  // 23 Information Kiosk
    0,  // 24 Bathroom
    0,  // 25 Ferris Wheel
    0,  // 26 Motion Simulator
    0,  // 27 3D Cinema
    0,  // 28 Topspin
    0,  // 29 Space Rings
    10, // 2a Reverse Freefall Coaster
    0,  // 2b Elevator
    10, // 2c Vertical Drop Roller Coaster
    0,  // 2d ATM
    0,  // 2e Twist
    0,  // 2f Haunted House
    0,  // 30 First Aid
    0,  // 31 Circus Show
    5,  // 32 Ghost Train
    10, // 33 Twister Roller Coaster
    10, // 34 Wooden Roller Coaster
    10, // 35 Side-Friction Roller Coaster
    10, // 36 Wild Mouse
    15, // 37 Multi Dimension Coaster
    15, // 38 (none)
    15, // 39 Flying Roller Coaster
    15, // 3a (none)
    10, // 3b Virginia Reel
    10, // 3c Splash Boats
    5,  // 3d Mini Helicopters
    15, // 3e Lay-down Roller Coaster
    10, // 3f Suspended Monorail
    15, // 40 (none)
    10, // 41 Reverser Roller Coaster
    10, // 42 Heartline Twister Roller Coaster
    10, // 43 Mini Golf
    40, // 44 Giga Coaster
    0,  // 45 Roto-Drop
    0,  // 46 Flying Saucers
    0,  // 47 Crooked House
    5,  // 48 Monorail Cycles
    10, // 49 Compact Inverted Coaster
    10, // 4a Water Coaster
    10, // 4b Air Powered Vertical Coaster
    10, // 4c Inverted Hairpin Coaster
    0,  // 4d Magic Carpet
    0,  // 4e Submarine Ride
    10, // 4f River Rafts
    0,  // 50 (none)
    0,  // 51 Enterprise
    0,  // 52 (none)
    0,  // 53 (none)
    0,  // 54 (none)
    10, // 55 (none)
    10, // 56 Inverted Impulse Coaster
    10, // 57 Mini Roller Coaster
    10, // 58 Mine Ride
    10, // 59 (none)
    10, // 5a LIM Launched Roller Coaster
};

// Data at 0x0097D21E
const uint8_t rideBonusValue[RIDE_TYPE_COUNT] = {
    85,  // 00 Spiral Roller coaster
    90,  // 01 Stand Up Coaster
    90,  // 02 Suspended Swinging
    100, // 03 Inverted
    60,  // 04 Steel Mini Coaster
    50,  // 05 Mini Railway
    60,  // 06 Monorail
    50,  // 07 Mini Suspended Coaster
    40,  // 08 Boat Hire
    55,  // 09 Wooden Wild Mine/Mouse
    60,  // 0a Steeplechase/Motorbike/Soap
    50,  // 0b Car Ride
    65,  // 0c Launched Freefall
    75,  // 0d Bobsleigh Coaster
    45,  // 0e Observation Tower
    95,  // 0f Looping Roller Coaster
    55,  // 10 Dinghy Slide
    85,  // 11 Mine Train Coaster
    55,  // 12 Chairlift
    100, // 13 Corkscrew Roller Coaster
    40,  // 14 Maze
    40,  // 15 Spiral Slide
    55,  // 16 Go Karts
    65,  // 17 Log Flume
    70,  // 18 River Rapids
    35,  // 19 Dodgems
    35,  // 1a Pirate Ship
    35,  // 1b Swinging Inverter Ship
    15,  // 1c Food Stall
    15,  // 1d (none)
    15,  // 1e Drink Stall
    15,  // 1f (none)
    15,  // 20 Shop (all types)
    45,  // 21 Merry Go Round
    15,  // 22 Balloon Stall (maybe)
    15,  // 23 Information Kiosk
    5,   // 24 Bathroom
    45,  // 25 Ferris Wheel
    45,  // 26 Motion Simulator
    45,  // 27 3D Cinema
    55,  // 28 Topspin
    30,  // 29 Space Rings
    70,  // 2a Reverse Freefall Coaster
    45,  // 2b Elevator
    95,  // 2c Vertical Drop Roller Coaster
    5,   // 2d ATM
    40,  // 2e Twist
    22,  // 2f Haunted House
    5,   // 30 First Aid
    39,  // 31 Circus Show
    50,  // 32 Ghost Train
    120, // 33 Twister Roller Coaster
    105, // 34 Wooden Roller Coaster
    65,  // 35 Side-Friction Roller Coaster
    55,  // 36 Wild Mouse
    100, // 37 Multi Dimension Coaster
    100, // 38 (none)
    100, // 39 Flying Roller Coaster
    100, // 3a (none)
    50,  // 3b Virginia Reel
    65,  // 3c Splash Boats
    45,  // 3d Mini Helicopters
    100, // 3e Lay-down Roller Coaster
    60,  // 3f Suspended Monorail
    100, // 40 (none)
    65,  // 41 Reverser Roller Coaster
    35,  // 42 Heartline Twister Roller Coaster
    23,  // 43 Mini Golf
    120, // 44 Giga Coaster
    45,  // 45 Roto-Drop
    35,  // 46 Flying Saucers
    22,  // 47 Crooked House
    45,  // 48 Monorail Cycles
    80,  // 49 Compact Inverted Coaster
    60,  // 4a Water Coaster
    70,  // 4b Air Powered Vertical Coaster
    55,  // 4c Inverted Hairpin Coaster
    35,  // 4d Magic Carpet
    40,  // 4e Submarine Ride
    65,  // 4f River Rafts
    15,  // 50 (none)
    45,  // 51 Enterprise
    15,  // 52 (none)
    15,  // 53 (none)
    15,  // 54 (none)
    100, // 55 (none)
    75,  // 56 Inverted Impulse Coaster
    60,  // 57 Mini Roller Coaster
    70,  // 58 Mine Ride
    55,  // 59 (none)
    55,  // 5a LIM Launched Roller Coaster
};

const rct_ride_name RideNaming[] =  {
    { STR_RIDE_NAME_SPIRAL_ROLLER_COASTER,          STR_RIDE_DESCRIPTION_SPIRAL_ROLLER_COASTER          }, // RIDE_TYPE_SPIRAL_ROLLER_COASTER
    { STR_RIDE_NAME_STAND_UP_ROLLER_COASTER,        STR_RIDE_DESCRIPTION_STAND_UP_ROLLER_COASTER        }, // RIDE_TYPE_STAND_UP_ROLLER_COASTER
    { STR_RIDE_NAME_SUSPENDED_SWINGING_COASTER,     STR_RIDE_DESCRIPTION_SUSPENDED_SWINGING_COASTER     }, // RIDE_TYPE_SUSPENDED_SWINGING_COASTER
    { STR_RIDE_NAME_INVERTED_ROLLER_COASTER,        STR_RIDE_DESCRIPTION_INVERTED_ROLLER_COASTER        }, // RIDE_TYPE_INVERTED_ROLLER_COASTER
    { STR_RIDE_NAME_JUNIOR_ROLLER_COASTER,          STR_RIDE_DESCRIPTION_JUNIOR_ROLLER_COASTER          }, // RIDE_TYPE_JUNIOR_ROLLER_COASTER
    { STR_RIDE_NAME_MINIATURE_RAILWAY,              STR_RIDE_DESCRIPTION_MINIATURE_RAILWAY              }, // RIDE_TYPE_MINIATURE_RAILWAY
    { STR_RIDE_NAME_MONORAIL,                       STR_RIDE_DESCRIPTION_MONORAIL                       }, // RIDE_TYPE_MONORAIL
    { STR_RIDE_NAME_MINI_SUSPENDED_COASTER,         STR_RIDE_DESCRIPTION_MINI_SUSPENDED_COASTER         }, // RIDE_TYPE_MINI_SUSPENDED_COASTER
    { STR_RIDE_NAME_BOAT_HIRE,                      STR_RIDE_DESCRIPTION_BOAT_HIRE                      }, // RIDE_TYPE_BOAT_HIRE
    { STR_RIDE_NAME_WOODEN_WILD_MOUSE,              STR_RIDE_DESCRIPTION_WOODEN_WILD_MOUSE              }, // RIDE_TYPE_WOODEN_WILD_MOUSE
    { STR_RIDE_NAME_STEEPLECHASE,                   STR_RIDE_DESCRIPTION_STEEPLECHASE                   }, // RIDE_TYPE_STEEPLECHASE
    { STR_RIDE_NAME_CAR_RIDE,                       STR_RIDE_DESCRIPTION_CAR_RIDE                       }, // RIDE_TYPE_CAR_RIDE
    { STR_RIDE_NAME_LAUNCHED_FREEFALL,              STR_RIDE_DESCRIPTION_LAUNCHED_FREEFALL              }, // RIDE_TYPE_LAUNCHED_FREEFALL
    { STR_RIDE_NAME_BOBSLEIGH_COASTER,              STR_RIDE_DESCRIPTION_BOBSLEIGH_COASTER              }, // RIDE_TYPE_BOBSLEIGH_COASTER
    { STR_RIDE_NAME_OBSERVATION_TOWER,              STR_RIDE_DESCRIPTION_OBSERVATION_TOWER              }, // RIDE_TYPE_OBSERVATION_TOWER
    { STR_RIDE_NAME_LOOPING_ROLLER_COASTER,         STR_RIDE_DESCRIPTION_LOOPING_ROLLER_COASTER         }, // RIDE_TYPE_LOOPING_ROLLER_COASTER
    { STR_RIDE_NAME_DINGHY_SLIDE,                   STR_RIDE_DESCRIPTION_DINGHY_SLIDE                   }, // RIDE_TYPE_DINGHY_SLIDE
    { STR_RIDE_NAME_MINE_TRAIN_COASTER,             STR_RIDE_DESCRIPTION_MINE_TRAIN_COASTER             }, // RIDE_TYPE_MINE_TRAIN_COASTER
    { STR_RIDE_NAME_CHAIRLIFT,                      STR_RIDE_DESCRIPTION_CHAIRLIFT                      }, // RIDE_TYPE_CHAIRLIFT
    { STR_RIDE_NAME_CORKSCREW_ROLLER_COASTER,       STR_RIDE_DESCRIPTION_CORKSCREW_ROLLER_COASTER       }, // RIDE_TYPE_CORKSCREW_ROLLER_COASTER
    { STR_RIDE_NAME_MAZE,                           STR_RIDE_DESCRIPTION_MAZE                           }, // RIDE_TYPE_MAZE
    { STR_RIDE_NAME_SPIRAL_SLIDE,                   STR_RIDE_DESCRIPTION_SPIRAL_SLIDE                   }, // RIDE_TYPE_SPIRAL_SLIDE
    { STR_RIDE_NAME_GO_KARTS,                       STR_RIDE_DESCRIPTION_GO_KARTS                       }, // RIDE_TYPE_GO_KARTS
    { STR_RIDE_NAME_LOG_FLUME,                      STR_RIDE_DESCRIPTION_LOG_FLUME                      }, // RIDE_TYPE_LOG_FLUME
    { STR_RIDE_NAME_RIVER_RAPIDS,                   STR_RIDE_DESCRIPTION_RIVER_RAPIDS                   }, // RIDE_TYPE_RIVER_RAPIDS
    { STR_RIDE_NAME_DODGEMS,                        STR_RIDE_DESCRIPTION_DODGEMS                        }, // RIDE_TYPE_DODGEMS
    { STR_RIDE_NAME_PIRATE_SHIP,                    STR_RIDE_DESCRIPTION_PIRATE_SHIP                    }, // RIDE_TYPE_SWINGING_SHIP
    { STR_RIDE_NAME_SWINGING_INVERTER_SHIP,         STR_RIDE_DESCRIPTION_SWINGING_INVERTER_SHIP         }, // RIDE_TYPE_SWINGING_INVERTER_SHIP
    { STR_RIDE_NAME_FOOD_STALL,                     STR_RIDE_DESCRIPTION_FOOD_STALL                     }, // RIDE_TYPE_FOOD_STALL
    { STR_RIDE_NAME_1D,                             STR_RIDE_DESCRIPTION_UNKNOWN                        }, // RIDE_TYPE_1D
    { STR_RIDE_NAME_DRINK_STALL,                    STR_RIDE_DESCRIPTION_DRINK_STALL                    }, // RIDE_TYPE_DRINK_STALL
    { STR_RIDE_NAME_1F,                             STR_RIDE_DESCRIPTION_UNKNOWN                        }, // RIDE_TYPE_1F
    { STR_RIDE_NAME_SHOP,                           STR_RIDE_DESCRIPTION_SHOP                           }, // RIDE_TYPE_SHOP
    { STR_RIDE_NAME_MERRY_GO_ROUND,                 STR_RIDE_DESCRIPTION_MERRY_GO_ROUND                 }, // RIDE_TYPE_MERRY_GO_ROUND
    { STR_RIDE_NAME_22,                             STR_RIDE_DESCRIPTION_UNKNOWN                        }, // RIDE_TYPE_22
    { STR_RIDE_NAME_INFORMATION_KIOSK,              STR_RIDE_DESCRIPTION_INFORMATION_KIOSK              }, // RIDE_TYPE_INFORMATION_KIOSK
    { STR_RIDE_NAME_TOILETS,                        STR_RIDE_DESCRIPTION_TOILETS                        }, // RIDE_TYPE_TOILETS
    { STR_RIDE_NAME_FERRIS_WHEEL,                   STR_RIDE_DESCRIPTION_FERRIS_WHEEL                   }, // RIDE_TYPE_FERRIS_WHEEL
    { STR_RIDE_NAME_MOTION_SIMULATOR,               STR_RIDE_DESCRIPTION_MOTION_SIMULATOR               }, // RIDE_TYPE_MOTION_SIMULATOR
    { STR_RIDE_NAME_3D_CINEMA,                      STR_RIDE_DESCRIPTION_3D_CINEMA                      }, // RIDE_TYPE_3D_CINEMA
    { STR_RIDE_NAME_TOP_SPIN,                       STR_RIDE_DESCRIPTION_TOP_SPIN                       }, // RIDE_TYPE_TOP_SPIN
    { STR_RIDE_NAME_SPACE_RINGS,                    STR_RIDE_DESCRIPTION_SPACE_RINGS                    }, // RIDE_TYPE_SPACE_RINGS
    { STR_RIDE_NAME_REVERSE_FREEFALL_COASTER,       STR_RIDE_DESCRIPTION_REVERSE_FREEFALL_COASTER       }, // RIDE_TYPE_REVERSE_FREEFALL_COASTER
    { STR_RIDE_NAME_LIFT,                           STR_RIDE_DESCRIPTION_LIFT                           }, // RIDE_TYPE_LIFT
    { STR_RIDE_NAME_VERTICAL_DROP_ROLLER_COASTER,   STR_RIDE_DESCRIPTION_VERTICAL_DROP_ROLLER_COASTER   }, // RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER
    { STR_RIDE_NAME_CASH_MACHINE,                   STR_RIDE_DESCRIPTION_CASH_MACHINE                   }, // RIDE_TYPE_CASH_MACHINE
    { STR_RIDE_NAME_TWIST,                          STR_RIDE_DESCRIPTION_TWIST                          }, // RIDE_TYPE_TWIST
    { STR_RIDE_NAME_HAUNTED_HOUSE,                  STR_RIDE_DESCRIPTION_HAUNTED_HOUSE                  }, // RIDE_TYPE_HAUNTED_HOUSE
    { STR_RIDE_NAME_FIRST_AID,                      STR_RIDE_DESCRIPTION_FIRST_AID                      }, // RIDE_TYPE_FIRST_AID
    { STR_RIDE_NAME_CIRCUS_SHOW,                    STR_RIDE_DESCRIPTION_CIRCUS_SHOW                    }, // RIDE_TYPE_CIRCUS
    { STR_RIDE_NAME_GHOST_TRAIN,                    STR_RIDE_DESCRIPTION_GHOST_TRAIN                    }, // RIDE_TYPE_GHOST_TRAIN
    { STR_RIDE_NAME_TWISTER_ROLLER_COASTER,         STR_RIDE_DESCRIPTION_TWISTER_ROLLER_COASTER         }, // RIDE_TYPE_TWISTER_ROLLER_COASTER
    { STR_RIDE_NAME_WOODEN_ROLLER_COASTER,          STR_RIDE_DESCRIPTION_WOODEN_ROLLER_COASTER          }, // RIDE_TYPE_WOODEN_ROLLER_COASTER
    { STR_RIDE_NAME_SIDE_FRICTION_ROLLER_COASTER,   STR_RIDE_DESCRIPTION_SIDE_FRICTION_ROLLER_COASTER   }, // RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER
    { STR_RIDE_NAME_WILD_MOUSE,                     STR_RIDE_DESCRIPTION_WILD_MOUSE                     }, // RIDE_TYPE_STEEL_WILD_MOUSE
    { STR_RIDE_NAME_MULTI_DIMENSION_ROLLER_COASTER, STR_RIDE_DESCRIPTION_MULTI_DIMENSION_ROLLER_COASTER }, // RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER
    { STR_RIDE_NAME_38,                             STR_RIDE_DESCRIPTION_UNKNOWN                        }, // RIDE_TYPE_38
    { STR_RIDE_NAME_FLYING_ROLLER_COASTER,          STR_RIDE_DESCRIPTION_FLYING_ROLLER_COASTER          }, // RIDE_TYPE_FLYING_ROLLER_COASTER
    { STR_RIDE_NAME_3A,                             STR_RIDE_DESCRIPTION_UNKNOWN                        }, // RIDE_TYPE_3A
    { STR_RIDE_NAME_VIRGINIA_REEL,                  STR_RIDE_DESCRIPTION_VIRGINIA_REEL                  }, // RIDE_TYPE_VIRGINIA_REEL
    { STR_RIDE_NAME_SPLASH_BOATS,                   STR_RIDE_DESCRIPTION_SPLASH_BOATS                   }, // RIDE_TYPE_SPLASH_BOATS
    { STR_RIDE_NAME_MINI_HELICOPTERS,               STR_RIDE_DESCRIPTION_MINI_HELICOPTERS               }, // RIDE_TYPE_MINI_HELICOPTERS
    { STR_RIDE_NAME_LAY_DOWN_ROLLER_COASTER,        STR_RIDE_DESCRIPTION_LAY_DOWN_ROLLER_COASTER        }, // RIDE_TYPE_LAY_DOWN_ROLLER_COASTER
    { STR_RIDE_NAME_SUSPENDED_MONORAIL,             STR_RIDE_DESCRIPTION_SUSPENDED_MONORAIL             }, // RIDE_TYPE_SUSPENDED_MONORAIL
    { STR_RIDE_NAME_40,                             STR_RIDE_DESCRIPTION_UNKNOWN                        }, // RIDE_TYPE_40
    { STR_RIDE_NAME_REVERSER_ROLLER_COASTER,        STR_RIDE_DESCRIPTION_REVERSER_ROLLER_COASTER        }, // RIDE_TYPE_REVERSER_ROLLER_COASTER
    { STR_RIDE_NAME_HEARTLINE_TWISTER_COASTER,      STR_RIDE_DESCRIPTION_HEARTLINE_TWISTER_COASTER      }, // RIDE_TYPE_HEARTLINE_TWISTER_COASTER
    { STR_RIDE_NAME_MINI_GOLF,                      STR_RIDE_DESCRIPTION_MINI_GOLF                      }, // RIDE_TYPE_MINI_GOLF
    { STR_RIDE_NAME_GIGA_COASTER,                   STR_RIDE_DESCRIPTION_GIGA_COASTER                   }, // RIDE_TYPE_GIGA_COASTER
    { STR_RIDE_NAME_ROTO_DROP,                      STR_RIDE_DESCRIPTION_ROTO_DROP                      }, // RIDE_TYPE_ROTO_DROP
    { STR_RIDE_NAME_FLYING_SAUCERS,                 STR_RIDE_DESCRIPTION_FLYING_SAUCERS                 }, // RIDE_TYPE_FLYING_SAUCERS
    { STR_RIDE_NAME_CROOKED_HOUSE,                  STR_RIDE_DESCRIPTION_CROOKED_HOUSE                  }, // RIDE_TYPE_CROOKED_HOUSE
    { STR_RIDE_NAME_MONORAIL_CYCLES,                STR_RIDE_DESCRIPTION_MONORAIL_CYCLES                }, // RIDE_TYPE_MONORAIL_CYCLES
    { STR_RIDE_NAME_COMPACT_INVERTED_COASTER,       STR_RIDE_DESCRIPTION_COMPACT_INVERTED_COASTER       }, // RIDE_TYPE_COMPACT_INVERTED_COASTER
    { STR_RIDE_NAME_WATER_COASTER,                  STR_RIDE_DESCRIPTION_WATER_COASTER                  }, // RIDE_TYPE_WATER_COASTER
    { STR_RIDE_NAME_AIR_POWERED_VERTICAL_COASTER,   STR_RIDE_DESCRIPTION_AIR_POWERED_VERTICAL_COASTER   }, // RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER
    { STR_RIDE_NAME_INVERTED_HAIRPIN_COASTER,       STR_RIDE_DESCRIPTION_INVERTED_HAIRPIN_COASTER       }, // RIDE_TYPE_INVERTED_HAIRPIN_COASTER
    { STR_RIDE_NAME_MAGIC_CARPET,                   STR_RIDE_DESCRIPTION_MAGIC_CARPET                   }, // RIDE_TYPE_MAGIC_CARPET
    { STR_RIDE_NAME_SUBMARINE_RIDE,                 STR_RIDE_DESCRIPTION_SUBMARINE_RIDE                 }, // RIDE_TYPE_SUBMARINE_RIDE
    { STR_RIDE_NAME_RIVER_RAFTS,                    STR_RIDE_DESCRIPTION_RIVER_RAFTS                    }, // RIDE_TYPE_RIVER_RAFTS
    { STR_RIDE_NAME_50,                             STR_RIDE_DESCRIPTION_UNKNOWN                        }, // RIDE_TYPE_50
    { STR_RIDE_NAME_ENTERPRISE,                     STR_RIDE_DESCRIPTION_ENTERPRISE                     }, // RIDE_TYPE_ENTERPRISE
    { STR_RIDE_NAME_52,                             STR_RIDE_DESCRIPTION_UNKNOWN                        }, // RIDE_TYPE_52
    { STR_RIDE_NAME_53,                             STR_RIDE_DESCRIPTION_UNKNOWN                        }, // RIDE_TYPE_53
    { STR_RIDE_NAME_54,                             STR_RIDE_DESCRIPTION_UNKNOWN                        }, // RIDE_TYPE_54
    { STR_RIDE_NAME_55,                             STR_RIDE_DESCRIPTION_UNKNOWN                        }, // RIDE_TYPE_55
    { STR_RIDE_NAME_INVERTED_IMPULSE_COASTER,       STR_RIDE_DESCRIPTION_INVERTED_IMPULSE_COASTER       }, // RIDE_TYPE_INVERTED_IMPULSE_COASTER
    { STR_RIDE_NAME_MINI_ROLLER_COASTER,            STR_RIDE_DESCRIPTION_MINI_ROLLER_COASTER            }, // RIDE_TYPE_MINI_ROLLER_COASTER
    { STR_RIDE_NAME_MINE_RIDE,                      STR_RIDE_DESCRIPTION_MINE_RIDE                      }, // RIDE_TYPE_MINE_RIDE
    { STR_RIDE_NAME_59,                             STR_RIDE_DESCRIPTION_UNKNOWN                        }, // RIDE_TYPE_59
    { STR_RIDE_NAME_LIM_LAUNCHED_ROLLER_COASTER,    STR_RIDE_DESCRIPTION_LIM_LAUNCHED_ROLLER_COASTER    }, // RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER
};

/**
 *
 *  rct2: 0x0097C8AC
 */
const uint8_t RideAvailableModes[] = {
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, 0xFF,                                                                       // 00 Spiral Roller coaster
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE, RIDE_MODE_POWERED_LAUNCH_PASSTROUGH, RIDE_MODE_POWERED_LAUNCH, 0xFF,                                                                       // 01 Stand Up Coaster
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, 0xFF,                                                                       // 02 Suspended Swinging
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, RIDE_MODE_POWERED_LAUNCH_PASSTROUGH, RIDE_MODE_POWERED_LAUNCH, 0xFF,        // 03 Inverted
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE, 0xFF,                           // 04 Steel Mini Coaster
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_SHUTTLE, 0xFF,                                                                                                  // 05 Mini Railway
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_SHUTTLE, 0xFF,                                                                                                  // 06 Monorail
    RIDE_MODE_CONTINUOUS_CIRCUIT, 0xFF,                                                                                                                     // 07 Mini Suspended Coaster
    RIDE_MODE_BOAT_HIRE, 0xFF,                                                                                                                              // 08 Boat Hire
    RIDE_MODE_CONTINUOUS_CIRCUIT, 0xFF,                                                                                                                     // 09 Wooden Wild Mine/Mouse
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, 0xFF,                                                                       // 0A Steeplechase/Motorbike/Soap Box Derby
    RIDE_MODE_CONTINUOUS_CIRCUIT, 0xFF,                                                                                                                     // 0B Car Ride
    RIDE_MODE_UPWARD_LAUNCH, RIDE_MODE_DOWNWARD_LAUNCH, 0xFF,                                                                                               // 0C Launched Freefall
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, 0xFF,                                                                       // 0D Bobsleigh Coaster
    RIDE_MODE_ROTATING_LIFT, 0xFF,                                                                                                                          // 0E Observation Tower
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE, RIDE_MODE_POWERED_LAUNCH_PASSTROUGH, RIDE_MODE_POWERED_LAUNCH, 0xFF,    // 0F Looping Roller Coaster
    RIDE_MODE_CONTINUOUS_CIRCUIT, 0xFF,                                                                                                                     // 10 Dinghy Slide
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, 0xFF,                                                                       // 11 Mine Train Coaster
    RIDE_MODE_STATION_TO_STATION, 0xFF,                                                                                                                     // 12 Chairlift
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, RIDE_MODE_POWERED_LAUNCH_PASSTROUGH, RIDE_MODE_POWERED_LAUNCH, RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE, 0xFF,                                                                // 13 Corkscrew Roller Coaster
    RIDE_MODE_MAZE, 0xFF,                                                                                                                                   // 14 Maze
    RIDE_MODE_SINGLE_RIDE_PER_ADMISSION, RIDE_MODE_UNLIMITED_RIDES_PER_ADMISSION, 0xFF,                                                                     // 15 Spiral Slide
    RIDE_MODE_RACE, RIDE_MODE_CONTINUOUS_CIRCUIT, 0xFF,                                                                                                     // 16 Go Karts
    RIDE_MODE_CONTINUOUS_CIRCUIT, 0xFF,                                                                                                                     // 17 Log Flume
    RIDE_MODE_CONTINUOUS_CIRCUIT, 0xFF,                                                                                                                     // 18 River Rapids
    RIDE_MODE_BUMPERCAR, 0xFF,                                                                                                                              // 19 Dodgems
    RIDE_MODE_SWING, 0xFF,                                                                                                                                  // 1A Pirate Ship
    RIDE_MODE_SWING, 0xFF,                                                                                                                                  // 1B Swinging Inverter Ship
    RIDE_MODE_SHOP_STALL, 0xFF,                                                                                                                             // 1C Food Stall
    RIDE_MODE_SHOP_STALL, 0xFF,                                                                                                                             // 1D (none)
    RIDE_MODE_SHOP_STALL, 0xFF,                                                                                                                             // 1E Drink Stall
    RIDE_MODE_SHOP_STALL, 0xFF,                                                                                                                             // 1F (none)
    RIDE_MODE_SHOP_STALL, 0xFF,                                                                                                                             // 20 Shop (all types)
    RIDE_MODE_ROTATION, 0xFF,                                                                                                                               // 21 Merry Go Round
    RIDE_MODE_SHOP_STALL, 0xFF,                                                                                                                             // 22 Balloon Stall (maybe)
    RIDE_MODE_SHOP_STALL, 0xFF,                                                                                                                             // 23 Information Kiosk
    RIDE_MODE_SHOP_STALL, 0xFF,                                                                                                                             // 24 Bathroom
    RIDE_MODE_FORWARD_ROTATION, RIDE_MODE_BACKWARD_ROTATION, 0xFF,                                                                                          // 25 Ferris Wheel
    RIDE_MODE_FILM_AVENGING_AVIATORS, RIDE_MODE_FILM_THRILL_RIDERS, 0xFF,                                                                                   // 26 Motion Simulator
    RIDE_MODE_3D_FILM_MOUSE_TAILS, RIDE_MODE_3D_FILM_STORM_CHASERS, RIDE_MODE_3D_FILM_SPACE_RAIDERS, 0xFF,                                                  // 27 3D Cinema
    RIDE_MODE_BEGINNERS, RIDE_MODE_INTENSE, RIDE_MODE_BERSERK, 0xFF,                                                                                        // 28 Topspin
    RIDE_MODE_SPACE_RINGS, 0xFF,                                                                                                                            // 29 Space Rings
    RIDE_MODE_LIM_POWERED_LAUNCH, 0xFF,                                                                                                                     // 2A Reverse Freefall Coaster
    RIDE_MODE_SHUTTLE, 0xFF,                                                                                                                                // 2B Elevator
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, 0xFF,                                                                       // 2C Vertical Drop Roller Coaster
    RIDE_MODE_SHOP_STALL, 0xFF,                                                                                                                             // 2D ATM
    RIDE_MODE_ROTATION, 0xFF,                                                                                                                               // 2E Twist
    RIDE_MODE_HAUNTED_HOUSE, 0xFF,                                                                                                                          // 2F Haunted House
    RIDE_MODE_SHOP_STALL, 0xFF,                                                                                                                             // 30 First Aid
    RIDE_MODE_CIRCUS_SHOW, 0xFF,                                                                                                                            // 31 Circus Show
    RIDE_MODE_CONTINUOUS_CIRCUIT, 0xFF,                                                                                                                     // 32 Ghost Train
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, 0xFF,                                                                       // 33 Twister Roller Coaster
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE, 0xFF,                                                                       // 34 Wooden Roller Coaster
    RIDE_MODE_CONTINUOUS_CIRCUIT, 0xFF,                                                                                                                     // 35 Side-Friction Roller Coaster
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, 0xFF,                                                                       // 36 Wild Mouse
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, 0xFF,                                                                       // 37 Multi Dimension Coaster
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, 0xFF,                                                                       // 38 (none)
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, 0xFF,                                                                       // 39 Flying Roller Coaster
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, 0xFF,                                                                       // 3A (none)
    RIDE_MODE_CONTINUOUS_CIRCUIT, 0xFF,                                                                                                                     // 3B Virginia Reel
    RIDE_MODE_CONTINUOUS_CIRCUIT, 0xFF,                                                                                                                     // 3C Splash Boats
    RIDE_MODE_CONTINUOUS_CIRCUIT, 0xFF,                                                                                                                     // 3D Mini Helicopters
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, 0xFF,                                                                       // 3E Lay-down Roller Coaster
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_SHUTTLE, 0xFF,                                                                                                  // 3F Suspended Monorail
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, 0xFF,                                                                       // 40 (none)
    RIDE_MODE_CONTINUOUS_CIRCUIT, 0xFF,                                                                                                                     // 41 Reverser Roller Coaster
    RIDE_MODE_CONTINUOUS_CIRCUIT, 0xFF,                                                                                                                     // 42 Heartline Twister Roller Coaster
    RIDE_MODE_CONTINUOUS_CIRCUIT, 0xFF,                                                                                                                     // 43 Mini Golf
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, 0xFF,                                                                       // 44 Giga Coaster
    RIDE_MODE_FREEFALL_DROP, 0xFF,                                                                                                                          // 45 Roto-Drop
    RIDE_MODE_BUMPERCAR, 0xFF,                                                                                                                              // 46 Flying Saucers
    RIDE_MODE_CROOKED_HOUSE, 0xFF,                                                                                                                          // 47 Crooked House
    RIDE_MODE_CONTINUOUS_CIRCUIT, 0xFF,                                                                                                                     // 48 Monorail Cycles
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE, 0xFF,                                                                   // 49 Compact Inverted Coaster
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, 0xFF,                                                                       // 4A Water Coaster
    RIDE_MODE_POWERED_LAUNCH_PASSTROUGH, RIDE_MODE_POWERED_LAUNCH, 0xFF,                                                                                    // 4B Air Powered Vertical Coaster
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, 0xFF,                                                                       // 4C Inverted Hairpin Coaster
    RIDE_MODE_SWING, 0xFF,                                                                                                                                  // 4D Magic Carpet
    RIDE_MODE_CONTINUOUS_CIRCUIT, 0xFF,                                                                                                                     // 4E Submarine Ride
    RIDE_MODE_CONTINUOUS_CIRCUIT, 0xFF,                                                                                                                     // 4F River Rafts
    RIDE_MODE_SHOP_STALL, 0xFF,                                                                                                                             // 50 (none)
    RIDE_MODE_ROTATION, 0xFF,                                                                                                                               // 51 Enterprise
    RIDE_MODE_SHOP_STALL, 0xFF,                                                                                                                             // 52 (none)
    RIDE_MODE_SHOP_STALL, 0xFF,                                                                                                                             // 53 (none)
    RIDE_MODE_SHOP_STALL, 0xFF,                                                                                                                             // 54 (none)
    RIDE_MODE_CONTINUOUS_CIRCUIT, 0xFF,                                                                                                                     // 55 (none)
    RIDE_MODE_POWERED_LAUNCH_PASSTROUGH, RIDE_MODE_POWERED_LAUNCH, 0xFF,                                                                                    // 56 Inverted Impulse Coaster
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, 0xFF,                                                                       // 57 Mini Roller Coaster
    RIDE_MODE_CONTINUOUS_CIRCUIT, 0xFF,                                                                                                                     // 58 Mine Ride
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, 0xFF,                                                                       // 59 Unknown
    RIDE_MODE_POWERED_LAUNCH_PASSTROUGH, RIDE_MODE_POWERED_LAUNCH, RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED, 0xFF                                           // 5a LIM Launched Roller Coaster
};

const uint8_t AllRideModesAvailable[] = {
    RIDE_MODE_CONTINUOUS_CIRCUIT, RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE, RIDE_MODE_POWERED_LAUNCH_PASSTROUGH, RIDE_MODE_SHUTTLE, RIDE_MODE_NORMAL, RIDE_MODE_BOAT_HIRE, RIDE_MODE_UPWARD_LAUNCH, RIDE_MODE_ROTATING_LIFT, RIDE_MODE_STATION_TO_STATION, RIDE_MODE_SINGLE_RIDE_PER_ADMISSION, RIDE_MODE_UNLIMITED_RIDES_PER_ADMISSION, RIDE_MODE_MAZE, RIDE_MODE_RACE, RIDE_MODE_BUMPERCAR, RIDE_MODE_SWING, RIDE_MODE_SHOP_STALL, RIDE_MODE_ROTATION, RIDE_MODE_FORWARD_ROTATION, RIDE_MODE_BACKWARD_ROTATION, RIDE_MODE_FILM_AVENGING_AVIATORS, RIDE_MODE_3D_FILM_MOUSE_TAILS, RIDE_MODE_SPACE_RINGS, RIDE_MODE_BEGINNERS, RIDE_MODE_LIM_POWERED_LAUNCH, RIDE_MODE_FILM_THRILL_RIDERS, RIDE_MODE_3D_FILM_STORM_CHASERS, RIDE_MODE_3D_FILM_SPACE_RAIDERS, RIDE_MODE_INTENSE, RIDE_MODE_BERSERK, RIDE_MODE_HAUNTED_HOUSE, RIDE_MODE_CIRCUS_SHOW, RIDE_MODE_DOWNWARD_LAUNCH, RIDE_MODE_CROOKED_HOUSE, RIDE_MODE_FREEFALL_DROP, RIDE_MODE_POWERED_LAUNCH, RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED, 0xFF
};

// rct2: 0x0097D4F0,  0x0097D4F1,  0x0097D4F2, 0x0097D4F4, 0x0097D4F5
const rct_ride_data_4 RideData4[RIDE_TYPE_COUNT] = {
    {   20, 20, MUSIC_STYLE_ROCK_STYLE_3,       0                                               },  // RIDE_TYPE_SPIRAL_ROLLER_COASTER
    {   20, 20, MUSIC_STYLE_TECHNO,             0                                               },  // RIDE_TYPE_STAND_UP_ROLLER_COASTER
    {   20, 20, MUSIC_STYLE_ROCK,               0                                               },  // RIDE_TYPE_SUSPENDED_SWINGING_COASTER
    {   20, 20, MUSIC_STYLE_TECHNO,             0                                               },  // RIDE_TYPE_INVERTED_ROLLER_COASTER
    {   20, 20, MUSIC_STYLE_SUMMER,             0                                               },  // RIDE_TYPE_JUNIOR_ROLLER_COASTER
    {   10, 0,  MUSIC_STYLE_SUMMER,             0                                               },  // RIDE_TYPE_MINIATURE_RAILWAY
    {   10, 0,  MUSIC_STYLE_SUMMER,             0                                               },  // RIDE_TYPE_MONORAIL
    {   20, 20, MUSIC_STYLE_WILD_WEST,          0                                               },  // RIDE_TYPE_MINI_SUSPENDED_COASTER
    {   10, 0,  MUSIC_STYLE_WATER,              0                                               },  // RIDE_TYPE_BOAT_HIRE
    {   20, 20, MUSIC_STYLE_WILD_WEST,          0                                               },  // RIDE_TYPE_WOODEN_WILD_MOUSE
    {   20, 20, MUSIC_STYLE_ROCK,               0                                               },  // RIDE_TYPE_STEEPLECHASE
    {   15, 0,  MUSIC_STYLE_SUMMER,             0                                               },  // RIDE_TYPE_CAR_RIDE
    {   20, 0,  MUSIC_STYLE_TECHNO,             0                                               },  // RIDE_TYPE_LAUNCHED_FREEFALL
    {   20, 20, MUSIC_STYLE_ROCK,               0                                               },  // RIDE_TYPE_BOBSLEIGH_COASTER
    {   10, 0,  MUSIC_STYLE_SUMMER,             0                                               },  // RIDE_TYPE_OBSERVATION_TOWER
    {   20, 20, MUSIC_STYLE_ROCK,               0                                               },  // RIDE_TYPE_LOOPING_ROLLER_COASTER
    {   20, 20, MUSIC_STYLE_WATER,              0                                               },  // RIDE_TYPE_DINGHY_SLIDE
    {   20, 20, MUSIC_STYLE_WILD_WEST,          0                                               },  // RIDE_TYPE_MINE_TRAIN_COASTER
    {   10, 0,  MUSIC_STYLE_SUMMER,             0                                               },  // RIDE_TYPE_CHAIRLIFT
    {   20, 20, MUSIC_STYLE_ROCK,               0                                               },  // RIDE_TYPE_CORKSCREW_ROLLER_COASTER
    {   10, 0,  MUSIC_STYLE_SUMMER,             0                                               },  // RIDE_TYPE_MAZE
    {   15, 0,  MUSIC_STYLE_SUMMER,             0                                               },  // RIDE_TYPE_SPIRAL_SLIDE
    {   20, 0,  MUSIC_STYLE_TECHNO,             0                                               },  // RIDE_TYPE_GO_KARTS
    {   20, 20, MUSIC_STYLE_WATER,              0                                               },  // RIDE_TYPE_LOG_FLUME
    {   20, 20, MUSIC_STYLE_WATER,              0                                               },  // RIDE_TYPE_RIVER_RAPIDS
    {   15, 0,  MUSIC_STYLE_DODGEMS_BEAT,       0                                               },  // RIDE_TYPE_DODGEMS
    {   15, 0,  MUSIC_STYLE_PIRATES,            0                                               },  // RIDE_TYPE_SWINGING_SHIP
    {   15, 0,  MUSIC_STYLE_TECHNO,             0                                               },  // RIDE_TYPE_SWINGING_INVERTER_SHIP
    {   9,  0,  MUSIC_STYLE_GENTLE,             0                                               },  // RIDE_TYPE_FOOD_STALL
    {   15, 0,  MUSIC_STYLE_GENTLE,             0                                               },  // RIDE_TYPE_1D
    {   12, 0,  MUSIC_STYLE_GENTLE,             0                                               },  // RIDE_TYPE_DRINK_STALL
    {   8,  0,  MUSIC_STYLE_GENTLE,             0                                               },  // RIDE_TYPE_1F
    {   15, 0,  MUSIC_STYLE_GENTLE,             0                                               },  // RIDE_TYPE_SHOP
    {   10, 0,  MUSIC_STYLE_FAIRGROUND_ORGAN,   0                                               },  // RIDE_TYPE_MERRY_GO_ROUND
    {   9,  0,  MUSIC_STYLE_GENTLE,             0                                               },  // RIDE_TYPE_22
    {   6,  25, MUSIC_STYLE_GENTLE,             0                                               },  // RIDE_TYPE_INFORMATION_KIOSK
    {   0,  0,  MUSIC_STYLE_GENTLE,             0                                               },  // RIDE_TYPE_TOILETS
    {   10, 0,  MUSIC_STYLE_SUMMER,             0                                               },  // RIDE_TYPE_FERRIS_WHEEL
    {   20, 0,  MUSIC_STYLE_ROCK,               0                                               },  // RIDE_TYPE_MOTION_SIMULATOR
    {   20, 0,  MUSIC_STYLE_ROCK,               0                                               },  // RIDE_TYPE_3D_CINEMA
    {   20, 0,  MUSIC_STYLE_TECHNO,             0                                               },  // RIDE_TYPE_TOP_SPIN
    {   5,  0,  MUSIC_STYLE_GENTLE,             0                                               },  // RIDE_TYPE_SPACE_RINGS
    {   20, 0,  MUSIC_STYLE_ROCK,               0                                               },  // RIDE_TYPE_REVERSE_FREEFALL_COASTER
    {   10, 0,  MUSIC_STYLE_SUMMER,             0                                               },  // RIDE_TYPE_LIFT
    {   20, 20, MUSIC_STYLE_ROCK,               0                                               },  // RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER
    {   0,  0,  MUSIC_STYLE_GENTLE,             0                                               },  // RIDE_TYPE_CASH_MACHINE
    {   10, 0,  MUSIC_STYLE_GENTLE,             0                                               },  // RIDE_TYPE_TWIST
    {   10, 0,  MUSIC_STYLE_HORROR,             0                                               },  // RIDE_TYPE_HAUNTED_HOUSE
    {   0,  0,  MUSIC_STYLE_GENTLE,             0                                               },  // RIDE_TYPE_FIRST_AID
    {   15, 0,  MUSIC_STYLE_CIRCUS_SHOW,        0                                               },  // RIDE_TYPE_CIRCUS
    {   15, 0,  MUSIC_STYLE_HORROR,             0                                               },  // RIDE_TYPE_GHOST_TRAIN
    {   20, 20, MUSIC_STYLE_ROCK,               0                                               },  // RIDE_TYPE_TWISTER_ROLLER_COASTER
    {   20, 20, MUSIC_STYLE_WILD_WEST,          0                                               },  // RIDE_TYPE_WOODEN_ROLLER_COASTER
    {   20, 20, MUSIC_STYLE_GENTLE,             0                                               },  // RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER
    {   20, 20, MUSIC_STYLE_ROCK,               0                                               },  // RIDE_TYPE_STEEL_WILD_MOUSE
    {   20, 20, MUSIC_STYLE_ROCK_STYLE_3,       RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER_ALT    },  // RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER
    {   20, 20, MUSIC_STYLE_GENTLE,             0                                               },  // RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER_ALT
    {   20, 20, MUSIC_STYLE_ROCK,               RIDE_TYPE_FLYING_ROLLER_COASTER_ALT             },  // RIDE_TYPE_FLYING_ROLLER_COASTER
    {   20, 20, MUSIC_STYLE_GENTLE,             0                                               },  // RIDE_TYPE_FLYING_ROLLER_COASTER_ALT
    {   20, 20, MUSIC_STYLE_ROCK,               0                                               },  // RIDE_TYPE_VIRGINIA_REEL
    {   20, 20, MUSIC_STYLE_WATER,              0                                               },  // RIDE_TYPE_SPLASH_BOATS
    {   15, 0,  MUSIC_STYLE_SUMMER,             0                                               },  // RIDE_TYPE_MINI_HELICOPTERS
    {   20, 20, MUSIC_STYLE_ROCK,               RIDE_TYPE_LAY_DOWN_ROLLER_COASTER_ALT           },  // RIDE_TYPE_LAY_DOWN_ROLLER_COASTER
    {   10, 0,  MUSIC_STYLE_SUMMER,             0                                               },  // RIDE_TYPE_SUSPENDED_MONORAIL
    {   20, 20, MUSIC_STYLE_GENTLE,             0                                               },  // RIDE_TYPE_LAY_DOWN_ROLLER_COASTER_ALT
    {   20, 20, MUSIC_STYLE_TECHNO,             0                                               },  // RIDE_TYPE_REVERSER_ROLLER_COASTER
    {   15, 20, MUSIC_STYLE_TECHNO,             0                                               },  // RIDE_TYPE_HEARTLINE_TWISTER_COASTER
    {   10, 0,  MUSIC_STYLE_SUMMER,             0                                               },  // RIDE_TYPE_MINI_GOLF
    {   20, 20, MUSIC_STYLE_ROCK_STYLE_3,       0                                               },  // RIDE_TYPE_GIGA_COASTER
    {   20, 0,  MUSIC_STYLE_ROCK,               0                                               },  // RIDE_TYPE_ROTO_DROP
    {   15, 0,  MUSIC_STYLE_ROCK,               0                                               },  // RIDE_TYPE_FLYING_SAUCERS
    {   10, 0,  MUSIC_STYLE_GENTLE,             0                                               },  // RIDE_TYPE_CROOKED_HOUSE
    {   15, 0,  MUSIC_STYLE_SUMMER,             0                                               },  // RIDE_TYPE_MONORAIL_CYCLES
    {   20, 20, MUSIC_STYLE_TECHNO,             0                                               },  // RIDE_TYPE_COMPACT_INVERTED_COASTER
    {   20, 20, MUSIC_STYLE_WATER,              0                                               },  // RIDE_TYPE_WATER_COASTER
    {   20, 0,  MUSIC_STYLE_ROCK_STYLE_2,       0                                               },  // RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER
    {   20, 20, MUSIC_STYLE_ROCK_STYLE_2,       0                                               },  // RIDE_TYPE_INVERTED_HAIRPIN_COASTER
    {   15, 0,  MUSIC_STYLE_EGYPTIAN,           0                                               },  // RIDE_TYPE_MAGIC_CARPET
    {   10, 0,  MUSIC_STYLE_WATER,              0                                               },  // RIDE_TYPE_SUBMARINE_RIDE
    {   10, 20, MUSIC_STYLE_GENTLE,             0                                               },  // RIDE_TYPE_RIVER_RAFTS
    {   7,  0,  MUSIC_STYLE_GENTLE,             0                                               },  // RIDE_TYPE_50
    {   20, 0,  MUSIC_STYLE_ROCK_STYLE_2,       0                                               },  // RIDE_TYPE_ENTERPRISE
    {   12, 0,  MUSIC_STYLE_GENTLE,             0                                               },  // RIDE_TYPE_52
    {   15, 0,  MUSIC_STYLE_GENTLE,             0                                               },  // RIDE_TYPE_53
    {   12, 0,  MUSIC_STYLE_GENTLE,             0                                               },  // RIDE_TYPE_54
    {   20, 20, MUSIC_STYLE_ROCK,               0                                               },  // RIDE_TYPE_55
    {   20, 20, MUSIC_STYLE_ROCK,               0                                               },  // RIDE_TYPE_INVERTED_IMPULSE_COASTER
    {   20, 20, MUSIC_STYLE_SUMMER,             0                                               },  // RIDE_TYPE_MINI_ROLLER_COASTER
    {   20, 20, MUSIC_STYLE_WILD_WEST,          0                                               },  // RIDE_TYPE_MINE_RIDE
    {   20, 20, MUSIC_STYLE_ROCK_STYLE_2,       0                                               },  // RIDE_TYPE_59
    {   20, 20, MUSIC_STYLE_ROCK,               0                                               },  // RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER
};

// rct2: 0x0097DD78
const ride_cost RideTrackCosts[RIDE_TYPE_COUNT] =   {
    {   70,     5   },  // RIDE_TYPE_SPIRAL_ROLLER_COASTER
    {   100,    5   },  // RIDE_TYPE_STAND_UP_ROLLER_COASTER
    {   95,     5   },  // RIDE_TYPE_SUSPENDED_SWINGING_COASTER
    {   110,    5   },  // RIDE_TYPE_INVERTED_ROLLER_COASTER
    {   45,     4   },  // RIDE_TYPE_JUNIOR_ROLLER_COASTER
    {   35,     6   },  // RIDE_TYPE_MINIATURE_RAILWAY
    {   42,     5   },  // RIDE_TYPE_MONORAIL
    {   60,     5   },  // RIDE_TYPE_MINI_SUSPENDED_COASTER
    {   55,     5   },  // RIDE_TYPE_BOAT_HIRE
    {   50,     6   },  // RIDE_TYPE_WOODEN_WILD_MOUSE
    {   57,     4   },  // RIDE_TYPE_STEEPLECHASE
    {   25,     5   },  // RIDE_TYPE_CAR_RIDE
    {   50,     0   },  // RIDE_TYPE_LAUNCHED_FREEFALL
    {   80,     5   },  // RIDE_TYPE_BOBSLEIGH_COASTER
    {   37,     0   },  // RIDE_TYPE_OBSERVATION_TOWER
    {   90,     5   },  // RIDE_TYPE_LOOPING_ROLLER_COASTER
    {   40,     4   },  // RIDE_TYPE_DINGHY_SLIDE
    {   82,     6   },  // RIDE_TYPE_MINE_TRAIN_COASTER
    {   65,     1   },  // RIDE_TYPE_CHAIRLIFT
    {   105,    5   },  // RIDE_TYPE_CORKSCREW_ROLLER_COASTER
    {   55,     2   },  // RIDE_TYPE_MAZE
    {   165,    2   },  // RIDE_TYPE_SPIRAL_SLIDE
    {   62,     4   },  // RIDE_TYPE_GO_KARTS
    {   45,     5   },  // RIDE_TYPE_LOG_FLUME
    {   62,     6   },  // RIDE_TYPE_RIVER_RAPIDS
    {   55,     2   },  // RIDE_TYPE_DODGEMS
    {   155,    2   },  // RIDE_TYPE_SWINGING_SHIP
    {   212,    2   },  // RIDE_TYPE_SWINGING_INVERTER_SHIP
    {   600,    0   },  // RIDE_TYPE_FOOD_STALL
    {   700,    0   },  // RIDE_TYPE_1D
    {   500,    0   },  // RIDE_TYPE_DRINK_STALL
    {   600,    0   },  // RIDE_TYPE_1F
    {   400,    0   },  // RIDE_TYPE_SHOP
    {   115,    2   },  // RIDE_TYPE_MERRY_GO_ROUND
    {   500,    0   },  // RIDE_TYPE_22
    {   500,    0   },  // RIDE_TYPE_INFORMATION_KIOSK
    {   450,    0   },  // RIDE_TYPE_TOILETS
    {   225,    2   },  // RIDE_TYPE_FERRIS_WHEEL
    {   220,    2   },  // RIDE_TYPE_MOTION_SIMULATOR
    {   140,    2   },  // RIDE_TYPE_3D_CINEMA
    {   145,    2   },  // RIDE_TYPE_TOP_SPIN
    {   72,     2   },  // RIDE_TYPE_SPACE_RINGS
    {   200,    0   },  // RIDE_TYPE_REVERSE_FREEFALL_COASTER
    {   39,     0   },  // RIDE_TYPE_LIFT
    {   120,    5   },  // RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER
    {   400,    0   },  // RIDE_TYPE_CASH_MACHINE
    {   90,     2   },  // RIDE_TYPE_TWIST
    {   85,     2   },  // RIDE_TYPE_HAUNTED_HOUSE
    {   500,    0   },  // RIDE_TYPE_FIRST_AID
    {   125,    2   },  // RIDE_TYPE_CIRCUS
    {   26,     5   },  // RIDE_TYPE_GHOST_TRAIN
    {   120,    5   },  // RIDE_TYPE_TWISTER_ROLLER_COASTER
    {   75,     7   },  // RIDE_TYPE_WOODEN_ROLLER_COASTER
    {   55,     6   },  // RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER
    {   55,     4   },  // RIDE_TYPE_STEEL_WILD_MOUSE
    {   180,    5   },  // RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER
    {   180,    5   },  // RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER_ALT
    {   125,    5   },  // RIDE_TYPE_FLYING_ROLLER_COASTER
    {   125,    5   },  // RIDE_TYPE_FLYING_ROLLER_COASTER_ALT
    {   53,     6   },  // RIDE_TYPE_VIRGINIA_REEL
    {   57,     5   },  // RIDE_TYPE_SPLASH_BOATS
    {   25,     5   },  // RIDE_TYPE_MINI_HELICOPTERS
    {   107,    5   },  // RIDE_TYPE_LAY_DOWN_ROLLER_COASTER
    {   65,     5   },  // RIDE_TYPE_SUSPENDED_MONORAIL
    {   107,    5   },  // RIDE_TYPE_LAY_DOWN_ROLLER_COASTER_ALT
    {   55,     6   },  // RIDE_TYPE_REVERSER_ROLLER_COASTER
    {   135,    7   },  // RIDE_TYPE_HEARTLINE_TWISTER_COASTER
    {   50,     7   },  // RIDE_TYPE_MINI_GOLF
    {   120,    5   },  // RIDE_TYPE_GIGA_COASTER
    {   55,     0   },  // RIDE_TYPE_ROTO_DROP
    {   70,     4   },  // RIDE_TYPE_FLYING_SAUCERS
    {   65,     2   },  // RIDE_TYPE_CROOKED_HOUSE
    {   20,     5   },  // RIDE_TYPE_MONORAIL_CYCLES
    {   100,    5   },  // RIDE_TYPE_COMPACT_INVERTED_COASTER
    {   55,     4   },  // RIDE_TYPE_WATER_COASTER
    {   200,    0   },  // RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER
    {   67,     4   },  // RIDE_TYPE_INVERTED_HAIRPIN_COASTER
    {   198,    2   },  // RIDE_TYPE_MAGIC_CARPET
    {   70,     0   },  // RIDE_TYPE_SUBMARINE_RIDE
    {   40,     5   },  // RIDE_TYPE_RIVER_RAFTS
    {   525,    0   },  // RIDE_TYPE_50
    {   100,    2   },  // RIDE_TYPE_ENTERPRISE
    {   500,    0   },  // RIDE_TYPE_52
    {   520,    0   },  // RIDE_TYPE_53
    {   420,    0   },  // RIDE_TYPE_54
    {   105,    5   },  // RIDE_TYPE_55
    {   125,    5   },  // RIDE_TYPE_INVERTED_IMPULSE_COASTER
    {   65,     4   },  // RIDE_TYPE_MINI_ROLLER_COASTER
    {   85,     4   },  // RIDE_TYPE_MINE_RIDE
    {   55,     4   },  // RIDE_TYPE_59
    {   95,     5   },  // RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER
};

// 0x0097D218
const rct_ride_data_5 RideData5[RIDE_TYPE_COUNT] = {
    {   19,     24,     9,      31,     11,     50,     85,     0   },  // RIDE_TYPE_SPIRAL_ROLLER_COASTER
    {   25,     24,     9,      18,     11,     50,     90,     0   },  // RIDE_TYPE_STAND_UP_ROLLER_COASTER
    {   24,     40,     29,     26,     8,      50,     90,     0   },  // RIDE_TYPE_SUSPENDED_SWINGING_COASTER
    {   38,     40,     29,     27,     8,      50,     100,    0   },  // RIDE_TYPE_INVERTED_ROLLER_COASTER
    {   15,     24,     4,      18,     7,      40,     60,     0   },  // RIDE_TYPE_JUNIOR_ROLLER_COASTER
    {   7,      32,     5,      39,     9,      50,     50,     0   },  // RIDE_TYPE_MINIATURE_RAILWAY
    {   8,      32,     8,      78,     9,      50,     60,     0   },  // RIDE_TYPE_MONORAIL
    {   10,     24,     24,     3,      8,      40,     50,     0   },  // RIDE_TYPE_MINI_SUSPENDED_COASTER
    {   255,    16,     0,      255,    3,      5,      40,     0   },  // RIDE_TYPE_BOAT_HIRE
    {   14,     24,     4,      4,      7,      40,     55,     0   },  // RIDE_TYPE_WOODEN_WILD_MOUSE
    {   14,     24,     7,      4,      7,      40,     60,     0   },  // RIDE_TYPE_STEEPLECHASE
    {   6,      24,     4,      2,      7,      30,     50,     0   },  // RIDE_TYPE_CAR_RIDE
    {   255,    32,     3,      15,     2,      4,      65,     0   },  // RIDE_TYPE_LAUNCHED_FREEFALL
    {   19,     24,     5,      25,     7,      45,     75,     0   },  // RIDE_TYPE_BOBSLEIGH_COASTER
    {   255,    32,     3,      15,     2,      4,      45,     0   },  // RIDE_TYPE_OBSERVATION_TOWER
    {   35,     24,     5,      18,     7,      50,     95,     0   },  // RIDE_TYPE_LOOPING_ROLLER_COASTER
    {   15,     24,     5,      5,      7,      40,     55,     0   },  // RIDE_TYPE_DINGHY_SLIDE
    {   21,     24,     4,      15,     7,      50,     85,     0   },  // RIDE_TYPE_MINE_TRAIN_COASTER
    {   40,     32,     28,     18,     2,      30,     55,     0   },  // RIDE_TYPE_CHAIRLIFT
    {   28,     24,     8,      18,     11,     50,     100,    0   },  // RIDE_TYPE_CORKSCREW_ROLLER_COASTER
    {   6,      24,     0,      18,     1,      8,      40,     0   },  // RIDE_TYPE_MAZE
    {   15,     128,    0,      255,    2,      1,      40,     0   },  // RIDE_TYPE_SPIRAL_SLIDE
    {   7,      24,     2,      255,    1,      20,     55,     0   },  // RIDE_TYPE_GO_KARTS
    {   9,      24,     7,      255,    9,      40,     65,     0   },  // RIDE_TYPE_LOG_FLUME
    {   9,      32,     14,     255,    15,     40,     70,     0   },  // RIDE_TYPE_RIVER_RAPIDS
    {   9,      48,     2,      255,    2,      1,      35,     0   },  // RIDE_TYPE_DODGEMS
    {   12,     112,    7,      255,    11,     1,      35,     0   },  // RIDE_TYPE_SWINGING_SHIP
    {   15,     176,    7,      255,    11,     1,      35,     0   },  // RIDE_TYPE_SWINGING_INVERTER_SHIP
    {   12,     64,     0,      255,    0,      1,      15,     0   },  // RIDE_TYPE_FOOD_STALL
    {   12,     64,     0,      255,    0,      1,      15,     0   },  // RIDE_TYPE_1D
    {   12,     64,     0,      255,    0,      1,      15,     0   },  // RIDE_TYPE_DRINK_STALL
    {   12,     64,     0,      255,    0,      1,      15,     0   },  // RIDE_TYPE_1F
    {   12,     64,     0,      255,    0,      1,      15,     0   },  // RIDE_TYPE_SHOP
    {   12,     64,     3,      255,    2,      1,      45,     0   },  // RIDE_TYPE_MERRY_GO_ROUND
    {   12,     64,     0,      255,    0,      1,      15,     0   },  // RIDE_TYPE_22
    {   12,     48,     0,      255,    0,      1,      15,     0   },  // RIDE_TYPE_INFORMATION_KIOSK
    {   12,     32,     0,      255,    0,      1,      5,      0   },  // RIDE_TYPE_TOILETS
    {   16,     176,    3,      255,    2,      1,      45,     0   },  // RIDE_TYPE_FERRIS_WHEEL
    {   12,     64,     3,      255,    2,      1,      45,     0   },  // RIDE_TYPE_MOTION_SIMULATOR
    {   12,     128,    3,      255,    2,      1,      45,     0   },  // RIDE_TYPE_3D_CINEMA
    {   16,     112,    3,      255,    2,      1,      55,     0   },  // RIDE_TYPE_TOP_SPIN
    {   16,     48,     3,      255,    2,      1,      30,     0   },  // RIDE_TYPE_SPACE_RINGS
    {   255,    32,     4,      255,    7,      45,     70,     0   },  // RIDE_TYPE_REVERSE_FREEFALL_COASTER
    {   255,    32,     3,      15,     2,      4,      45,     0   },  // RIDE_TYPE_LIFT
    {   53,     24,     8,      25,     11,     42,     95,     0   },  // RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER
    {   12,     32,     0,      255,    0,      1,      5,      0   },  // RIDE_TYPE_CASH_MACHINE
    {   12,     64,     3,      255,    2,      1,      40,     0   },  // RIDE_TYPE_TWIST
    {   16,     160,    3,      255,    2,      1,      22,     0   },  // RIDE_TYPE_HAUNTED_HOUSE
    {   12,     48,     0,      255,    0,      1,      5,      0   },  // RIDE_TYPE_FIRST_AID
    {   12,     128,    3,      255,    2,      1,      39,     0   },  // RIDE_TYPE_CIRCUS
    {   8,      24,     6,      2,      7,      30,     50,     0   },  // RIDE_TYPE_GHOST_TRAIN
    {   54,     24,     8,      31,     9,      55,     120,    0   },  // RIDE_TYPE_TWISTER_ROLLER_COASTER
    {   41,     24,     8,      19,     11,     50,     105,    0   },  // RIDE_TYPE_WOODEN_ROLLER_COASTER
    {   15,     24,     4,      15,     11,     37,     65,     0   },  // RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER
    {   16,     24,     4,      4,      7,      40,     55,     0   },  // RIDE_TYPE_STEEL_WILD_MOUSE
    {   40,     24,     8,      78,     11,     50,     100,    0   },  // RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER
    {   40,     24,     8,      78,     11,     50,     100,    0   },  // RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER_ALT
    {   30,     24,     8,      35,     11,     50,     100,    0   },  // RIDE_TYPE_FLYING_ROLLER_COASTER
    {   30,     24,     8,      35,     11,     50,     100,    0   },  // RIDE_TYPE_FLYING_ROLLER_COASTER_ALT
    {   14,     24,     6,      15,     7,      25,     50,     0   },  // RIDE_TYPE_VIRGINIA_REEL
    {   16,     24,     7,      255,    11,     30,     65,     0   },  // RIDE_TYPE_SPLASH_BOATS
    {   7,      24,     4,      2,      7,      30,     45,     0   },  // RIDE_TYPE_MINI_HELICOPTERS
    {   26,     24,     8,      25,     11,     50,     100,    0   },  // RIDE_TYPE_LAY_DOWN_ROLLER_COASTER
    {   12,     40,     32,     78,     8,      50,     60,     0   },  // RIDE_TYPE_SUSPENDED_MONORAIL
    {   26,     24,     8,      25,     11,     50,     100,    0   },  // RIDE_TYPE_LAY_DOWN_ROLLER_COASTER_ALT
    {   18,     24,     8,      15,     11,     37,     65,     0   },  // RIDE_TYPE_REVERSER_ROLLER_COASTER
    {   22,     24,     15,     18,     9,      30,     35,     0   },  // RIDE_TYPE_HEARTLINE_TWISTER_COASTER
    {   7,      32,     2,      255,    2,      20,     23,     0   },  // RIDE_TYPE_MINI_GOLF
    {   86,     24,     9,      31,     11,     55,     120,    0   },  // RIDE_TYPE_GIGA_COASTER
    {   255,    32,     3,      15,     2,      4,      45,     0   },  // RIDE_TYPE_ROTO_DROP
    {   9,      48,     2,      255,    2,      1,      35,     0   },  // RIDE_TYPE_FLYING_SAUCERS
    {   16,     96,     3,      255,    2,      1,      22,     0   },  // RIDE_TYPE_CROOKED_HOUSE
    {   5,      24,     8,      2,      7,      30,     45,     0   },  // RIDE_TYPE_MONORAIL_CYCLES
    {   27,     40,     29,     18,     8,      50,     80,     0   },  // RIDE_TYPE_COMPACT_INVERTED_COASTER
    {   18,     24,     4,      13,     7,      40,     60,     0   },  // RIDE_TYPE_WATER_COASTER
    {   255,    32,     4,      255,    7,      45,     70,     0   },  // RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER
    {   16,     24,     24,     4,      7,      40,     55,     0   },  // RIDE_TYPE_INVERTED_HAIRPIN_COASTER
    {   15,     176,    7,      255,    11,     1,      35,     0   },  // RIDE_TYPE_MAGIC_CARPET
    {   255,    16,     0,      255,    3,      5,      40,     0   },  // RIDE_TYPE_SUBMARINE_RIDE
    {   12,     24,     7,      255,    11,     30,     65,     0   },  // RIDE_TYPE_RIVER_RAFTS
    {   12,     64,     0,      255,    0,      1,      15,     0   },  // RIDE_TYPE_50
    {   16,     160,    3,      255,    2,      1,      45,     0   },  // RIDE_TYPE_ENTERPRISE
    {   12,     48,     0,      255,    0,      1,      15,     0   },  // RIDE_TYPE_52
    {   12,     64,     0,      255,    0,      1,      15,     0   },  // RIDE_TYPE_53
    {   12,     48,     0,      255,    0,      1,      15,     0   },  // RIDE_TYPE_54
    {   44,     24,     8,      18,     11,     50,     100,    0   },  // RIDE_TYPE_55
    {   45,     40,     29,     23,     8,      25,     75,     0   },  // RIDE_TYPE_INVERTED_IMPULSE_COASTER
    {   16,     24,     9,      10,     11,     40,     60,     0   },  // RIDE_TYPE_MINI_ROLLER_COASTER
    {   13,     24,     9,      27,     11,     40,     70,     0   },  // RIDE_TYPE_MINE_RIDE
    {   16,     24,     4,      4,      7,      40,     55,     0   },  // RIDE_TYPE_59
    {   35,     24,     5,      18,     7,      50,     95,     0   },  // RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER
};

const rct_ride_entry_vehicle CableLiftVehicle = {
    /* .rotation_frame_mask = */ 31,
    /* .num_vertical_frames = */ 0,
    /* .num_horizontal_frames = */ 0,
    /* .spacing = */ 0,
    /* .car_mass = */ 0,
    /* .tab_height = */ 0,
    /* .num_seats = */ 0,
    /* .sprite_flags = */ VEHICLE_SPRITE_FLAG_FLAT | VEHICLE_SPRITE_FLAG_GENTLE_SLOPES | VEHICLE_SPRITE_FLAG_STEEP_SLOPES,
    /* .sprite_width = */ 0,
    /* .sprite_height_negative = */ 0,
    /* .sprite_height_positive = */ 0,
    /* .animation = */ 0,
    /* .flags = */ 0,
    /* .base_num_frames = */ 1,
    /* .base_image_id = */ 29110,
    /* .restraint_image_id = */ 0,
    /* .gentle_slope_image_id = */ 29142,
    /* .steep_slope_image_id = */ 29214,
    /* .vertical_slope_image_id = */ 0,
    /* .diagonal_slope_image_id = */ 0,
    /* .banked_image_id = */ 0,
    /* .inline_twist_image_id = */ 0,
    /* .flat_to_gentle_bank_image_id = */ 0,
    /* .diagonal_to_gentle_slope_bank_image_id = */ 0,
    /* .gentle_slope_to_bank_image_id = */ 0,
    /* .gentle_slope_bank_turn_image_id = */ 0,
    /* .flat_bank_to_gentle_slope_image_id = */ 0,
    /* .corkscrew_image_id = */ 0,
    /* .no_vehicle_images = */ 0,
    /* .no_seating_rows = */ 0,
    /* .spinning_inertia = */ 0,
    /* .spinning_friction = */ 255,
    /* .friction_sound_id = */ SoundId::LiftClassic,
    /* .log_flume_reverser_vehicle_type = */ 0,
    /* .sound_range = */ 0,
    /* .double_sound_frequency = */ 0,
    /* .powered_acceleration = */ 0,
    /* .powered_max_speed = */ 0,
    /* .car_visual = */ 0,
    /* .effect_visual = */ 1,
    /* .draw_order = */ 14,
    /* .num_vertical_frames_override = */ 0,
    /* .peep_loading_positions = */ 0
};

/* rct2: 0x009A0AA0 */
const uint16_t RideFilmLength[3] = {
    5000, // MOUSE_TAILS
    6000, // STORM_CHASERS
    7000  // SPACE_RAIDERS
};

/* rct2: 0x009A0AC4 */
const uint16_t RideCrookedHouseLength[1] = {
    600
};

/** rct2: 0x0097D7CB */
const int32_t RidePhotoItems[] = {
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_SPIRAL_ROLLER_COASTER
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_STAND_UP_ROLLER_COASTER
    SHOP_ITEM_PHOTO2,   // RIDE_TYPE_SUSPENDED_SWINGING_COASTER
    SHOP_ITEM_PHOTO2,   // RIDE_TYPE_INVERTED_ROLLER_COASTER
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_JUNIOR_ROLLER_COASTER
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_MINIATURE_RAILWAY
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_MONORAIL
    SHOP_ITEM_PHOTO2,   // RIDE_TYPE_MINI_SUSPENDED_COASTER
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_BOAT_HIRE
    SHOP_ITEM_PHOTO3,   // RIDE_TYPE_WOODEN_WILD_MOUSE
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_STEEPLECHASE
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_CAR_RIDE
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_LAUNCHED_FREEFALL
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_BOBSLEIGH_COASTER
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_OBSERVATION_TOWER
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_LOOPING_ROLLER_COASTER
    SHOP_ITEM_PHOTO4,   // RIDE_TYPE_DINGHY_SLIDE
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_MINE_TRAIN_COASTER
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_CHAIRLIFT
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_CORKSCREW_ROLLER_COASTER
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_MAZE
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_SPIRAL_SLIDE
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_GO_KARTS
    SHOP_ITEM_PHOTO4,   // RIDE_TYPE_LOG_FLUME
    SHOP_ITEM_PHOTO4,   // RIDE_TYPE_RIVER_RAPIDS
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_DODGEMS
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_SWINGING_SHIP
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_SWINGING_INVERTER_SHIP
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_FOOD_STALL
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_1D
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_DRINK_STALL
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_1F
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_SHOP
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_MERRY_GO_ROUND
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_22
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_INFORMATION_KIOSK
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_TOILETS
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_FERRIS_WHEEL
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_MOTION_SIMULATOR
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_3D_CINEMA
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_TOP_SPIN
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_SPACE_RINGS
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_REVERSE_FREEFALL_COASTER
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_LIFT
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_CASH_MACHINE
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_TWIST
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_HAUNTED_HOUSE
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_FIRST_AID
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_CIRCUS
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_GHOST_TRAIN
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_TWISTER_ROLLER_COASTER
    SHOP_ITEM_PHOTO3,   // RIDE_TYPE_WOODEN_ROLLER_COASTER
    SHOP_ITEM_PHOTO3,   // RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_STEEL_WILD_MOUSE
    SHOP_ITEM_PHOTO2,   // RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER
    SHOP_ITEM_PHOTO2,   // RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER_ALT
    SHOP_ITEM_PHOTO2,   // RIDE_TYPE_FLYING_ROLLER_COASTER
    SHOP_ITEM_PHOTO2,   // RIDE_TYPE_FLYING_ROLLER_COASTER_ALT
    SHOP_ITEM_PHOTO3,   // RIDE_TYPE_VIRGINIA_REEL
    SHOP_ITEM_PHOTO4,   // RIDE_TYPE_SPLASH_BOATS
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_MINI_HELICOPTERS
    SHOP_ITEM_PHOTO2,   // RIDE_TYPE_LAY_DOWN_ROLLER_COASTER
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_SUSPENDED_MONORAIL
    SHOP_ITEM_PHOTO2,   // RIDE_TYPE_LAY_DOWN_ROLLER_COASTER_ALT
    SHOP_ITEM_PHOTO3,   // RIDE_TYPE_REVERSER_ROLLER_COASTER
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_HEARTLINE_TWISTER_COASTER
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_MINI_GOLF
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_GIGA_COASTER
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_ROTO_DROP
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_FLYING_SAUCERS
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_CROOKED_HOUSE
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_MONORAIL_CYCLES
    SHOP_ITEM_PHOTO2,   // RIDE_TYPE_COMPACT_INVERTED_COASTER
    SHOP_ITEM_PHOTO4,   // RIDE_TYPE_WATER_COASTER
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER
    SHOP_ITEM_PHOTO2,   // RIDE_TYPE_INVERTED_HAIRPIN_COASTER
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_MAGIC_CARPET
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_SUBMARINE_RIDE
    SHOP_ITEM_PHOTO4,   // RIDE_TYPE_RIVER_RAFTS
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_50
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_ENTERPRISE
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_52
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_53
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_54
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_55
    SHOP_ITEM_PHOTO2,   // RIDE_TYPE_INVERTED_IMPULSE_COASTER
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_MINI_ROLLER_COASTER
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_MINE_RIDE
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_59
    SHOP_ITEM_PHOTO,    // RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER
};

// rct2: 0x0097CD1E
const rating_tuple RideRatings[RIDE_TYPE_COUNT] = {
    {   50,     30, 10  },  // RIDE_TYPE_SPIRAL_ROLLER_COASTER
    {   50,     30, 10  },  // RIDE_TYPE_STAND_UP_ROLLER_COASTER
    {   50,     30, 10  },  // RIDE_TYPE_SUSPENDED_SWINGING_COASTER
    {   50,     30, 10  },  // RIDE_TYPE_INVERTED_ROLLER_COASTER
    {   50,     30, 10  },  // RIDE_TYPE_JUNIOR_ROLLER_COASTER
    {   70,     6,  -10 },  // RIDE_TYPE_MINIATURE_RAILWAY
    {   70,     6,  -10 },  // RIDE_TYPE_MONORAIL
    {   50,     30, 10  },  // RIDE_TYPE_MINI_SUSPENDED_COASTER
    {   70,     6,  0   },  // RIDE_TYPE_BOAT_HIRE
    {   50,     30, 30  },  // RIDE_TYPE_WOODEN_WILD_MOUSE
    {   50,     30, 10  },  // RIDE_TYPE_STEEPLECHASE
    {   70,     10, 10  },  // RIDE_TYPE_CAR_RIDE
    {   50,     50, 10  },  // RIDE_TYPE_LAUNCHED_FREEFALL
    {   50,     30, 10  },  // RIDE_TYPE_BOBSLEIGH_COASTER
    {   80,     10, 0   },  // RIDE_TYPE_OBSERVATION_TOWER
    {   50,     30, 10  },  // RIDE_TYPE_LOOPING_ROLLER_COASTER
    {   50,     30, 10  },  // RIDE_TYPE_DINGHY_SLIDE
    {   50,     30, 10  },  // RIDE_TYPE_MINE_TRAIN_COASTER
    {   70,     10, 0   },  // RIDE_TYPE_CHAIRLIFT
    {   50,     30, 10  },  // RIDE_TYPE_CORKSCREW_ROLLER_COASTER
    {   50,     0,  0   },  // RIDE_TYPE_MAZE
    {   50,     10, 0   },  // RIDE_TYPE_SPIRAL_SLIDE
    {   120,    20, 0   },  // RIDE_TYPE_GO_KARTS
    {   80,     34, 6   },  // RIDE_TYPE_LOG_FLUME
    {   72,     26, 6   },  // RIDE_TYPE_RIVER_RAPIDS
    {   40,     20, 0   },  // RIDE_TYPE_DODGEMS
    {   50,     30, 10  },  // RIDE_TYPE_SWINGING_SHIP
    {   50,     30, 10  },  // RIDE_TYPE_SWINGING_INVERTER_SHIP
    {   0,      0,  0   },  // RIDE_TYPE_FOOD_STALL
    {   0,      0,  0   },  // RIDE_TYPE_1D
    {   0,      0,  0   },  // RIDE_TYPE_DRINK_STALL
    {   0,      0,  0   },  // RIDE_TYPE_1F
    {   0,      0,  0   },  // RIDE_TYPE_SHOP
    {   50,     10, 0   },  // RIDE_TYPE_MERRY_GO_ROUND
    {   0,      0,  0   },  // RIDE_TYPE_22
    {   0,      0,  0   },  // RIDE_TYPE_INFORMATION_KIOSK
    {   0,      0,  0   },  // RIDE_TYPE_TOILETS
    {   60,     20, 10  },  // RIDE_TYPE_FERRIS_WHEEL
    {   24,     20, 10  },  // RIDE_TYPE_MOTION_SIMULATOR
    {   20,     10, 0   },  // RIDE_TYPE_3D_CINEMA
    {   24,     20, 10  },  // RIDE_TYPE_TOP_SPIN
    {   12,     4,  4   },  // RIDE_TYPE_SPACE_RINGS
    {   44,     66, 10  },  // RIDE_TYPE_REVERSE_FREEFALL_COASTER
    {   80,     10, 0   },  // RIDE_TYPE_LIFT
    {   52,     38, 10  },  // RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER
    {   0,      0,  0   },  // RIDE_TYPE_CASH_MACHINE
    {   40,     20, 10  },  // RIDE_TYPE_TWIST
    {   20,     10, 0   },  // RIDE_TYPE_HAUNTED_HOUSE
    {   0,      0,  0   },  // RIDE_TYPE_FIRST_AID
    {   20,     10, 0   },  // RIDE_TYPE_CIRCUS
    {   70,     10, 10  },  // RIDE_TYPE_GHOST_TRAIN
    {   52,     36, 10  },  // RIDE_TYPE_TWISTER_ROLLER_COASTER
    {   52,     33, 8   },  // RIDE_TYPE_WOODEN_ROLLER_COASTER
    {   48,     28, 7   },  // RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER
    {   50,     30, 30  },  // RIDE_TYPE_STEEL_WILD_MOUSE
    {   50,     30, 10  },  // RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER
    {   50,     30, 10  },  // RIDE_TYPE_38
    {   50,     30, 10  },  // RIDE_TYPE_FLYING_ROLLER_COASTER
    {   50,     30, 10  },  // RIDE_TYPE_3A
    {   30,     15, 25  },  // RIDE_TYPE_VIRGINIA_REEL
    {   80,     34, 6   },  // RIDE_TYPE_SPLASH_BOATS
    {   70,     10, 10  },  // RIDE_TYPE_MINI_HELICOPTERS
    {   50,     30, 10  },  // RIDE_TYPE_LAY_DOWN_ROLLER_COASTER
    {   70,     6,  -10 },  // RIDE_TYPE_SUSPENDED_MONORAIL
    {   50,     30, 10  },  // RIDE_TYPE_40
    {   48,     28, 7   },  // RIDE_TYPE_REVERSER_ROLLER_COASTER
    {   50,     30, 10  },  // RIDE_TYPE_HEARTLINE_TWISTER_COASTER
    {   50,     30, 10  },  // RIDE_TYPE_MINI_GOLF
    {   51,     32, 10  },  // RIDE_TYPE_GIGA_COASTER
    {   50,     50, 10  },  // RIDE_TYPE_ROTO_DROP
    {   50,     25, 0   },  // RIDE_TYPE_FLYING_SAUCERS
    {   15,     8,  0   },  // RIDE_TYPE_CROOKED_HOUSE
    {   50,     10, 10  },  // RIDE_TYPE_MONORAIL_CYCLES
    {   50,     30, 10  },  // RIDE_TYPE_COMPACT_INVERTED_COASTER
    {   50,     30, 10  },  // RIDE_TYPE_WATER_COASTER
    {   44,     66, 10  },  // RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER
    {   50,     30, 30  },  // RIDE_TYPE_INVERTED_HAIRPIN_COASTER
    {   50,     30, 10  },  // RIDE_TYPE_MAGIC_CARPET
    {   70,     6,  0   },  // RIDE_TYPE_SUBMARINE_RIDE
    {   80,     34, 6   },  // RIDE_TYPE_RIVER_RAFTS
    {   0,      0,  0   },  // RIDE_TYPE_50
    {   50,     10, 0   },  // RIDE_TYPE_ENTERPRISE
    {   0,      0,  0   },  // RIDE_TYPE_52
    {   0,      0,  0   },  // RIDE_TYPE_53
    {   0,      0,  0   },  // RIDE_TYPE_54
    {   50,     30, 10  },  // RIDE_TYPE_55
    {   50,     30, 10  },  // RIDE_TYPE_INVERTED_IMPULSE_COASTER
    {   50,     30, 10  },  // RIDE_TYPE_MINI_ROLLER_COASTER
    {   60,     20, 10  },  // RIDE_TYPE_MINE_RIDE
    {   50,     30, 30  },  // RIDE_TYPE_59
    {   50,     30, 10  },  // RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER
};

// rct2: 0x0097CF40
const rct_ride_properties RideProperties[RIDE_TYPE_COUNT] = {
        { 10, 27,  30, 17, 17, 0 }, // RIDE_TYPE_SPIRAL_ROLLER_COASTER
        { 7,  27,  0,  0,  0, 0 }, // RIDE_TYPE_STAND_UP_ROLLER_COASTER
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_SUSPENDED_SWINGING_COASTER
        { 7,  27,  0,  0,  0, 0 },  // RIDE_TYPE_INVERTED_ROLLER_COASTER
        { 0,  0,   0,  17,  16, -1 },  // RIDE_TYPE_JUNIOR_ROLLER_COASTER
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_MINIATURE_RAILWAY
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_MONORAIL
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_MINI_SUSPENDED_COASTER
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_BOAT_HIRE
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_WOODEN_WILD_MOUSE
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_STEEPLECHASE
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_CAR_RIDE
        { 10, 40,  0,  0,  0, 0 },  // RIDE_TYPE_LAUNCHED_FREEFALL
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_BOBSLEIGH_COASTER
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_OBSERVATION_TOWER
        { 10, 27,  26, 18,  18, 0 },  // RIDE_TYPE_LOOPING_ROLLER_COASTER
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_DINGHY_SLIDE
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_MINE_TRAIN_COASTER
        { 1,  4,   0,  0,  0, 0 },  // RIDE_TYPE_CHAIRLIFT
        { 10, 27,  30, 25,  25, 0 },  // RIDE_TYPE_CORKSCREW_ROLLER_COASTER
        { 1,  64,  0,  0,  0, 0 },  // RIDE_TYPE_MAZE
        { 1,  5,   0,  0,  0, 0 },  // RIDE_TYPE_SPIRAL_SLIDE
        { 1,  10,  0,  0,  0, 0 },  // RIDE_TYPE_GO_KARTS
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_LOG_FLUME
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_RIVER_RAPIDS
        { 20, 180, 0,  0,  0, 0 },  // RIDE_TYPE_DODGEMS
        { 7,  25,  0,  0,  0, 0 },  // RIDE_TYPE_SWINGING_SHIP
        { 7,  15,  0,  0,  0, 0 },  // RIDE_TYPE_SWINGING_INVERTER_SHIP
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_FOOD_STALL
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_1D
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_DRINK_STALL
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_1F
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_SHOP
        { 4,  25,  0,  0,  0, 0 },  // RIDE_TYPE_MERRY_GO_ROUND
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_22
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_INFORMATION_KIOSK
        { 4,  4,   0,  0,  0, 0 },  // RIDE_TYPE_TOILETS
        { 1,  3,   0,  0,  0, 0 },  // RIDE_TYPE_FERRIS_WHEEL
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_MOTION_SIMULATOR
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_3D_CINEMA
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_TOP_SPIN
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_SPACE_RINGS
        { 7,  30,  30, 40, 40, 0 },  // RIDE_TYPE_REVERSE_FREEFALL_COASTER
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_LIFT
        { 10, 27,  26, 17, 68, 0 },  // RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_CASH_MACHINE
        { 3,  6,   0,  0,  0, 0 },  // RIDE_TYPE_TWIST
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_HAUNTED_HOUSE
        { 8,  8,   0,  0,  0, 0 },  // RIDE_TYPE_FIRST_AID
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_CIRCUS
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_GHOST_TRAIN
        { 10, 27,  30, 17,  68, 0 },  // RIDE_TYPE_TWISTER_ROLLER_COASTER
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_WOODEN_ROLLER_COASTER
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_STEEL_WILD_MOUSE
        { 10, 27,  30, 25,  25, 0 },  // RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER
        { 10, 27,  30, 25,  25, 0 },  // RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER_ALT
        { 10, 27,  30, 25,  25, 0 },  // RIDE_TYPE_FLYING_ROLLER_COASTER
        { 10, 27,  30, 25,  25, 0 },  // RIDE_TYPE_FLYING_ROLLER_COASTER_ALT
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_VIRGINIA_REEL
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_SPLASH_BOATS
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_MINI_HELICOPTERS
        { 10, 27,  30, 25,  25, 0 },  // RIDE_TYPE_LAY_DOWN_ROLLER_COASTER
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_SUSPENDED_MONORAIL
        { 10, 27,  30, 25,  25, 0 },  // RIDE_TYPE_LAY_DOWN_ROLLER_COASTER_ALT
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_REVERSER_ROLLER_COASTER
        { 10, 27,  30, 25,  25, 0 },  // RIDE_TYPE_HEARTLINE_TWISTER_COASTER
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_MINI_GOLF
        { 10, 27,  30, 17,  68, 1 },  // RIDE_TYPE_GIGA_COASTER
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_ROTO_DROP
        { 20, 180, 0,  0,  0, 0 },  // RIDE_TYPE_FLYING_SAUCERS
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_CROOKED_HOUSE
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_MONORAIL_CYCLES
        { 7,  27,  0,  0,  0, 0 },  // RIDE_TYPE_COMPACT_INVERTED_COASTER
        { 0,  0,   0,  17,  16, -1 },  // RIDE_TYPE_WATER_COASTER
        { 30, 50,  30, 40,  40, 0 },  // RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_INVERTED_HAIRPIN_COASTER
        { 7,  15,  0,  0,  0, 0 },  // RIDE_TYPE_MAGIC_CARPET
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_SUBMARINE_RIDE
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_RIVER_RAFTS
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_50
        { 10, 20,  0,  0,  0, 0 },  // RIDE_TYPE_ENTERPRISE
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_52
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_53
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_54
        { 10, 27,  30, 25,  25, 0 },  // RIDE_TYPE_55
        { 10, 33,  30, 25,  25, 0 },  // RIDE_TYPE_INVERTED_IMPULSE_COASTER
        { 0,  0,   0,  0,  68, 1 },  // RIDE_TYPE_MINI_ROLLER_COASTER
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_MINE_RIDE
        { 0,  0,   0,  0,  0, 0 },  // RIDE_TYPE_59
        { 10, 31,  26, 18,  18, 0 },  // RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER
};

#define TRACK_COLOUR_PRESETS(...)       {static_cast<uint8_t>(std::size<TrackColour>({__VA_ARGS__})), {__VA_ARGS__}}

#define DEFAULT_FLAT_RIDE_COLOUR_PRESET TRACK_COLOUR_PRESETS( { COLOUR_BRIGHT_RED, COLOUR_LIGHT_BLUE, COLOUR_YELLOW } )
#define DEFAULT_STALL_COLOUR_PRESET     TRACK_COLOUR_PRESETS( { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED } )

const track_colour_preset_list RideColourPresets[] = {
    // RIDE_TYPE_SPIRAL_ROLLER_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_DARK_GREEN },
        { COLOUR_GREY, COLOUR_GREY, COLOUR_BORDEAUX_RED },
    ),

    // RIDE_TYPE_STAND_UP_ROLLER_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_BLACK },
        { COLOUR_BLACK, COLOUR_LIGHT_BLUE, COLOUR_BLACK },
        { COLOUR_LIGHT_ORANGE, COLOUR_WHITE, COLOUR_LIGHT_ORANGE },
        { COLOUR_BORDEAUX_RED, COLOUR_YELLOW, COLOUR_BORDEAUX_RED },
        { COLOUR_MOSS_GREEN, COLOUR_MOSS_GREEN, COLOUR_WHITE },
        { COLOUR_SATURATED_BROWN, COLOUR_LIGHT_ORANGE, COLOUR_SATURATED_BROWN },
    ),

    // RIDE_TYPE_SUSPENDED_SWINGING_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_OLIVE_GREEN, COLOUR_OLIVE_GREEN, COLOUR_LIGHT_BROWN },
        { COLOUR_BLACK, COLOUR_LIGHT_BLUE, COLOUR_BLACK },
        { COLOUR_SATURATED_BROWN, COLOUR_YELLOW, COLOUR_SATURATED_BROWN },
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_OLIVE_GREEN },
    ),

    // RIDE_TYPE_INVERTED_ROLLER_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_BLACK },
        { COLOUR_WHITE, COLOUR_YELLOW, COLOUR_WHITE },
        { COLOUR_SALMON_PINK, COLOUR_SALMON_PINK, COLOUR_GREY },
        { COLOUR_BLACK, COLOUR_BRIGHT_RED, COLOUR_BLACK },
        { COLOUR_SATURATED_BROWN, COLOUR_WHITE, COLOUR_SATURATED_BROWN },
        { COLOUR_YELLOW, COLOUR_YELLOW, COLOUR_BLACK },
    ),

    // RIDE_TYPE_JUNIOR_ROLLER_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_YELLOW, COLOUR_YELLOW, COLOUR_SATURATED_BROWN },
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_DARK_OLIVE_GREEN },
        { COLOUR_TEAL, COLOUR_YELLOW, COLOUR_TEAL },
        { COLOUR_SATURATED_BROWN, COLOUR_SATURATED_BROWN, COLOUR_OLIVE_GREEN },
        { COLOUR_BLACK, COLOUR_LIGHT_BLUE, COLOUR_BLACK },
    ),

    // RIDE_TYPE_MINIATURE_RAILWAY
    TRACK_COLOUR_PRESETS(
        { COLOUR_LIGHT_BLUE, COLOUR_WHITE, COLOUR_DARK_BROWN },
        { COLOUR_BRIGHT_RED, COLOUR_WHITE, COLOUR_BLACK },
    ),

    // RIDE_TYPE_MONORAIL
    TRACK_COLOUR_PRESETS(
        { COLOUR_GREY, COLOUR_GREY, COLOUR_DARK_OLIVE_GREEN },
        { COLOUR_GREY, COLOUR_GREY, COLOUR_SATURATED_BROWN },
        { COLOUR_GREY, COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED },
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_BLACK },
        { COLOUR_OLIVE_GREEN, COLOUR_OLIVE_GREEN, COLOUR_WHITE },
    ),

    // RIDE_TYPE_MINI_SUSPENDED_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_BLACK, COLOUR_BLACK, COLOUR_SATURATED_BROWN },
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_BLACK },
    ),

    // RIDE_TYPE_BOAT_HIRE
    TRACK_COLOUR_PRESETS(
        { COLOUR_MOSS_GREEN, COLOUR_MOSS_GREEN, COLOUR_WHITE },
        { COLOUR_BLACK, COLOUR_BLACK, COLOUR_DARK_GREEN },
        { COLOUR_LIGHT_BROWN, COLOUR_LIGHT_BROWN, COLOUR_YELLOW },
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED },
    ),

    // RIDE_TYPE_WOODEN_WILD_MOUSE
    TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_WHITE },
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_SATURATED_BROWN },
        { COLOUR_LIGHT_BLUE, COLOUR_LIGHT_BLUE, COLOUR_SATURATED_BROWN },
    ),

    // RIDE_TYPE_STEEPLECHASE
    TRACK_COLOUR_PRESETS(
        { COLOUR_LIGHT_BLUE, COLOUR_LIGHT_BLUE, COLOUR_BLACK },
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_BLACK },
        { COLOUR_BRIGHT_GREEN, COLOUR_BRIGHT_GREEN, COLOUR_BLACK },
        { COLOUR_YELLOW, COLOUR_YELLOW, COLOUR_BLACK },
        { COLOUR_LIGHT_ORANGE, COLOUR_LIGHT_ORANGE, COLOUR_BLACK },
        { COLOUR_WHITE, COLOUR_WHITE, COLOUR_BLACK },
        { COLOUR_BRIGHT_PINK, COLOUR_BRIGHT_PINK, COLOUR_BLACK },
    ),

    // RIDE_TYPE_CAR_RIDE
    TRACK_COLOUR_PRESETS(
        { COLOUR_SATURATED_BROWN, COLOUR_SATURATED_BROWN, COLOUR_GREY },
        { COLOUR_LIGHT_PURPLE, COLOUR_LIGHT_PURPLE, COLOUR_WHITE },
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_OLIVE_GREEN },
        { COLOUR_GREY, COLOUR_GREY, COLOUR_BLACK },
    ),

    // RIDE_TYPE_LAUNCHED_FREEFALL
    TRACK_COLOUR_PRESETS(
        { COLOUR_WHITE, COLOUR_BRIGHT_RED, COLOUR_LIGHT_PURPLE },
        { COLOUR_DARK_GREEN, COLOUR_YELLOW, COLOUR_WHITE },
    ),

    // RIDE_TYPE_BOBSLEIGH_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_WHITE, COLOUR_BRIGHT_RED, COLOUR_WHITE },
        { COLOUR_LIGHT_BLUE, COLOUR_WHITE, COLOUR_BLACK },
        { COLOUR_YELLOW, COLOUR_BRIGHT_RED, COLOUR_BLACK },
    ),

    // RIDE_TYPE_OBSERVATION_TOWER
    TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_BLACK, COLOUR_GREY },
        { COLOUR_LIGHT_BLUE, COLOUR_YELLOW, COLOUR_GREY },
    ),

    // RIDE_TYPE_LOOPING_ROLLER_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_LIGHT_BROWN, COLOUR_LIGHT_BROWN, COLOUR_BLACK },
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_BLACK },
        { COLOUR_WHITE, COLOUR_WHITE, COLOUR_SATURATED_RED },
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_WHITE },
        { COLOUR_LIGHT_PURPLE, COLOUR_LIGHT_PURPLE, COLOUR_DARK_GREEN },
        { COLOUR_OLIVE_GREEN, COLOUR_OLIVE_GREEN, COLOUR_LIGHT_BROWN },
        { COLOUR_GREY, COLOUR_LIGHT_ORANGE, COLOUR_GREY },
    ),

    // RIDE_TYPE_DINGHY_SLIDE
    TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_GREEN, COLOUR_SATURATED_BROWN, COLOUR_DARK_GREEN },
        { COLOUR_TEAL, COLOUR_BORDEAUX_RED, COLOUR_TEAL },
        { COLOUR_BRIGHT_RED, COLOUR_GREY, COLOUR_SATURATED_RED },
    ),

    // RIDE_TYPE_MINE_TRAIN_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_BROWN, COLOUR_GREY, COLOUR_DARK_BROWN },
        { COLOUR_DARK_BROWN, COLOUR_GREY, COLOUR_BLACK },
    ),

    // RIDE_TYPE_CHAIRLIFT
    TRACK_COLOUR_PRESETS(
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_GREY },
        { COLOUR_WHITE, COLOUR_WHITE, COLOUR_OLIVE_GREEN },
        { COLOUR_LIGHT_BROWN, COLOUR_LIGHT_BROWN, COLOUR_WHITE },
    ),

    // RIDE_TYPE_CORKSCREW_ROLLER_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_BLACK },
        { COLOUR_LIGHT_BLUE, COLOUR_LIGHT_BLUE, COLOUR_WHITE },
        { COLOUR_BLACK, COLOUR_BORDEAUX_RED, COLOUR_BLACK },
        { COLOUR_WHITE, COLOUR_WHITE, COLOUR_DARK_GREEN },
    ),

    // RIDE_TYPE_MAZE
    TRACK_COLOUR_PRESETS(
        { COLOUR_GREY, COLOUR_GREY, COLOUR_GREY },
    ),

    // RIDE_TYPE_SPIRAL_SLIDE
    TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_YELLOW, COLOUR_DARK_BROWN },
        { COLOUR_LIGHT_BLUE, COLOUR_BRIGHT_PINK, COLOUR_LIGHT_PURPLE },
    ),

    // RIDE_TYPE_GO_KARTS
    TRACK_COLOUR_PRESETS(
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_DARK_BROWN },
        { COLOUR_YELLOW, COLOUR_YELLOW, COLOUR_BORDEAUX_RED },
        { COLOUR_DARK_GREEN, COLOUR_DARK_GREEN, COLOUR_DARK_GREEN },
        { COLOUR_DARK_BROWN, COLOUR_DARK_BROWN, COLOUR_BLACK },
    ),

    // RIDE_TYPE_LOG_FLUME
    TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_BROWN, COLOUR_DARK_BROWN, COLOUR_GREY },
        { COLOUR_OLIVE_GREEN, COLOUR_BLACK, COLOUR_GREY },
    ),

    // RIDE_TYPE_RIVER_RAPIDS
    TRACK_COLOUR_PRESETS(
        { COLOUR_WHITE, COLOUR_BLACK, COLOUR_DARK_BROWN },
    ),

    // RIDE_TYPE_DODGEMS
    TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_SATURATED_BROWN, COLOUR_SATURATED_BROWN },
        { COLOUR_LIGHT_BLUE, COLOUR_GREY, COLOUR_GREY },
        { COLOUR_DARK_GREEN, COLOUR_BLACK, COLOUR_DARK_GREEN },
    ),

    // RIDE_TYPE_SWINGING_SHIP
    TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_BROWN, COLOUR_DARK_BROWN, COLOUR_BLACK },
    ),

    // RIDE_TYPE_SWINGING_INVERTER_SHIP
    TRACK_COLOUR_PRESETS(
        { COLOUR_BORDEAUX_RED, COLOUR_YELLOW, COLOUR_BLACK },
        { COLOUR_BLACK, COLOUR_SATURATED_BROWN, COLOUR_BLACK },
    ),

    // RIDE_TYPE_FOOD_STALL
    DEFAULT_STALL_COLOUR_PRESET,

    // RIDE_TYPE_1D
    DEFAULT_STALL_COLOUR_PRESET,

    // RIDE_TYPE_DRINK_STALL
    DEFAULT_STALL_COLOUR_PRESET,

    // RIDE_TYPE_1F
    DEFAULT_STALL_COLOUR_PRESET,

    // RIDE_TYPE_SHOP
    DEFAULT_STALL_COLOUR_PRESET,

    // RIDE_TYPE_MERRY_GO_ROUND
    DEFAULT_FLAT_RIDE_COLOUR_PRESET,

    // RIDE_TYPE_22
    DEFAULT_STALL_COLOUR_PRESET,

    // RIDE_TYPE_INFORMATION_KIOSK
    DEFAULT_STALL_COLOUR_PRESET,

    // RIDE_TYPE_TOILETS
    DEFAULT_FLAT_RIDE_COLOUR_PRESET,

    // RIDE_TYPE_FERRIS_WHEEL
    TRACK_COLOUR_PRESETS(
        { COLOUR_BLACK, COLOUR_BLACK, COLOUR_BLACK },
        { COLOUR_GREY, COLOUR_GREY, COLOUR_GREY },
    ),

    // RIDE_TYPE_MOTION_SIMULATOR
    DEFAULT_FLAT_RIDE_COLOUR_PRESET,

    // RIDE_TYPE_3D_CINEMA
    DEFAULT_FLAT_RIDE_COLOUR_PRESET,

    // RIDE_TYPE_TOP_SPIN
    TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_YELLOW, COLOUR_YELLOW },
        { COLOUR_TEAL, COLOUR_WHITE, COLOUR_WHITE },
        { COLOUR_DARK_GREEN, COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED },
    ),

    // RIDE_TYPE_SPACE_RINGS
    DEFAULT_FLAT_RIDE_COLOUR_PRESET,

    // RIDE_TYPE_REVERSE_FREEFALL_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_GREEN, COLOUR_YELLOW, COLOUR_DARK_GREEN },
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_YELLOW },
        { COLOUR_GREY, COLOUR_SALMON_PINK, COLOUR_GREY },
    ),

    // RIDE_TYPE_LIFT
    TRACK_COLOUR_PRESETS(
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_GREY },
        { COLOUR_DARK_GREEN, COLOUR_DARK_GREEN, COLOUR_GREY },
        { COLOUR_BLACK, COLOUR_BLACK, COLOUR_GREY },
    ),

    // RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_BLACK, COLOUR_GREY, COLOUR_BLACK },
        { COLOUR_DARK_GREEN, COLOUR_DARK_GREEN, COLOUR_GREY },
    ),

    // RIDE_TYPE_CASH_MACHINE
    DEFAULT_FLAT_RIDE_COLOUR_PRESET,

    // RIDE_TYPE_TWIST
    DEFAULT_FLAT_RIDE_COLOUR_PRESET,

    // RIDE_TYPE_HAUNTED_HOUSE
    DEFAULT_FLAT_RIDE_COLOUR_PRESET,

    // RIDE_TYPE_FIRST_AID
    DEFAULT_FLAT_RIDE_COLOUR_PRESET,

    // RIDE_TYPE_CIRCUS
    DEFAULT_FLAT_RIDE_COLOUR_PRESET,

    // RIDE_TYPE_GHOST_TRAIN
    TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_GREEN, COLOUR_BORDEAUX_RED, COLOUR_BLACK },
        { COLOUR_TEAL, COLOUR_WHITE, COLOUR_DARK_BROWN },
        { COLOUR_DARK_BROWN, COLOUR_BORDEAUX_RED, COLOUR_BLACK },
        { COLOUR_BORDEAUX_RED, COLOUR_YELLOW, COLOUR_BLACK },
    ),

    // RIDE_TYPE_TWISTER_ROLLER_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_YELLOW, COLOUR_YELLOW, COLOUR_BORDEAUX_RED },
        { COLOUR_AQUAMARINE, COLOUR_AQUAMARINE, COLOUR_DARK_PURPLE },
        { COLOUR_WHITE, COLOUR_WHITE, COLOUR_LIGHT_BLUE },
        { COLOUR_DARK_GREEN, COLOUR_MOSS_GREEN, COLOUR_DARK_BROWN },
        { COLOUR_BORDEAUX_RED, COLOUR_LIGHT_ORANGE, COLOUR_WHITE },
    ),

    // RIDE_TYPE_WOODEN_ROLLER_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_BORDEAUX_RED, COLOUR_BLACK, COLOUR_WHITE },
        { COLOUR_BRIGHT_RED, COLOUR_BLACK, COLOUR_GREY },
        { COLOUR_YELLOW, COLOUR_DARK_BROWN, COLOUR_DARK_BROWN },
        { COLOUR_TEAL, COLOUR_BORDEAUX_RED, COLOUR_WHITE },
        { COLOUR_LIGHT_BLUE, COLOUR_BLACK, COLOUR_BLACK },
    ),

    // RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_WHITE, COLOUR_DARK_BROWN, COLOUR_WHITE },
        { COLOUR_SATURATED_BROWN, COLOUR_WHITE, COLOUR_SATURATED_BROWN },
        { COLOUR_DARK_BROWN, COLOUR_LIGHT_BROWN, COLOUR_DARK_BROWN },
    ),

    // RIDE_TYPE_STEEL_WILD_MOUSE
    TRACK_COLOUR_PRESETS(
        { COLOUR_LIGHT_BLUE, COLOUR_LIGHT_BLUE, COLOUR_YELLOW },
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_WHITE },
        { COLOUR_DARK_PURPLE, COLOUR_BRIGHT_PINK, COLOUR_DARK_PURPLE },
        { COLOUR_LIGHT_BROWN, COLOUR_LIGHT_BROWN, COLOUR_BORDEAUX_RED },
    ),

    // RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_PINK, COLOUR_YELLOW, COLOUR_YELLOW },
        { COLOUR_LIGHT_PURPLE, COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED },
        { COLOUR_BORDEAUX_RED, COLOUR_WHITE, COLOUR_WHITE },
    ),

    // RIDE_TYPE_38
    TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_PINK, COLOUR_YELLOW, COLOUR_YELLOW },
        { COLOUR_LIGHT_PURPLE, COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED },
        { COLOUR_BORDEAUX_RED, COLOUR_WHITE, COLOUR_WHITE },
    ),

    // RIDE_TYPE_FLYING_ROLLER_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_OLIVE_GREEN, COLOUR_OLIVE_GREEN, COLOUR_TEAL },
        { COLOUR_TEAL, COLOUR_TEAL, COLOUR_BORDEAUX_RED },
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_DARK_PURPLE },
    ),

    // RIDE_TYPE_3A
    TRACK_COLOUR_PRESETS(
        { COLOUR_OLIVE_GREEN, COLOUR_OLIVE_GREEN, COLOUR_TEAL },
        { COLOUR_TEAL, COLOUR_TEAL, COLOUR_BORDEAUX_RED },
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_DARK_PURPLE },
    ),

    // RIDE_TYPE_VIRGINIA_REEL
    TRACK_COLOUR_PRESETS(
        { COLOUR_SATURATED_BROWN, COLOUR_GREY, COLOUR_SATURATED_BROWN },
        { COLOUR_BLACK, COLOUR_DARK_BROWN, COLOUR_DARK_BROWN },
        { COLOUR_DARK_BROWN, COLOUR_BORDEAUX_RED, COLOUR_DARK_BROWN },
    ),

    // RIDE_TYPE_SPLASH_BOATS
    TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_BROWN, COLOUR_DARK_BROWN, COLOUR_BLACK },
        { COLOUR_DARK_GREEN, COLOUR_DARK_GREEN, COLOUR_GREY },
    ),

    // RIDE_TYPE_MINI_HELICOPTERS
    TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_PURPLE, COLOUR_YELLOW, COLOUR_DARK_PURPLE },
        { COLOUR_WHITE, COLOUR_WHITE, COLOUR_BRIGHT_RED },
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_YELLOW },
        { COLOUR_BRIGHT_PINK, COLOUR_BRIGHT_PINK, COLOUR_WHITE },
        { COLOUR_LIGHT_BLUE, COLOUR_LIGHT_BLUE, COLOUR_WHITE },
    ),

    // RIDE_TYPE_LAY_DOWN_ROLLER_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_YELLOW },
        { COLOUR_DARK_PURPLE, COLOUR_DARK_PURPLE, COLOUR_YELLOW },
        { COLOUR_YELLOW, COLOUR_YELLOW, COLOUR_DARK_GREEN },
    ),

    // RIDE_TYPE_SUSPENDED_MONORAIL
    TRACK_COLOUR_PRESETS(
        { COLOUR_BORDEAUX_RED, COLOUR_BLACK, COLOUR_BLACK },
        { COLOUR_DARK_PURPLE, COLOUR_DARK_PURPLE, COLOUR_BLACK },
        { COLOUR_DARK_GREEN, COLOUR_DARK_GREEN, COLOUR_BLACK },
    ),

    // RIDE_TYPE_40
    TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_YELLOW },
        { COLOUR_DARK_PURPLE, COLOUR_DARK_PURPLE, COLOUR_YELLOW },
        { COLOUR_YELLOW, COLOUR_YELLOW, COLOUR_DARK_GREEN },
    ),

    // RIDE_TYPE_REVERSER_ROLLER_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_BROWN, COLOUR_LIGHT_BROWN, COLOUR_DARK_BROWN },
        { COLOUR_SATURATED_BROWN, COLOUR_BORDEAUX_RED, COLOUR_SATURATED_BROWN },
    ),

    // RIDE_TYPE_HEARTLINE_TWISTER_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_WHITE, COLOUR_LIGHT_BLUE, COLOUR_WHITE },
        { COLOUR_BORDEAUX_RED, COLOUR_YELLOW, COLOUR_BLACK },
        { COLOUR_DARK_PURPLE, COLOUR_WHITE, COLOUR_DARK_GREEN },
    ),

    // RIDE_TYPE_MINI_GOLF
    TRACK_COLOUR_PRESETS(
        { COLOUR_SATURATED_GREEN, COLOUR_DARK_BROWN, COLOUR_BLACK },
        { COLOUR_MOSS_GREEN, COLOUR_BORDEAUX_RED, COLOUR_DARK_BROWN },
    ),

    // RIDE_TYPE_GIGA_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_WHITE },
        { COLOUR_WHITE, COLOUR_WHITE, COLOUR_YELLOW },
        { COLOUR_YELLOW, COLOUR_YELLOW, COLOUR_BORDEAUX_RED },
        { COLOUR_LIGHT_BLUE, COLOUR_LIGHT_BLUE, COLOUR_BLACK },
    ),

    // RIDE_TYPE_ROTO_DROP
    TRACK_COLOUR_PRESETS(
        { COLOUR_BORDEAUX_RED, COLOUR_YELLOW, COLOUR_GREY },
        { COLOUR_BLACK, COLOUR_LIGHT_BLUE, COLOUR_GREY },
    ),

    // RIDE_TYPE_FLYING_SAUCERS
    TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_PURPLE, COLOUR_GREY, COLOUR_BLACK },
        { COLOUR_BLACK, COLOUR_YELLOW, COLOUR_BLACK },
    ),

    // RIDE_TYPE_CROOKED_HOUSE
    TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_BLACK, COLOUR_GREY },
    ),

    // RIDE_TYPE_MONORAIL_CYCLES
    TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_BROWN, COLOUR_GREY, COLOUR_DARK_BROWN },
        { COLOUR_GREY, COLOUR_BLACK, COLOUR_GREY },
    ),

    // RIDE_TYPE_COMPACT_INVERTED_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_YELLOW, COLOUR_BRIGHT_YELLOW, COLOUR_BRIGHT_RED },
        { COLOUR_ICY_BLUE, COLOUR_ICY_BLUE, COLOUR_DARK_GREEN },
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_BRIGHT_YELLOW },
        { COLOUR_WHITE, COLOUR_WHITE, COLOUR_DARK_GREEN },
    ),

    // RIDE_TYPE_WATER_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_GREEN, COLOUR_DARK_GREEN, COLOUR_BLACK },
        { COLOUR_DARK_BROWN, COLOUR_DARK_BROWN, COLOUR_BORDEAUX_RED },
        { COLOUR_WHITE, COLOUR_WHITE, COLOUR_DARK_PURPLE },
    ),

    // RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_WHITE, COLOUR_BRIGHT_RED, COLOUR_DARK_BROWN },
        { COLOUR_BRIGHT_RED, COLOUR_LIGHT_BLUE, COLOUR_WHITE },
        { COLOUR_LIGHT_ORANGE, COLOUR_BRIGHT_RED, COLOUR_BRIGHT_YELLOW },
    ),

    // RIDE_TYPE_INVERTED_HAIRPIN_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_YELLOW, COLOUR_BRIGHT_YELLOW, COLOUR_BRIGHT_RED },
        { COLOUR_BLACK, COLOUR_ICY_BLUE, COLOUR_BLACK },
        { COLOUR_WHITE, COLOUR_WHITE, COLOUR_YELLOW },
    ),

    // RIDE_TYPE_MAGIC_CARPET
    TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_PURPLE, COLOUR_WHITE, COLOUR_BLACK },
        { COLOUR_YELLOW, COLOUR_WHITE, COLOUR_BLACK },
        { COLOUR_BRIGHT_RED, COLOUR_YELLOW, COLOUR_BLACK },
    ),

    // RIDE_TYPE_SUBMARINE_RIDE
    TRACK_COLOUR_PRESETS(
        { COLOUR_GREY, COLOUR_GREY, COLOUR_BLACK },
    ),

    // RIDE_TYPE_RIVER_RAFTS
    TRACK_COLOUR_PRESETS(
        { COLOUR_BLACK, COLOUR_BLACK, COLOUR_SATURATED_BROWN },
    ),

    // RIDE_TYPE_50
    DEFAULT_FLAT_RIDE_COLOUR_PRESET,

    // RIDE_TYPE_ENTERPRISE
    DEFAULT_FLAT_RIDE_COLOUR_PRESET,

    // RIDE_TYPE_52
    DEFAULT_FLAT_RIDE_COLOUR_PRESET,

    // RIDE_TYPE_53
    DEFAULT_FLAT_RIDE_COLOUR_PRESET,

    // RIDE_TYPE_54
    DEFAULT_FLAT_RIDE_COLOUR_PRESET,

    // RIDE_TYPE_55
    TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_GREY },
        { COLOUR_YELLOW, COLOUR_YELLOW, COLOUR_SATURATED_BROWN },
    ),

    // RIDE_TYPE_INVERTED_IMPULSE_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_GREY },
        { COLOUR_YELLOW, COLOUR_YELLOW, COLOUR_WHITE },
        { COLOUR_BRIGHT_PURPLE, COLOUR_BRIGHT_PURPLE, COLOUR_BORDEAUX_RED },
    ),

    // RIDE_TYPE_MINI_ROLLER_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_GREY, COLOUR_GREY, COLOUR_DARK_BROWN },
        { COLOUR_MOSS_GREEN, COLOUR_MOSS_GREEN, COLOUR_DARK_BROWN },
        { COLOUR_SALMON_PINK, COLOUR_SALMON_PINK, COLOUR_BORDEAUX_RED },
    ),

    // RIDE_TYPE_MINE_RIDE
    TRACK_COLOUR_PRESETS(
        { COLOUR_DARK_BROWN, COLOUR_DARK_BROWN, COLOUR_BLACK },
        { COLOUR_BORDEAUX_RED, COLOUR_BORDEAUX_RED, COLOUR_DARK_BROWN },
    ),

    // RIDE_TYPE_59
    TRACK_COLOUR_PRESETS(
        { COLOUR_LIGHT_BLUE, COLOUR_LIGHT_BLUE, COLOUR_YELLOW },
    ),

    // RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER
    TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_GREEN, COLOUR_BRIGHT_GREEN, COLOUR_WHITE },
        { COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED, COLOUR_DARK_BROWN },
        { COLOUR_YELLOW, COLOUR_YELLOW, COLOUR_SATURATED_GREEN },
    ),
};

const rct_string_id RideModeNames[] = {
        STR_RIDE_MODE_NORMAL,
        STR_RIDE_MODE_CONTINUOUS_CIRCUIT,
        STR_RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE,
        STR_RIDE_MODE_POWERED_LAUNCH_PASSTROUGH,
        STR_RIDE_MODE_SHUTTLE,
        STR_RIDE_MODE_BOAT_HIRE,
        STR_RIDE_MODE_UPWARD_LAUNCH,
        STR_RIDE_MODE_ROTATING_LIFT,
        STR_RIDE_MODE_STATION_TO_STATION,
        STR_RIDE_MODE_SINGLE_RIDE_PER_ADMISSION,
        STR_RIDE_MODE_UNLIMITED_RIDES_PER_ADMISSION,
        STR_RIDE_MODE_MAZE,
        STR_RIDE_MODE_RACE,
        STR_RIDE_MODE_DODGEMS,
        STR_RIDE_MODE_SWING,
        STR_RIDE_MODE_SHOP_STALL,
        STR_RIDE_MODE_ROTATION,
        STR_RIDE_MODE_FORWARD_ROTATION,
        STR_RIDE_MODE_BACKWARD_ROTATION,
        STR_RIDE_MODE_FILM_AVENGING_AVIATORS,
        STR_RIDE_MODE_3D_FILM_MOUSE_TAILS,
        STR_RIDE_MODE_SPACE_RINGS,
        STR_RIDE_MODE_BEGINNERS,
        STR_RIDE_MODE_LIM_POWERED_LAUNCH,
        STR_RIDE_MODE_FILM_THRILL_RIDERS,
        STR_RIDE_MODE_3D_FILM_STORM_CHASERS,
        STR_RIDE_MODE_3D_FILM_SPACE_RAIDERS,
        STR_RIDE_MODE_INTENSE,
        STR_RIDE_MODE_BERSERK,
        STR_RIDE_MODE_HAUNTED_HOUSE,
        STR_RIDE_MODE_CIRCUS_SHOW,
        STR_RIDE_MODE_DOWNWARD_LAUNCH,
        STR_RIDE_MODE_CROOKED_HOUSE,
        STR_RIDE_MODE_FREEFALL_DROP,
        STR_RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED,
        STR_RIDE_MODE_POWERED_LAUNCH,
        STR_RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED_MODE,
};

static constexpr const RideTypeDescriptor DummyRTD =
{
    SET_FIELD(Category, RIDE_CATEGORY_NONE),
    SET_FIELD(EnabledTrackPieces, 0),
    SET_FIELD(ExtraTrackPieces, 0),
    SET_FIELD(StartTrackPiece, TRACK_ELEM_END_STATION),
    SET_FIELD(TrackPaintFunction, nullptr),
    SET_FIELD(Flags, 0),
    SET_FIELD(NameConvention, { RIDE_COMPONENT_TYPE_TRAIN, RIDE_COMPONENT_TYPE_TRACK, RIDE_COMPONENT_TYPE_STATION }),
    SET_FIELD(AvailableBreakdowns, 0),
    SET_FIELD(LiftData, { SoundId::Null, 5, 5 }),
};
// clang-format on

constexpr const RideTypeDescriptor RideTypeDescriptors[RIDE_TYPE_COUNT] = {
    /* RIDE_TYPE_SPIRAL_ROLLER_COASTER              */ SpiralRollerCoasterRTD,
    /* RIDE_TYPE_STAND_UP_ROLLER_COASTER            */ StandUpRollerCoasterRTD,
    /* RIDE_TYPE_SUSPENDED_SWINGING_COASTER         */ SuspendedSwingingCoasterRTD,
    /* RIDE_TYPE_INVERTED_ROLLER_COASTER            */ InvertedRollerCoasterRTD,
    /* RIDE_TYPE_JUNIOR_ROLLER_COASTER              */ JuniorRollerCoasterRTD,
    /* RIDE_TYPE_MINIATURE_RAILWAY                  */ MiniatureRailwayRTD,
    /* RIDE_TYPE_MONORAIL                           */ MonorailRTD,
    /* RIDE_TYPE_MINI_SUSPENDED_COASTER             */ MiniSuspendedCoasterRTD,
    /* RIDE_TYPE_BOAT_HIRE                          */ BoatHireRTD,
    /* RIDE_TYPE_WOODEN_WILD_MOUSE                  */ WoodenWildMouseRTD,
    /* RIDE_TYPE_STEEPLECHASE                       */ SteeplechaseRTD,
    /* RIDE_TYPE_CAR_RIDE                           */ CarRideRTD,
    /* RIDE_TYPE_LAUNCHED_FREEFALL                  */ LaunchedFreefallRTD,
    /* RIDE_TYPE_BOBSLEIGH_COASTER                  */ BobsleighCoasterRTD,
    /* RIDE_TYPE_OBSERVATION_TOWER                  */ ObservationTowerRTD,
    /* RIDE_TYPE_LOOPING_ROLLER_COASTER             */ LoopingRollerCoasterRTD,
    /* RIDE_TYPE_DINGHY_SLIDE                       */ DinghySlideRTD,
    /* RIDE_TYPE_MINE_TRAIN_COASTER                 */ MineTrainCoasterRTD,
    /* RIDE_TYPE_CHAIRLIFT                          */ ChairliftRTD,
    /* RIDE_TYPE_CORKSCREW_ROLLER_COASTER           */ CorkscrewRollerCoasterRTD,
    /* RIDE_TYPE_MAZE                               */ MazeRTD,
    /* RIDE_TYPE_SPIRAL_SLIDE                       */ SpiralSlideRTD,
    /* RIDE_TYPE_GO_KARTS                           */ GoKartsRTD,
    /* RIDE_TYPE_LOG_FLUME                          */ LogFlumeRTD,
    /* RIDE_TYPE_RIVER_RAPIDS                       */ RiverRapidsRTD,
    /* RIDE_TYPE_DODGEMS                            */ DodgemsRTD,
    /* RIDE_TYPE_SWINGING_SHIP                      */ SwingingShipRTD,
    /* RIDE_TYPE_SWINGING_INVERTER_SHIP             */ SwingingInverterShipRTD,
    /* RIDE_TYPE_FOOD_STALL                         */ FoodStallRTD,
    /* RIDE_TYPE_1D                                 */ DummyRTD,
    /* RIDE_TYPE_DRINK_STALL                        */ DrinkStallRTD,
    /* RIDE_TYPE_1F                                 */ DummyRTD,
    /* RIDE_TYPE_SHOP                               */ ShopRTD,
    /* RIDE_TYPE_MERRY_GO_ROUND                     */ MerryGoRoundRTD,
    /* RIDE_TYPE_22                                 */ DummyRTD,
    /* RIDE_TYPE_INFORMATION_KIOSK                  */ InformationKioskRTD,
    /* RIDE_TYPE_TOILETS                            */ ToiletsRTD,
    /* RIDE_TYPE_FERRIS_WHEEL                       */ FerrisWheelRTD,
    /* RIDE_TYPE_MOTION_SIMULATOR                   */ MotionSimulatorRTD,
    /* RIDE_TYPE_3D_CINEMA                          */ CinemaRTD,
    /* RIDE_TYPE_TOP_SPIN                           */ TopSpinRTD,
    /* RIDE_TYPE_SPACE_RINGS                        */ SpaceRingsRTD,
    /* RIDE_TYPE_REVERSE_FREEFALL_COASTER           */ ReverseFreefallCoasterRTD,
    /* RIDE_TYPE_LIFT                               */ LiftRTD,
    /* RIDE_TYPE_VERTICAL_DROP_ROLLER_COASTER       */ VerticalDropCoasterRTD,
    /* RIDE_TYPE_CASH_MACHINE                       */ CashMachineRTD,
    /* RIDE_TYPE_TWIST                              */ TwistRTD,
    /* RIDE_TYPE_HAUNTED_HOUSE                      */ HauntedHouseRTD,
    /* RIDE_TYPE_FIRST_AID                          */ FirstAidRTD,
    /* RIDE_TYPE_CIRCUS                             */ CircusRTD,
    /* RIDE_TYPE_GHOST_TRAIN                        */ GhostTrainRTD,
    /* RIDE_TYPE_TWISTER_ROLLER_COASTER             */ TwisterRollerCoasterRTD,
    /* RIDE_TYPE_WOODEN_ROLLER_COASTER              */ WoodenRollerCoasterRTD,
    /* RIDE_TYPE_SIDE_FRICTION_ROLLER_COASTER       */ SideFrictionRollerCoasterRTD,
    /* RIDE_TYPE_STEEL_WILD_MOUSE                   */ SteelWildMouseRTD,
    /* RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER     */ MultiDimensionRollerCoasterRTD,
    /* RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER_ALT */ MultiDimensionRollerCoasterAltRTD,
    /* RIDE_TYPE_FLYING_ROLLER_COASTER              */ FlyingRollerCoasterRTD,
    /* RIDE_TYPE_FLYING_ROLLER_COASTER_ALT          */ FlyingRollerCoasterAltRTD,
    /* RIDE_TYPE_VIRGINIA_REEL                      */ VirginiaReelRTD,
    /* RIDE_TYPE_SPLASH_BOATS                       */ SplashBoatsRTD,
    /* RIDE_TYPE_MINI_HELICOPTERS                   */ MiniHelicoptersRTD,
    /* RIDE_TYPE_LAY_DOWN_ROLLER_COASTER            */ LayDownRollerCoasterRTD,
    /* RIDE_TYPE_SUSPENDED_MONORAIL                 */ SuspendedMonorailRTD,
    /* RIDE_TYPE_LAY_DOWN_ROLLER_COASTER_ALT        */ LayDownRollerCoasterAltRTD,
    /* RIDE_TYPE_REVERSER_ROLLER_COASTER            */ ReverserRollerCoasterRTD,
    /* RIDE_TYPE_HEARTLINE_TWISTER_COASTER          */ HeartlineTwisterCoasterRTD,
    /* RIDE_TYPE_MINI_GOLF                          */ MiniGolfRTD,
    /* RIDE_TYPE_GIGA_COASTER                       */ GigaCoasterRTD,
    /* RIDE_TYPE_ROTO_DROP                          */ RotoDropRTD,
    /* RIDE_TYPE_FLYING_SAUCERS                     */ FlyingSaucersRTD,
    /* RIDE_TYPE_CROOKED_HOUSE                      */ CrookedHouseRTD,
    /* RIDE_TYPE_MONORAIL_CYCLES                    */ MonorailCyclesRTD,
    /* RIDE_TYPE_COMPACT_INVERTED_COASTER           */ CompactInvertedCoasterRTD,
    /* RIDE_TYPE_WATER_COASTER                      */ WaterCoasterRTD,
    /* RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER       */ AirPoweredVerticalCoasterRTD,
    /* RIDE_TYPE_INVERTED_HAIRPIN_COASTER           */ InvertedHairpinCoasterRTD,
    /* RIDE_TYPE_MAGIC_CARPET                       */ MagicCarpetRTD,
    /* RIDE_TYPE_SUBMARINE_RIDE                     */ SubmarineRideRTD,
    /* RIDE_TYPE_RIVER_RAFTS                        */ RiverRaftsRTD,
    /* RIDE_TYPE_50                                 */ DummyRTD,
    /* RIDE_TYPE_ENTERPRISE                         */ EnterpriseRTD,
    /* RIDE_TYPE_52                                 */ DummyRTD,
    /* RIDE_TYPE_53                                 */ DummyRTD,
    /* RIDE_TYPE_54                                 */ DummyRTD,
    /* RIDE_TYPE_55                                 */ DummyRTD,
    /* RIDE_TYPE_INVERTED_IMPULSE_COASTER           */ InvertedImpulseCoasterRTD,
    /* RIDE_TYPE_MINI_ROLLER_COASTER                */ MiniRollerCoasterRTD,
    /* RIDE_TYPE_MINE_RIDE                          */ MineRideRTD,
    /* RIDE_TYPE_59                                 */ DummyRTD,
    /* RIDE_TYPE_LIM_LAUNCHED_ROLLER_COASTER        */ LIMLaunchedRollerCoasterRTD,
};

bool RideTypeDescriptor::HasFlag(uint64_t flag) const
{
    return Flags & flag;
}

uint64_t RideTypeDescriptor::GetAvailableTrackPieces() const
{
    if (gCheatsEnableAllDrawableTrackPieces)
    {
        return EnabledTrackPieces | ExtraTrackPieces;
    }
    return EnabledTrackPieces;
}

bool RideTypeDescriptor::SupportsTrackPiece(const uint64_t trackPiece) const
{
    return GetAvailableTrackPieces() & (1ULL << trackPiece);
}
