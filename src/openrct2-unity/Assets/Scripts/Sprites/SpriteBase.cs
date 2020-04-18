using System.Runtime.InteropServices;

namespace OpenRCT2.Unity
{
    [StructLayout(LayoutKind.Sequential)]
    public struct SpriteBase
    {
        public byte spriteIdentifier;
        public byte type;
        public ushort nextInQuadrant;
        public ushort next;
        public ushort previous;
        public byte linkedListIndex; // Valid values are SPRITE_LINKEDLIST_OFFSET_...
        public byte spriteHeightNegative; // Height from centre of sprite to bottom
        public ushort spriteIndex;
        public ushort flags;
        public short x;
        public short y;
        public short z;
        public byte spriteWidth; // Width from centre of sprite to edge
        public byte spriteHeightPositive; // Height from centre of sprite to top

        // Screen Coordinates of sprite
        public short sprite_left;
        public short sprite_top;
        public short sprite_right;
        public short sprite_bottom;
        public byte sprite_direction;
    }
}
