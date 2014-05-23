/*
 * Whether a particular ride has a running track or not. Will probably end up
 * being used in various places in the game.
 *
 * Data source is 0x0097E3AC
 *
 * Generating function is here 
 * https://gist.github.com/kevinburke/eaeb1d8149a6eef0dcc1
 */
const bool hasRunningTrack[0x59] = {
	true,	// 0 Spiral Roller coaster
	true,	// 1 Stand Up Coaster
	true,	// 2 Suspended Swinging
	true,	// 3 Inverted
	true,	// 4 Steel Mini Coaster
	true,	// 5 Mini Railroad
	true,	// 6 Monorail
	true,	// 7 Mini Suspended Coaster
	false,	// 8 Bumper Boats
	true,	// 9 Wooden Wild Mine/Mouse
	true,	// a Steeplechase/Motorbike/Soap Box Derby
	true,	// b Car Ride
	true,	// c Launched Freefall
	true,	// d Bobsleigh Coaster
	true,	// e Observation Tower
	true,	// f Looping Roller Coaster
	true,	// 10 Dinghy Slide
	true,	// 11 Mine Train Coaster
	true,	// 12 Chairlift
	true,	// 13 Corkscrew Roller Coaster
	false,	// 14 Maze
	false,	// 15 Spiral Slide
	true,	// 16 Go Karts
	true,	// 17 Log Flume
	true,	// 18 River Rapids
	false,	// 19 Bumper Cars
	false,	// 1a Pirate Ship
	false,	// 1b Swinging Inverter Ship
	false,	// 1c Food Stall
	false,	// 1d (none)
	false,	// 1e Drink Stall
	false,	// 1f (none)
	false,	// 20 Shop (all types)
	false,	// 21 Merry Go Round
	false,	// 22 Balloon Stall (maybe)
	false,	// 23 Information Kiosk
	false,	// 24 Bathroom
	false,	// 25 Ferris Wheel
	false,	// 26 Motion Simulator
	false,	// 27 3D Cinema
	false,	// 28 Gravitron
	false,	// 29 Space Rings
	true,	// 2a Reverse Freefall Coaster
	true,	// 2b Elevator
	true,	// 2c Vertical Drop Roller Coaster
	false,	// 2d ATM
	false,	// 2e Twist
	false,	// 2f Haunted House
	false,	// 30 First Aid
	false,	// 31 Circus Show
	true,	// 32 Ghost Train
	true,	// 33 Twister Roller Coaster
	true,	// 34 Wooden Roller Coaster
	true,	// 35 Side-Friction Roller Coaster
	true,	// 36 Wild Mouse
	true,	// 37 Multi Dimension Coaster
	true,	// 38 (none)
	true,	// 39 Flying Roller Coaster
	true,	// 3a (none)
	true,	// 3b Virginia Reel
	true,	// 3c Splash Boats
	true,	// 3d Mini Helicopters
	true,	// 3e Lay-down Roller Coaster
	true,	// 3f Suspended Monorail
	true,	// 40 (none)
	true,	// 41 Reverser Roller Coaster
	true,	// 42 Heartline Twister Roller Coaster
	true,	// 43 Mini Golf
	true,	// 44 Giga Coaster
	true,	// 45 Roto-Drop
	false,	// 46 Flying Saucers
	false,	// 47 Crooked House
	true,	// 48 Monorail Cycles
	true,	// 49 Compact Inverted Coaster
	true,	// 4a Water Coaster
	true,	// 4b Air Powered Vertical Coaster
	true,	// 4c Inverted Hairpin Coaster
	false,	// 4d Magic Carpet
	false,	// 4e Submarine Ride
	true,	// 4f River Rafts
	false,	// 50 (none)
	false,	// 51 Enterprise
	false,	// 52 (none)
	false,	// 53 (none)
	false,	// 54 (none)
	true,	// 55 (none)
	true,	// 56 Inverted Impulse Coaster
	true,	// 57 Mini Roller Coaster
	true,	// 58 Mine Ride
	true,	// 59 LIM Launched Roller Coaster
}

/**
 * Data about ride running costs. This is widely adjusted by the upkeep
 * function, so values that don't make much sense here (a roller coaster having
 * cheaper upkeep than a car ride) are fixed later on.
 *
 * data generation script: https://gist.github.com/kevinburke/6bcf4a8fcc95faad7bac
 */
const uint8 initialUpkeepCosts[0x59] = {
	41,	// 00 Spiral Roller coaster
	40,	// 01 Stand Up Coaster
	40,	// 02 Suspended Swinging
	40,	// 03 Inverted
	40,	// 04 Steel Mini Coaster
	60,	// 05 Mini Railroad
	65,	// 06 Monorail
	40,	// 07 Mini Suspended Coaster
	50,	// 08 Bumper Boats
	40,	// 09 Wooden Wild Mine/Mouse
	40,	// 0a Steeplechase/Motorbike/Soap Box Derby
	70,	// 0b Car Ride
	50,	// 0c Launched Freefall
	40,	// 0d Bobsleigh Coaster
	50,	// 0e Observation Tower
	40,	// 0f Looping Roller Coaster
	40,	// 10 Dinghy Slide
	40,	// 11 Mine Train Coaster
	60,	// 12 Chairlift
	40,	// 13 Corkscrew Roller Coaster
	50,	// 14 Maze
	50,	// 15 Spiral Slide
	50,	// 16 Go Karts
	80,	// 17 Log Flume
	82,	// 18 River Rapids
	50,	// 19 Bumper Cars
	50,	// 1a Pirate Ship
	50,	// 1b Swinging Inverter Ship
	50,	// 1c Food Stall
	50,	// 1d (none)
	50,	// 1e Drink Stall
	50,	// 1f (none)
	50,	// 20 Shop (all types)
	50,	// 21 Merry Go Round
	50,	// 22 Balloon Stall (maybe)
	50,	// 23 Information Kiosk
	50,	// 24 Bathroom
	50,	// 25 Ferris Wheel
	50,	// 26 Motion Simulator
	50,	// 27 3D Cinema
	50,	// 28 Gravitron
	50,	// 29 Space Rings
	80,	// 2a Reverse Freefall Coaster
	50,	// 2b Elevator
	44,	// 2c Vertical Drop Roller Coaster
	40,	// 2d ATM
	50,	// 2e Twist
	50,	// 2f Haunted House
	45,	// 30 First Aid
	50,	// 31 Circus Show
	80,	// 32 Ghost Train
	43,	// 33 Twister Roller Coaster
	40,	// 34 Wooden Roller Coaster
	39,	// 35 Side-Friction Roller Coaster
	40,	// 36 Wild Mouse
	75,	// 37 Multi Dimension Coaster
	75,	// 38 (none)
	49,	// 39 Flying Roller Coaster
	49,	// 3a (none)
	39,	// 3b Virginia Reel
	70,	// 3c Splash Boats
	70,	// 3d Mini Helicopters
	49,	// 3e Lay-down Roller Coaster
	70,	// 3f Suspended Monorail
	49,	// 40 (none)
	39,	// 41 Reverser Roller Coaster
	47,	// 42 Heartline Twister Roller Coaster
	30,	// 43 Mini Golf
	10,	// 44 Giga Coaster
	50,	// 45 Roto-Drop
	90,	// 46 Flying Saucers
	30,	// 47 Crooked House
	47,	// 48 Monorail Cycles
	40,	// 49 Compact Inverted Coaster
	60,	// 4a Water Coaster
	90,	// 4b Air Powered Vertical Coaster
	40,	// 4c Inverted Hairpin Coaster
	50,	// 4d Magic Carpet
	50,	// 4e Submarine Ride
	50,	// 4f River Rafts
	50,	// 50 (none)
	50,	// 51 Enterprise
	50,	// 52 (none)
	50,	// 53 (none)
	50,	// 54 (none)
	40,	// 55 (none)
	180,	// 56 Inverted Impulse Coaster
	35,	// 57 Mini Roller Coaster
	50,	// 58 Mine Ride
	42,	// 59 LIM Launched Roller Coaster
}

const uint8 costPerTrackPiece[0x59] = {
	80,	// 00 Spiral Roller coaster
	80,	// 01 Stand Up Coaster
	80,	// 02 Suspended Swinging
	80,	// 03 Inverted
	80,	// 04 Steel Mini Coaster
	0,	// 05 Mini Railroad
	0,	// 06 Monorail
	80,	// 07 Mini Suspended Coaster
	0,	// 08 Bumper Boats
	80,	// 09 Wooden Wild Mine/Mouse
	80,	// 0a Steeplechase/Motorbike/Soap Box Derby
	0,	// 0b Car Ride
	0,	// 0c Launched Freefall
	80,	// 0d Bobsleigh Coaster
	0,	// 0e Observation Tower
	80,	// 0f Looping Roller Coaster
	80,	// 10 Dinghy Slide
	80,	// 11 Mine Train Coaster
	0,	// 12 Chairlift
	80,	// 13 Corkscrew Roller Coaster
	0,	// 14 Maze
	0,	// 15 Spiral Slide
	0,	// 16 Go Karts
	0,	// 17 Log Flume
	0,	// 18 River Rapids
	0,	// 19 Bumper Cars
	0,	// 1a Pirate Ship
	0,	// 1b Swinging Inverter Ship
	0,	// 1c Food Stall
	0,	// 1d (none)
	0,	// 1e Drink Stall
	0,	// 1f (none)
	0,	// 20 Shop (all types)
	0,	// 21 Merry Go Round
	0,	// 22 Balloon Stall (maybe)
	0,	// 23 Information Kiosk
	0,	// 24 Bathroom
	0,	// 25 Ferris Wheel
	0,	// 26 Motion Simulator
	0,	// 27 3D Cinema
	0,	// 28 Gravitron
	0,	// 29 Space Rings
	0,	// 2a Reverse Freefall Coaster
	0,	// 2b Elevator
	80,	// 2c Vertical Drop Roller Coaster
	0,	// 2d ATM
	0,	// 2e Twist
	0,	// 2f Haunted House
	0,	// 30 First Aid
	0,	// 31 Circus Show
	0,	// 32 Ghost Train
	80,	// 33 Twister Roller Coaster
	80,	// 34 Wooden Roller Coaster
	80,	// 35 Side-Friction Roller Coaster
	80,	// 36 Wild Mouse
	90,	// 37 Multi Dimension Coaster
	90,	// 38 (none)
	90,	// 39 Flying Roller Coaster
	90,	// 3a (none)
	80,	// 3b Virginia Reel
	0,	// 3c Splash Boats
	0,	// 3d Mini Helicopters
	90,	// 3e Lay-down Roller Coaster
	0,	// 3f Suspended Monorail
	90,	// 40 (none)
	80,	// 41 Reverser Roller Coaster
	80,	// 42 Heartline Twister Roller Coaster
	80,	// 43 Mini Golf
	80,	// 44 Giga Coaster
	0,	// 45 Roto-Drop
	0,	// 46 Flying Saucers
	0,	// 47 Crooked House
	0,	// 48 Monorail Cycles
	80,	// 49 Compact Inverted Coaster
	80,	// 4a Water Coaster
	0,	// 4b Air Powered Vertical Coaster
	80,	// 4c Inverted Hairpin Coaster
	0,	// 4d Magic Carpet
	0,	// 4e Submarine Ride
	0,	// 4f River Rafts
	0,	// 50 (none)
	0,	// 51 Enterprise
	0,	// 52 (none)
	0,	// 53 (none)
	0,	// 54 (none)
	80,	// 55 (none)
	80,	// 56 Inverted Impulse Coaster
	80,	// 57 Mini Roller Coaster
	80,	// 58 Mine Ride
	80,	// 59 LIM Launched Roller Coaster
}

/**
 * Data initially at 0x0097E3B4
 */
const uint8 rideUnknownData1[0x59] = {
	10,	// 00 Spiral Roller coaster
	10,	// 01 Stand Up Coaster
	20,	// 02 Suspended Swinging
	13,	// 03 Inverted
	8,	// 04 Steel Mini Coaster
	10,	// 05 Mini Railroad
	10,	// 06 Monorail
	10,	// 07 Mini Suspended Coaster
	4,	// 08 Bumper Boats
	9,	// 09 Wooden Wild Mine/Mouse
	10,	// 0a Steeplechase/Motorbike/Soap Box Derby
	8,	// 0b Car Ride
	10,	// 0c Launched Freefall
	10,	// 0d Bobsleigh Coaster
	10,	// 0e Observation Tower
	10,	// 0f Looping Roller Coaster
	4,	// 10 Dinghy Slide
	10,	// 11 Mine Train Coaster
	4,	// 12 Chairlift
	11,	// 13 Corkscrew Roller Coaster
	0,	// 14 Maze
	0,	// 15 Spiral Slide
	8,	// 16 Go Karts
	9,	// 17 Log Flume
	10,	// 18 River Rapids
	5,	// 19 Bumper Cars
	0,	// 1a Pirate Ship
	0,	// 1b Swinging Inverter Ship
	0,	// 1c Food Stall
	0,	// 1d (none)
	0,	// 1e Drink Stall
	0,	// 1f (none)
	0,	// 20 Shop (all types)
	0,	// 21 Merry Go Round
	0,	// 22 Balloon Stall (maybe)
	0,	// 23 Information Kiosk
	0,	// 24 Bathroom
	0,	// 25 Ferris Wheel
	0,	// 26 Motion Simulator
	0,	// 27 3D Cinema
	0,	// 28 Gravitron
	0,	// 29 Space Rings
	0,	// 2a Reverse Freefall Coaster
	10,	// 2b Elevator
	11,	// 2c Vertical Drop Roller Coaster
	0,	// 2d ATM
	0,	// 2e Twist
	0,	// 2f Haunted House
	0,	// 30 First Aid
	0,	// 31 Circus Show
	8,	// 32 Ghost Train
	11,	// 33 Twister Roller Coaster
	10,	// 34 Wooden Roller Coaster
	10,	// 35 Side-Friction Roller Coaster
	9,	// 36 Wild Mouse
	11,	// 37 Multi Dimension Coaster
	11,	// 38 (none)
	11,	// 39 Flying Roller Coaster
	11,	// 3a (none)
	10,	// 3b Virginia Reel
	9,	// 3c Splash Boats
	8,	// 3d Mini Helicopters
	11,	// 3e Lay-down Roller Coaster
	10,	// 3f Suspended Monorail
	11,	// 40 (none)
	10,	// 41 Reverser Roller Coaster
	11,	// 42 Heartline Twister Roller Coaster
	11,	// 43 Mini Golf
	12,	// 44 Giga Coaster
	10,	// 45 Roto-Drop
	5,	// 46 Flying Saucers
	0,	// 47 Crooked House
	8,	// 48 Monorail Cycles
	13,	// 49 Compact Inverted Coaster
	8,	// 4a Water Coaster
	0,	// 4b Air Powered Vertical Coaster
	9,	// 4c Inverted Hairpin Coaster
	0,	// 4d Magic Carpet
	4,	// 4e Submarine Ride
	9,	// 4f River Rafts
	0,	// 50 (none)
	0,	// 51 Enterprise
	0,	// 52 (none)
	0,	// 53 (none)
	0,	// 54 (none)
	11,	// 55 (none)
	11,	// 56 Inverted Impulse Coaster
	8,	// 57 Mini Roller Coaster
	10,	// 58 Mine Ride
	9,	// 59 LIM Launched Roller Coaster
}

/**
 * Data at 0x0097E3B6, originally set to either be 3 or 0 and replaced here by
 * a boolean table. This may be exactly the same as hasRunningTrack above.
 */
const bool rideUnknownData2[0x59] = {
	true,	// 00 Spiral Roller coaster
	true,	// 01 Stand Up Coaster
	true,	// 02 Suspended Swinging
	true,	// 03 Inverted
	true,	// 04 Steel Mini Coaster
	true,	// 05 Mini Railroad
	true,	// 06 Monorail
	true,	// 07 Mini Suspended Coaster
	false,	// 08 Bumper Boats
	true,	// 09 Wooden Wild Mine/Mouse
	true,	// 0a Steeplechase/Motorbike/Soap Box Derby
	true,	// 0b Car Ride
	false,	// 0c Launched Freefall
	true,	// 0d Bobsleigh Coaster
	false,	// 0e Observation Tower
	true,	// 0f Looping Roller Coaster
	true,	// 10 Dinghy Slide
	true,	// 11 Mine Train Coaster
	true,	// 12 Chairlift
	true,	// 13 Corkscrew Roller Coaster
	false,	// 14 Maze
	false,	// 15 Spiral Slide
	false,	// 16 Go Karts
	false,	// 17 Log Flume
	false,	// 18 River Rapids
	false,	// 19 Bumper Cars
	false,	// 1a Pirate Ship
	false,	// 1b Swinging Inverter Ship
	false,	// 1c Food Stall
	false,	// 1d (none)
	false,	// 1e Drink Stall
	false,	// 1f (none)
	false,	// 20 Shop (all types)
	false,	// 21 Merry Go Round
	false,	// 22 Balloon Stall (maybe)
	false,	// 23 Information Kiosk
	false,	// 24 Bathroom
	false,	// 25 Ferris Wheel
	false,	// 26 Motion Simulator
	false,	// 27 3D Cinema
	false,	// 28 Gravitron
	false,	// 29 Space Rings
	false,	// 2a Reverse Freefall Coaster
	false,	// 2b Elevator
	true,	// 2c Vertical Drop Roller Coaster
	false,	// 2d ATM
	false,	// 2e Twist
	false,	// 2f Haunted House
	false,	// 30 First Aid
	false,	// 31 Circus Show
	true,	// 32 Ghost Train
	true,	// 33 Twister Roller Coaster
	true,	// 34 Wooden Roller Coaster
	true,	// 35 Side-Friction Roller Coaster
	true,	// 36 Wild Mouse
	true,	// 37 Multi Dimension Coaster
	true,	// 38 (none)
	true,	// 39 Flying Roller Coaster
	true,	// 3a (none)
	true,	// 3b Virginia Reel
	false,	// 3c Splash Boats
	true,	// 3d Mini Helicopters
	true,	// 3e Lay-down Roller Coaster
	true,	// 3f Suspended Monorail
	true,	// 40 (none)
	true,	// 41 Reverser Roller Coaster
	true,	// 42 Heartline Twister Roller Coaster
	true,	// 43 Mini Golf
	true,	// 44 Giga Coaster
	false,	// 45 Roto-Drop
	false,	// 46 Flying Saucers
	false,	// 47 Crooked House
	true,	// 48 Monorail Cycles
	true,	// 49 Compact Inverted Coaster
	true,	// 4a Water Coaster
	false,	// 4b Air Powered Vertical Coaster
	true,	// 4c Inverted Hairpin Coaster
	false,	// 4d Magic Carpet
	false,	// 4e Submarine Ride
	false,	// 4f River Rafts
	false,	// 50 (none)
	false,	// 51 Enterprise
	false,	// 52 (none)
	false,	// 53 (none)
	false,	// 54 (none)
	true,	// 55 (none)
	true,	// 56 Inverted Impulse Coaster
	true,	// 57 Mini Roller Coaster
	true,	// 58 Mine Ride
	true,	// 59 LIM Launched Roller Coaster
}

const uint8 rideUnknownData3[0x59] = {
	10,	// 00 Spiral Roller coaster
	10,	// 01 Stand Up Coaster
	10,	// 02 Suspended Swinging
	10,	// 03 Inverted
	5,	// 04 Steel Mini Coaster
	5,	// 05 Mini Railroad
	10,	// 06 Monorail
	10,	// 07 Mini Suspended Coaster
	0,	// 08 Bumper Boats
	10,	// 09 Wooden Wild Mine/Mouse
	10,	// 0a Steeplechase/Motorbike/Soap Box Derby
	5,	// 0b Car Ride
	0,	// 0c Launched Freefall
	10,	// 0d Bobsleigh Coaster
	0,	// 0e Observation Tower
	10,	// 0f Looping Roller Coaster
	10,	// 10 Dinghy Slide
	10,	// 11 Mine Train Coaster
	10,	// 12 Chairlift
	10,	// 13 Corkscrew Roller Coaster
	0,	// 14 Maze
	0,	// 15 Spiral Slide
	0,	// 16 Go Karts
	10,	// 17 Log Flume
	10,	// 18 River Rapids
	0,	// 19 Bumper Cars
	0,	// 1a Pirate Ship
	0,	// 1b Swinging Inverter Ship
	0,	// 1c Food Stall
	0,	// 1d (none)
	0,	// 1e Drink Stall
	0,	// 1f (none)
	0,	// 20 Shop (all types)
	0,	// 21 Merry Go Round
	0,	// 22 Balloon Stall (maybe)
	0,	// 23 Information Kiosk
	0,	// 24 Bathroom
	0,	// 25 Ferris Wheel
	0,	// 26 Motion Simulator
	0,	// 27 3D Cinema
	0,	// 28 Gravitron
	0,	// 29 Space Rings
	10,	// 2a Reverse Freefall Coaster
	0,	// 2b Elevator
	10,	// 2c Vertical Drop Roller Coaster
	0,	// 2d ATM
	0,	// 2e Twist
	0,	// 2f Haunted House
	0,	// 30 First Aid
	0,	// 31 Circus Show
	5,	// 32 Ghost Train
	10,	// 33 Twister Roller Coaster
	10,	// 34 Wooden Roller Coaster
	10,	// 35 Side-Friction Roller Coaster
	10,	// 36 Wild Mouse
	15,	// 37 Multi Dimension Coaster
	15,	// 38 (none)
	15,	// 39 Flying Roller Coaster
	15,	// 3a (none)
	10,	// 3b Virginia Reel
	10,	// 3c Splash Boats
	5,	// 3d Mini Helicopters
	15,	// 3e Lay-down Roller Coaster
	10,	// 3f Suspended Monorail
	15,	// 40 (none)
	10,	// 41 Reverser Roller Coaster
	10,	// 42 Heartline Twister Roller Coaster
	10,	// 43 Mini Golf
	40,	// 44 Giga Coaster
	0,	// 45 Roto-Drop
	0,	// 46 Flying Saucers
	0,	// 47 Crooked House
	5,	// 48 Monorail Cycles
	10,	// 49 Compact Inverted Coaster
	10,	// 4a Water Coaster
	10,	// 4b Air Powered Vertical Coaster
	10,	// 4c Inverted Hairpin Coaster
	0,	// 4d Magic Carpet
	0,	// 4e Submarine Ride
	10,	// 4f River Rafts
	0,	// 50 (none)
	0,	// 51 Enterprise
	0,	// 52 (none)
	0,	// 53 (none)
	0,	// 54 (none)
	10,	// 55 (none)
	10,	// 56 Inverted Impulse Coaster
	10,	// 57 Mini Roller Coaster
	10,	// 58 Mine Ride
	10,	// 59 LIM Launched Roller Coaster
}
