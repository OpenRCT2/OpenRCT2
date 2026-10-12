// World-depth G-buffer: shared by the sprite pass that writes it (drawrect), the shadow
// resolve that reads two of them (dropshadow) and the composite (applypalette).
//
// Stored as RGBA16UI. World coords reach 1001*32 = 32032 so they fit a uint16 outright, world
// Z never exceeds a few thousand so 13 bits is ample, and a terrain surface's baseZ is always a
// multiple of 8 (baseHeight * kCoordsZStep) so it is stored shifted down by 3, leaving room for
// the slope.
//   R = worldX                    G = worldY
//   B = (mode << 13) | worldZ     A = terrain: (baseZ >> 3) | (slope << 8);  wall: flag bits
//                                     roof: surface gradient, see roofGradient()
// mode 0 means nothing wrote depth here, which is also what the buffer clears to.

const uint DEPTH_Z_BITS = 13u;
const uint DEPTH_Z_MASK = 0x1FFFu;

// Must match SpriteWorldDepthMode.
const uint DEPTH_MODE_NONE = 0u;
const uint DEPTH_MODE_FLAT = 1u;
const uint DEPTH_MODE_BILLBOARD = 2u;
const uint DEPTH_MODE_WALL = 3u;
const uint DEPTH_MODE_TERRAIN = 5u;
const uint DEPTH_MODE_TERRAIN_CAP = 6u;
// The sloped top of a roof prop. Receives like a floor; its gable ends are written as billboard.
const uint DEPTH_MODE_ROOF = 7u;

// Wall surface flags (A channel).
const uint WALL_FACES_SUN = 1u;
const uint WALL_IS_TERRAIN_SIDE = 2u;
const uint WALL_RUNS_ALONG_Y = 4u;
// Set over any surface by a translucent UI panel drawn across it (OR-ed in, so the record
// underneath survives). The composite shades such pixels at reduced strength.
const uint UNDER_TRANSLUCENT_UI = 0x8000u;

uint depthMode(uvec4 depth)
{
    return depth.b >> DEPTH_Z_BITS;
}

int depthWorldZ(uvec4 depth)
{
    return int(depth.b & DEPTH_Z_MASK);
}

// Two wall texels on one vertical plane: same axis and the same constant coordinate. The
// solve keeps a wall inside its owning tile, so faces of neighbouring tiles along one cliff
// line share that coordinate, while the opposite face of the same boundary does not.
bool wallsCoplanar(uvec4 a, uvec4 b)
{
    if (depthMode(a) != DEPTH_MODE_WALL || depthMode(b) != DEPTH_MODE_WALL)
        return false;
    uint axis = a.a & WALL_RUNS_ALONG_Y;
    if (axis != (b.a & WALL_RUNS_ALONG_Y))
        return false;
    return axis != 0u ? a.r == b.r : a.g == b.g;
}

bool isTerrainMode(uint mode)
{
    return mode == DEPTH_MODE_TERRAIN || mode == DEPTH_MODE_TERRAIN_CAP;
}

// Surfaces solved to a real world position per pixel, so they can be tested for shadow. A
// billboard is a sprite card standing at one tile's centre: it casts but cannot receive.
bool isReceiverMode(uint mode)
{
    return mode == DEPTH_MODE_FLAT || mode == DEPTH_MODE_WALL || mode == DEPTH_MODE_ROOF || isTerrainMode(mode);
}

// A roof stores dz/dx and dz/dy at the pixel in eighths, 6 signed bits each, so the resolve can
// tell a slope turned away from the sun without knowing the roof's shape.
const float ROOF_GRADIENT_SCALE = 8.0;

uint encodeRoofGradient(vec2 gradient)
{
    ivec2 q = clamp(ivec2(round(gradient * ROOF_GRADIENT_SCALE)), ivec2(-31), ivec2(31)) + 32;
    return uint(q.x) | (uint(q.y) << 6);
}

vec2 roofGradient(uvec4 surface)
{
    ivec2 q = ivec2(int(surface.a & 63u), int((surface.a >> 6) & 63u)) - 32;
    return vec2(q) / ROOF_GRADIENT_SCALE;
}

ivec2 rotateCoords(ivec2 p, int dir)
{
    dir &= 3;
    if (dir == 1)
        return ivec2(p.y, -p.x);
    if (dir == 2)
        return ivec2(-p.x, -p.y);
    if (dir == 3)
        return ivec2(-p.y, p.x);
    return p;
}

// Matches Translate3DTo2DWithZ exactly, including the arithmetic shift (floor).
ivec2 translate3DTo2D(ivec2 worldXY, int worldZ, int rotation)
{
    ivec2 r = rotateCoords(worldXY, rotation);
    return ivec2(r.y - r.x, ((r.x + r.y) >> 1) - worldZ);
}

// Terrain geometry, shared by the solver that reconstructs a surface and the resolve that
// judges which way it faces.
// clang-format off
const int kCoordsXYStep             = 32;
const int kLandHeightStep           = 16;
const int kTileSlopeNCornerUp       = 1;
const int kTileSlopeECornerUp       = 2;
const int kTileSlopeSCornerUp       = 4;
const int kTileSlopeWCornerUp       = 8;
const int kTileSlopeRaisedCornersMask = 15;
const int kTileSlopeDiagonalFlag    = 16;
const int kTileSlopeNESideUp        = 3;
const int kTileSlopeSESideUp        = 6;
const int kTileSlopeNWSideUp        = 9;
const int kTileSlopeSWSideUp        = 12;
// "corner down" = all corners raised EXCEPT that one, i.e. kTileSlopeRaisedCornersMask & ~corner.
const int kTileSlopeWCornerDown     = 7;  // 15 & ~8
const int kTileSlopeSCornerDown     = 11; // 15 & ~4
const int kTileSlopeECornerDown     = 13; // 15 & ~2
const int kTileSlopeNCornerDown     = 14; // 15 & ~1
const int kTileSlopeWEValley        = 10;
const int kTileSlopeNSValley        = 5;
// clang-format on

// Exact port of Map.cpp TileElementHeight (integer maths, incl. truncating division).
// SW corner is camera-near (lower diamond tip).
int tileElementHeight(ivec2 worldXY, int baseZ, int slopeIn)
{
    int height = baseZ;
    int slope = slopeIn;
    int extraHeight = (slope & kTileSlopeDiagonalFlag) >> 4;
    slope &= kTileSlopeRaisedCornersMask;

    int xl = worldXY.x & 0x1f;
    int yl = worldXY.y & 0x1f;
    int quad = 0;
    int quadExtra = 0;

    if (slope == kTileSlopeNCornerUp || slope == kTileSlopeECornerUp || slope == kTileSlopeSCornerUp
        || slope == kTileSlopeWCornerUp)
    {
        if (slope == kTileSlopeNCornerUp)
            quad = xl + yl - kCoordsXYStep;
        else if (slope == kTileSlopeECornerUp)
            quad = xl - yl;
        else if (slope == kTileSlopeSCornerUp)
            quad = kCoordsXYStep - yl - xl;
        else
            quad = yl - xl;
        if (quad > 0)
            height += quad / 2;
    }

    if (slope == kTileSlopeNESideUp)
        height += xl / 2;
    else if (slope == kTileSlopeSESideUp)
        height += (kCoordsXYStep - yl) / 2;
    else if (slope == kTileSlopeNWSideUp)
        height += yl / 2;
    else if (slope == kTileSlopeSWSideUp)
        height += (kCoordsXYStep - xl) / 2;

    if (slope == kTileSlopeWCornerDown || slope == kTileSlopeSCornerDown || slope == kTileSlopeECornerDown
        || slope == kTileSlopeNCornerDown)
    {
        if (slope == kTileSlopeWCornerDown)
        {
            quadExtra = xl + kCoordsXYStep - yl;
            quad = xl - yl;
        }
        else if (slope == kTileSlopeSCornerDown)
        {
            quadExtra = xl + yl;
            quad = xl + yl - kCoordsXYStep;
        }
        else if (slope == kTileSlopeECornerDown)
        {
            quadExtra = kCoordsXYStep - xl + yl;
            quad = yl - xl;
        }
        else
        {
            quadExtra = (kCoordsXYStep - xl) + (kCoordsXYStep - yl);
            quad = kCoordsXYStep - yl - xl;
        }

        if (extraHeight != 0)
            return height + quadExtra / 2;

        height += kLandHeightStep;
        if (quad < 0)
        {
            // This is rendered surface geometry, so complementary triangles must describe
            // the same plane at their shared edge. Integer division truncates -1 / 2 to 0,
            // putting every odd sample of a descending corner one unit above the matching
            // rising corner. Arithmetic shift floors the negative half and closes that seam.
            height += quad >> 1;
        }
    }

    if (slope == kTileSlopeWEValley || slope == kTileSlopeNSValley)
    {
        if (slope == kTileSlopeWEValley)
            quad = abs(xl - yl);
        else
            quad = abs(xl + yl - kCoordsXYStep);
        height += quad / 2;
    }

    return height;
}
