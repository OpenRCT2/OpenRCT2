using System;

namespace OpenRCT2.Unity
{
    /// <summary>
    /// Flags of ownership of a specific tile.
    /// </summary>
    [Flags]
    public enum Ownership : byte
    {
        Unowned = 0,
        ConstructionRights = (1 << 4),
        Owned = (1 << 5),
        ConstructionRightsAvailable = (1 << 6),
        Available = (1 << 7)
    }
}
