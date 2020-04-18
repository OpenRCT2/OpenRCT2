using System.Runtime.InteropServices;

namespace OpenRCT2.Unity
{
    [StructLayout(LayoutKind.Sequential, Size = 31)]
    public struct SpriteBase
    {
        public byte spriteIdentifier;     // 0
        public byte type;                 // 1
        public ushort nextInQuadrant;     // 2,3
        public ushort next;               // 4,5
        public ushort previous;           // 6,7
        public byte linkedListIndex;      // 8;  Valid values are SPRITE_LINKEDLIST_OFFSET_...
        public byte spriteHeightNegative; // 9;  Height from centre of sprite to bottom
        public ushort spriteIndex;        // 10,11
        public ushort flags;              // 12,13
        public short x;                   // 14,15
        public short y;                   // 16,17
        public short z;                   // 18,19
        public byte spriteWidth;          // 20;  Width from centre of sprite to edge
        public byte spriteHeightPositive; // 21;  Height from centre of sprite to top

        // Screen Coordinates of sprite
        public short sprite_left;         // 22,23
        public short sprite_top;          // 24,25
        public short sprite_right;        // 26,27
        public short sprite_bottom;       // 28,29
        public byte sprite_direction;     // 30
    }
}
