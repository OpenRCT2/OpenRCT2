using System;

namespace OpenRCT2.Unity
{
    /// <summary>
    /// The type of element.
    /// </summary>
    [Flags]
    public enum TileElementType : byte
    {
        Surface = (0 << 2),
        Path = (1 << 2),
        Track = (2 << 2),
        SmallScenery = (3 << 2),
        Entrance = (4 << 2),
        Wall = (5 << 2),
        LargeScenery = (6 << 2),
        Banner = (7 << 2),
        Corrupt = (8 << 2),
    }
}
