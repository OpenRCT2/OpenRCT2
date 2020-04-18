using System;

namespace OpenRCT2.Unity
{
    [Flags]
    public enum SurfaceSlope : byte
    {
        Flat = 0,
        AllCornersUp = 0x0F,

        NorthUp = (1 << 0),
        EastUp = (1 << 1),
        SouthUp = (1 << 2),
        WestUp = (1 << 3),
        DoubleHeight = (1 << 4),

        NorthDown = (AllCornersUp & ~NorthUp),
        EastDown = (AllCornersUp & ~EastUp),
        SouthDown = (AllCornersUp & ~SouthUp),
        WestDown = (AllCornersUp & ~WestUp),

        NorthEastUp = (NorthUp | EastUp),
        SouthEastUp = (SouthUp | EastUp),
        NorthWestUp = (NorthUp | WestUp),
        SouthWestUp = (SouthUp | WestUp),

        NorthSouthValley = (NorthUp | SouthUp),
        WestEastValley = (EastUp | WestUp),
    }
}
