namespace OpenRCT2.Unity
{
    /// <summary>
    /// An element representing the surface of the map.
    /// </summary>
    public readonly struct SurfaceElement
    {
        public TileElementType Type => element.type;

        public SurfaceSlope Slope => (SurfaceSlope)element.slot0x1;

        public Ownership Ownership => (Ownership)element.slot0x4;

        public TerrainSurfaceStyle SurfaceStyle => (TerrainSurfaceStyle)element.slot0x5;

        public TerrainEdgeStyle EdgeStyle => (TerrainEdgeStyle)element.slot0x6;


        /* 0x1 = Slope
         * 0x2 = WaterHeight
         * 0x3 = GrassLength
         * 0x4 = Ownership
         * 0x5 = SurfaceStyle
         * 0x6 = EdgeStyle
         */
        readonly TileElement element;


        public SurfaceElement(TileElement element)
        {
            this.element = element;
        }
    }
}
