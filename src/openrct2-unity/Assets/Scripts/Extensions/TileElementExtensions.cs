namespace OpenRCT2.Unity
{
    public static class TileElementExtensions
    {
        public static SurfaceElement AsSurface(this TileElement tileElement)
            => new SurfaceElement(tileElement);


        public static PathElement AsPath(this TileElement tileElement)
            => new PathElement(tileElement);


        public static TrackElement AsTrack(this TileElement tileElement)
            => new TrackElement(tileElement);


        public static SmallSceneryElement AsSmallScenery(this TileElement tileElement)
            => new SmallSceneryElement(tileElement);
    }
}
