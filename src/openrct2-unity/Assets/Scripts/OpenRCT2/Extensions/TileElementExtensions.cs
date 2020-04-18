using System;
using UnityEngine;

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


        /// <summary>
        /// Dumps all values on this tile element to the console log.
        /// </summary>
		public static void DumpToConsole(this TileElement tile)
            => Console.WriteLine(DumpToString(tile));


        /// <summary>
        /// Dumps all values on this tile element to the debug log.
        /// </summary>
        public static void DumpToDebug(this TileElement tile)
            => Debug.Log(DumpToString(tile));


        static string DumpToString(TileElement tile)
        {
            return
$@"(me) TileElement:
(me)   -> {tile.type}   (type)
(me)   -> {tile.flags}  (flags)
(me)   -> {tile.baseHeight}     (base_height)
(me)   -> {tile.clearanceHeight}    (clearance_height)
(me)   -> {tile.slot0x1}    (pad 0x1)
(me)   -> {tile.slot0x2}    (pad 0x2)
(me)   -> {tile.slot0x3}    (pad 0x3)
(me)   -> {tile.slot0x4}    (pad 0x4)
(me)   -> {tile.slot0x5}    (pad 0x5)
(me)   -> {tile.slot0x6}    (pad 0x6)
(me)   -> {tile.slot0x7}    (pad 0x7)
(me)   -> {tile.slot0x8}    (pad 0x8)
(me)   -> {tile.slot0x9}    (pad 0x9)
(me)   -> {tile.slot0xA}    (pad 0xA)
(me)   -> {tile.slot0xB}    (pad 0xB)
(me)   -> {tile.slot0xC}    (pad 0xC)";
        }
	}
}
