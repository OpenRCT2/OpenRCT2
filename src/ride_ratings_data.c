/*
 * Whether a particular ride has a running track or not. Will probably end up
 * being used in various places in the game.
 */
const uint8 runningTrack[100] = {
	 1,	// 0 Spiral Roller coaster
	 1,	// 1 Stand Up Coaster
	 1,	// 2 Suspended Swinging
	 1,	// 3 Inverted
	 1,	// 4 Steel Mini Coaster
	 1,	// 5 Mini Railroad
	 1,	// 6 Monorail
	 1,	// 7 Mini Suspended Coaster
	 0,	// 8 Bumper Boats
	 1,	// 9 Wooden Wild Mine/Mouse
	 1,	// a Steeplechase/Motorbike/Soap Box Derby
	 1,	// b Car Ride
	 1,	// c Launched Freefall
	 1,	// d Bobsleigh Coaster
	 1,	// e Observation Tower
	 1,	// f Looping Roller Coaster
	 1,	// 10 Dinghy Slide
	 1,	// 11 Mine Train Coaster
	 1,	// 12 Chairlift
	 1,	// 13 Corkscrew Roller Coaster
	 0,	// 14 Maze
	 0,	// 15 Spiral Slide
	 1,	// 16 Go Karts
	 1,	// 17 Log Flume
	 1,	// 18 River Rapids
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
	 1,	// 2a Reverse Freefall Coaster
	 1,	// 2b Elevator
	 1,	// 2c Vertical Drop Roller Coaster
	 0,	// 2d ATM
	 0,	// 2e Twist
	 0,	// 2f Haunted House
	 0,	// 30 First Aid
	 0,	// 31 Circus Show
	 1,	// 32 Ghost Train
	 1,	// 33 Twister Roller Coaster
	 1,	// 34 Wooden Roller Coaster
	 1,	// 35 Side-Friction Roller Coaster
	 1,	// 36 Wild Mouse
	 1,	// 37 Multi Dimension Coaster
	 1,	// 38 (none)
	 1,	// 39 Flying Roller Coaster
	 1,	// 3a (none)
	 1,	// 3b Virginia Reel
	 1,	// 3c Splash Boats
	 1,	// 3d Mini Helicopters
	 1,	// 3e Lay-down Roller Coaster
	 1,	// 3f Suspended Monorail
	 1,	// 40 (none)
	 1,	// 41 Reverser Roller Coaster
	 1,	// 42 Heartline Twister Roller Coaster
	 1,	// 43 Mini Golf
	 1,	// 44 Giga Coaster
	 1,	// 45 Roto-Drop
	 0,	// 46 Flying Saucers
	 0,	// 47 Crooked House
	 1,	// 48 Monorail Cycles
	 1,	// 49 Compact Inverted Coaster
	 1,	// 4a Water Coaster
	 1,	// 4b Air Powered Vertical Coaster
	 1,	// 4c Inverted Hairpin Coaster
	 0,	// 4d Magic Carpet
	 0,	// 4e Submarine Ride
	 1,	// 4f River Rafts
	 0,	// 50 (none)
	 0,	// 51 Enterprise
	 0,	// 52 (none)
	 0,	// 53 (none)
	 0,	// 54 (none)
	 1,	// 55 (none)
	 1,	// 56 Inverted Impulse Coaster
	 1,	// 57 Mini Roller Coaster
	 1,	// 58 Mine Ride
	 1,	// 59 LIM Launched Roller Coaster
}

