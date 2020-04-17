namespace OpenRCT2.Unity
{
    /// <summary>
    /// An element representing a piece of path.
    /// </summary>
    public readonly struct PathElement
    {
        public TileElementType Type => element.type;


        /* 0x1 = SurfaceIndex
         * 0x2 = SurfaceIndex
         * 0x3 = RailingsIndex
         * 0x4 = RailingsIndex
         * 0x5 = Additions
         * 0x6 = Edges
         * 0x7 = Flags2
         * 0x8 = SlopeDirection
         * 0x9 = RideIndex / AdditionStatus
         * 0xA = RideIndex 
         * 0xB = StationIndex
         */
        readonly TileElement element;


        public PathElement(TileElement element)
        {
            this.element = element;
        }
    }
}
