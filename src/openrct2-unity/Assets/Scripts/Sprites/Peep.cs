using System;
using System.Runtime.InteropServices;
using UnityEngine;

namespace OpenRCT2.Unity
{
	[StructLayout(LayoutKind.Sequential, Size = 264)]
	public struct Peep
	{
		public SpriteBase sprite;          
		public IntPtr namePtr;             
		public int nextLocX;               
		public int nextLocY;               
		public int nextLocZ;               
	    public byte nextFlags;                                 
	    public byte outsideOfPark;                             
		public PeepState state;            
	    public byte substate;                                  
		public PeepSpriteType spriteType;  
		public PeepType type;              
	//  public byte staffTypeOrNoOfRides; // union of staff type or no. of rides.
	//  public byte tshirtColour;
	//  public byte trousersColour;
	//  public ushort destinationX;
	//  public ushort destinationY;
	//  public byte destinationTolerance;
	//  public byte var37;
	//  public byte energy;
	//  public byte energyTarget;
	//  public byte happiness;
	//  public byte happinessTarget;
	//  public byte hunger;
	//  public byte thirst;
	//  public byte toilet;
	//  public byte mass;
	//  public byte timeToCconsume;
	//  public byte intensity; // The max intensity is stored in the first 4 bits, and the min intensity in the second 4 bits
	//  public byte nauseaTolerance;
	//  public byte windowInvalidateFlags;
	//  public short paidOnDrink;
	//  public long rideTypesBeenOn1; // This is 16x a byte for a ride;
	//  public long rideTypesBeenOn2;
	//  public uint itemExtraFlags;
	//  public byte photo2RideRef;
	//  public byte photo3RideRef;
	//  public byte photo4RideRef;
	//  public byte currentRide;
	//  public byte stationIndex;
	//  public byte currentTrain;


        public ushort Id
            => sprite.spriteIndex;

		public string Name
			=> Marshal.PtrToStringAnsi(namePtr);


        public Vector3 Position
            => new Vector3(sprite.x, sprite.z, sprite.y);
	}
}
