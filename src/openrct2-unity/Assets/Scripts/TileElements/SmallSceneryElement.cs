namespace OpenRCT2.Unity
{
    /// <summary>
    /// An element representing a small scenery piece.
    /// </summary>
    public readonly struct SmallSceneryElement
    {
        public TileElementType Type => element.type;


        /* 0x1 = EntryIndex
         * 0x2 = EntryIndex
         * 0x3 = Age
         * 0x4 = Colour 1
         * 0x5 = Colour 2
         */
        readonly TileElement element;


        public SmallSceneryElement(TileElement element)
        {
            this.element = element;
        }
    }
}
