namespace OpenRCT2.Unity
{
    public enum PeepState : byte
    {
        Falling = 0, // drowning is part of falling
        QueuingFront = 2,
        On_ride = 3,
        LeavingRide = 4,
        Walking = 5,
        Queuing = 6,
        EnteringRide = 7,
        Sitting = 8,
        Picked = 9,
        Patrolling = 10, 
        Mowing = 11,
        Sweeping = 12,
        EnteringPark = 13,
        LeavingPark = 14,
        Answering = 15,
        Fixing = 16,
        Buying = 17,
        Watching = 18,
        EmptyingBin = 19,
        UsingBin = 20,
        Watering = 21,
        HeadingToInspection = 22,
        Inspecting = 23
    };
}
