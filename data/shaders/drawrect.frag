#version 330 core

// clang-format off
const int MASK_REMAP_COUNT          = 3;
const int FLAG_NO_TEXTURE           = (1 << 2);
const int FLAG_MASK                 = (1 << 3);
const int FLAG_CROSS_HATCH          = (1 << 4);
const int FLAG_TTF_TEXT             = (1 << 5);
const int FLAG_WORLD_DEPTH_ONLY     = (1 << 6);
// clang-format on

#include "worlddepth.glsl"

// clang-format off
uniform usampler2DArray uTexture;
uniform usampler2D      uPaletteTex;

uniform sampler2D       uPeelingTex;
uniform bool            uPeeling;
uniform bool            uShadowMapPass;

in vec4 gl_FragCoord;

flat in int             fFlags;
flat in uint            fColour;
flat in vec4            fTexColour;
flat in vec4            fTexMask;
flat in vec3            fPalettes;

flat in vec2            fPosition;
in vec3                 fPeelPos;
flat in float           fZoom;
flat in int             fTexColourAtlas;
flat in int             fTexMaskAtlas;
flat in int             fScreenHeight;
flat in vec4            fWorld0;
flat in vec4            fWorld1;
// clang-format on

layout(location = 0) out uint oColour;
layout(location = 1) out uvec4 oWorldDepth;

ivec2 viewportPosToMapPos(ivec2 view, int z, int rotation)
{
    // Invert sx = y - x and sy = floor((x + y) / 2) - z. The two halves of an odd sx
    // cannot use the same rounding: floor(sx / 2) belongs to x and ceil(sx / 2) belongs
    // to y. Using floor for both reconstructed sx - 1 for every odd destination column.
    int halfFloor = view.x >> 1;
    int halfCeil = view.x - halfFloor;
    ivec2 ret = ivec2(view.y - halfFloor + z, view.y + halfCeil + z);
    int inv = (rotation * 3) % 4;
    return rotateCoords(ret, inv);
}

// Roof props, in the tile's own frame (0..1 across it). The shape code packs shape | rise
// direction << 4 | height << 8. Direction 0 rises toward +X, 1 toward -Y, 2 toward -X, 3 toward
// +Y; ridges and barrels only use its axis.
// Command-only depth mode (never stored): a bulky prop solved against its bound box.
const uint DEPTH_MODE_BOX = 8u;

const int ROOF_FLAT = 0;
const int ROOF_WEDGE = 1;
const int ROOF_RIDGE = 2;
const int ROOF_BARREL = 3;
const int ROOF_HIP = 4;
const int ROOF_PLATE = 5; // a wedge's top alone, ROOF_PLATE_THICKNESS thick
const float ROOF_PLATE_THICKNESS = 2.0;

float roofHeight(vec2 worldXY, ivec2 tileOrigin, int baseZ, int code)
{
    int shape = code & 15;
    int dir = (code >> 4) & 3;
    float height = float((code >> 8) & 255);
    vec2 local = clamp((worldXY - vec2(tileOrigin) + 0.5) / float(kCoordsXYStep), 0.0, 1.0);
    float t = dir == 0 ? local.x : dir == 1 ? 1.0 - local.y : dir == 2 ? 1.0 - local.x : local.y;
    float profile = 1.0;
    if (shape == ROOF_WEDGE || shape == ROOF_PLATE)
        profile = t;
    else if (shape == ROOF_RIDGE)
        profile = 1.0 - abs(2.0 * t - 1.0);
    else if (shape == ROOF_BARREL)
        profile = sqrt(max(0.0, 1.0 - (2.0 * t - 1.0) * (2.0 * t - 1.0)));
    else if (shape == ROOF_HIP)
        profile = clamp(4.0 * min(min(local.x, 1.0 - local.x), min(local.y, 1.0 - local.y)), 0.0, 1.0);
    return float(baseZ) + height * profile;
}

void main()
{
    if (uPeeling)
    {
        float peel = texture(uPeelingTex, fPeelPos.xy).r;
        if (peel == 0.0 || fPeelPos.z >= peel)
        {
            discard;
        }
    }

    vec2 fragCoord = vec2(floor(gl_FragCoord.x), float(fScreenHeight) - floor(gl_FragCoord.y) - 1.0);
    vec2 position = (fragCoord - fPosition) * fZoom;

    uint texel;
    if ((fFlags & FLAG_NO_TEXTURE) == 0)
    {
        float colourU = (fTexColour.x + position.x) / fTexColour.z;
        float colourV = (fTexColour.y + position.y) / fTexColour.w;
        texel = texture(uTexture, vec3(colourU, colourV, fTexColourAtlas)).r;
        if (texel == 0u)
        {
            discard;
        }
        if ((fFlags & FLAG_TTF_TEXT) == 0)
        {
            texel += fColour;
        }
        else
        {
            uint hint_thresh = uint(fFlags & 0xff00) >> 8;
            if (hint_thresh > 0u)
            {
                bool solidColor = texel > 180u;
                texel = (texel > hint_thresh) ? fColour : 0u;
                texel = texel << 8;
                if (solidColor)
                {
                    texel += 1u;
                }
            }
            else
            {
                texel = fColour;
            }
        }
    }
    else
    {
        texel = fColour;
    }

    int paletteCount = fFlags & MASK_REMAP_COUNT;
    if (paletteCount >= 3 && texel >= 0x2Eu && texel < 0x3Au)
    {
        texel = texture(uPaletteTex, vec2(texel + 0xC5u, fPalettes.z) / 256.0f).r;
    }
    else if (paletteCount >= 2 && texel >= 0xCAu && texel < 0xD6u)
    {
        texel = texture(uPaletteTex, vec2(texel + 0x29u, fPalettes.y) / 256.f).r;
    }
    else if (paletteCount >= 1)
    {
        texel = texture(uPaletteTex, vec2(texel, fPalettes.x) / 256.f).r;
    }

    if (texel == 0u)
    {
        discard;
    }

    if ((fFlags & FLAG_CROSS_HATCH) != 0)
    {
        int posSum = int(position.x) + int(position.y);
        if ((posSum % 2) != 0)
        {
            discard;
        }
    }

    if ((fFlags & FLAG_MASK) != 0)
    {
        float maskU = (fTexMask.x + position.x) / fTexMask.z;
        float maskV = (fTexMask.y + position.y) / fTexMask.w;
        uint mask = texture(uTexture, vec3(maskU, maskV, fTexMaskAtlas)).r;
        if (mask == 0u)
        {
            discard;
        }
    }

    oColour = texel;

    // fWorld1.z packs the depth mode with the rotation of the pass that emitted this
    // command, so viewports with different rotations can coexist in one G-buffer.
    int packedMode = int(fWorld1.z);
    uint depthMode = uint(packedMode & 0xFF);
    int rotation = (packedMode >> 8) & 3;

    int worldZ = 0;
    ivec2 worldXY = ivec2(0);
    int terrainBaseZ = 0;
    int terrainSlope = 0;
    // Walls only: does the face we are looking at point at the sun? A wall turned away from
    // the light is already drawn dark by the art, and that is a lighting fact, not a shadow
    // one - nothing stands between it and the sun, it simply never catches the light. Laying
    // cast shadow over it attenuates a second time and muddies it, so the resolve skips it.
    bool wallFacesSun = false;
    bool wallIsTerrainSide = false;
    bool wallRunsAlongY = false;
    uint roofSurface = 0u;

    if (depthMode == DEPTH_MODE_BILLBOARD)
    {
        float zMin = fWorld0.x;
        float zMax = fWorld0.y;
        worldXY = ivec2(int(fWorld0.z), int(fWorld0.w));
        float zf = zMax;
        float yRange = fWorld1.y - fWorld1.x;
        if (yRange > 1e-3)
        {
            float t = clamp((fragCoord.y - fWorld1.x) / yRange, 0.0, 1.0);
            zf = mix(zMax, zMin, t);
        }
        worldZ = int(zf);
    }
    else if (depthMode == DEPTH_MODE_WALL)
    {
        // A wall runs 32 units along one tile edge. That segment stays axis-aligned under
        // every 90-degree rotation, so screen X is affine along it and the inverse is exact
        // - no iteration, unlike the sloped floor above.
        vec2 viewF = vec2(fragCoord.x + fWorld1.x, fragCoord.y + fWorld1.y) * fWorld1.w;
        ivec2 view = ivec2(int(floor(viewF.x)), int(floor(viewF.y)));
        ivec2 edgeOrigin = ivec2(int(fWorld0.x), int(fWorld0.y));
        int zMin = int(fWorld0.z);
        int zMax = int(fWorld0.w);
        ivec2 step = ((packedMode >> 10) & 1) == 0 ? ivec2(1, 0) : ivec2(0, 1);

        ivec2 r0 = rotateCoords(edgeOrigin, rotation);
        ivec2 rs = rotateCoords(step, rotation);
        // Rotating an axis unit vector always leaves this +-1, so the divide is exact.
        int slopeAlongEdge = rs.y - rs.x;
        int along = clamp((view.x - (r0.y - r0.x)) / slopeAlongEdge, 0, kCoordsXYStep - 1);
        worldXY = edgeOrigin + step * along;
        // Keep the wall inside the tile that owns it. A boundary edge would otherwise carry
        // the next tile's coordinates, and the resolve pass - which identifies a tile by
        // masking off the low 5 bits - would credit a pillar's own side to the tile it is
        // supposed to be shadowing, and then discard that shadow as self-shadowing.
        ivec2 ownerTile = edgeOrigin
            - ivec2(((packedMode >> 11) & 1) * kCoordsXYStep, ((packedMode >> 12) & 1) * kCoordsXYStep);
        worldXY = clamp(worldXY, ownerTile, ownerTile + (kCoordsXYStep - 1));

        ivec2 r = rotateCoords(worldXY, rotation);
        worldZ = clamp(((r.x + r.y) >> 1) - view.y, zMin, zMax);

        // The paint pass records the actual face, not merely the edge axis. Parallel near and
        // far faces have the same axis but point in opposite directions.
        wallFacesSun = ((packedMode >> 13) & 1) != 0;
        wallIsTerrainSide = ((packedMode >> 14) & 1) != 0;
        wallRunsAlongY = step.y != 0;
    }
    else if (depthMode == DEPTH_MODE_FLAT || isTerrainMode(depthMode))
    {
        // Isometric floor: SW tip is camera-near. Z depends on XY on a slope and XY
        // depends on Z, so solve by fixed-point iteration from the tile's base height.
        ivec2 tileOrigin = ivec2(int(fWorld0.x), int(fWorld0.y));
        int baseZ = int(fWorld0.z);
        int slope = int(fWorld0.w);
        terrainBaseZ = baseZ;
        terrainSlope = slope;
        // (dest pixel + RT offset) is a view-space coord only at zoom 0; scale it or the
        // tile reconstruction lands on the wrong world XY.
        vec2 viewF = vec2(fragCoord.x + fWorld1.x, fragCoord.y + fWorld1.y) * fWorld1.w;
        ivec2 view = ivec2(int(floor(viewF.x)), int(floor(viewF.y)));

        // Seed from the tile's CENTRE height, not baseZ. Several slope shapes sit well
        // above baseZ everywhere on the tile - the diagonal corner-down ones reach
        // baseZ+32 - so seeding at baseZ starts a whole tile off and the iteration
        // cannot walk back before it is clamped to the tile edge. The centre sample is
        // within about half a land step of the truth for every slope shape.
        worldZ = tileElementHeight(tileOrigin + (kCoordsXYStep / 2), baseZ, slope);
        int prevZ = worldZ;
        for (int i = 0; i < 5; i++)
        {
            prevZ = worldZ;
            ivec2 mapPos = viewportPosToMapPos(view, worldZ, rotation);
            worldXY = clamp(mapPos, tileOrigin, tileOrigin + (kCoordsXYStep - 1));
            worldZ = tileElementHeight(worldXY, baseZ, slope);
        }
        // The iteration z -> tileElementHeight(mapPos(z)) has derivative dH/dx + dH/dy.
        // Slope shapes where that is -1 (S corner up, N/S valley, N corner down, and the
        // diagonal N corner down) never settle: they flip between two values straddling
        // the answer, so their midpoint is the exact fixed point. Every other shape has
        // already converged by now, which makes this a no-op for them.
        int mixZ = (prevZ + worldZ) / 2;
        worldXY = clamp(viewportPosToMapPos(view, mixZ, rotation), tileOrigin, tileOrigin + (kCoordsXYStep - 1));
        worldZ = tileElementHeight(worldXY, baseZ, slope);
    }

    else if (depthMode == DEPTH_MODE_ROOF)
    {
        // Walk the pixel's view ray down through the roof's solid - its profile over the tile,
        // standing on baseZ - one unit of Z at a time; each step moves one unit along X and Y,
        // so nothing is skipped. The first sample inside the solid is what this pixel shows.
        // Entered from above, that is the sloped top, which receives. Entered through the side
        // of the tile column, it is a gable end, which only casts.
        ivec2 tileOrigin = ivec2(int(fWorld0.x), int(fWorld0.y));
        int baseZ = int(fWorld0.z);
        int code = int(fWorld0.w);
        vec2 viewF = vec2(fragCoord.x + fWorld1.x, fragCoord.y + fWorld1.y) * fWorld1.w;
        ivec2 view = ivec2(int(floor(viewF.x)), int(floor(viewF.y)));
        int top = baseZ + ((code >> 8) & 255);

        bool hit = false;
        bool fromAbove = true;
        worldXY = clamp(viewportPosToMapPos(view, baseZ, rotation), tileOrigin, tileOrigin + (kCoordsXYStep - 1));
        worldZ = baseZ;
        for (int z = top; z >= baseZ; z--)
        {
            ivec2 mapPos = viewportPosToMapPos(view, z, rotation);
            bool inside = all(greaterThanEqual(mapPos, tileOrigin)) && all(lessThan(mapPos, tileOrigin + kCoordsXYStep));
            float surface = inside ? roofHeight(vec2(mapPos), tileOrigin, baseZ, code) : 0.0;
            bool underPlate = (code & 15) == ROOF_PLATE && float(z) < surface - ROOF_PLATE_THICKNESS;
            if (inside && float(z) <= surface && !underPlate)
            {
                hit = true;
                worldXY = mapPos;
                worldZ = z;
                break;
            }
            fromAbove = inside;
        }
        // Where roof pieces meet, the ray crosses into this tile's column a unit or two under the
        // shared surface: it came over the neighbour's edge, not through a gable, so it is still
        // the top. Only a side entry well below the surface is a gable end.
        const float ROOF_SEAM_DEPTH = 2.5;
        if (hit && !fromAbove && roofHeight(vec2(worldXY), tileOrigin, baseZ, code) - float(worldZ) <= ROOF_SEAM_DEPTH)
        {
            // Lift it onto the surface it shows, or it sits under its neighbour's edge and the
            // sun sees that edge above it.
            worldZ = int(floor(roofHeight(vec2(worldXY), tileOrigin, baseZ, code)));
            fromAbove = true;
        }
        // Art the solid does not cover - eaves, trim, a pixel of overhang at the edge - goes on the
        // nearest point of the top rather than being dropped from receiving.
        // That placement is a guess, good enough to shade the pixel but not to cast from: from the
        // sun, a clamped edge guess can stand above the neighbouring piece and speck it. So the sun
        // pass leaves these out.
        if (!hit && uShadowMapPass)
        {
            depthMode = DEPTH_MODE_NONE;
        }
        else if (!hit)
        {
            worldZ = top;
            for (int i = 0; i < 4; i++)
            {
                worldXY = clamp(viewportPosToMapPos(view, worldZ, rotation), tileOrigin, tileOrigin + (kCoordsXYStep - 1));
                worldZ = int(roofHeight(vec2(worldXY), tileOrigin, baseZ, code));
            }
            hit = true;
            fromAbove = true;
        }
        if (hit && fromAbove)
        {
            vec2 p = vec2(worldXY);
            vec2 gradient = vec2(
                                roofHeight(p + vec2(1.0, 0.0), tileOrigin, baseZ, code)
                                    - roofHeight(p - vec2(1.0, 0.0), tileOrigin, baseZ, code),
                                roofHeight(p + vec2(0.0, 1.0), tileOrigin, baseZ, code)
                                    - roofHeight(p - vec2(0.0, 1.0), tileOrigin, baseZ, code))
                * 0.5;
            roofSurface = encodeRoofGradient(gradient);
        }
        else if (hit)
        {
            depthMode = DEPTH_MODE_BILLBOARD;
        }
    }

    else if (depthMode == DEPTH_MODE_BOX)
    {
        // Along a pixel's view ray each world axis moves one unit per unit of Z, so each axis of
        // the box admits one interval of Z; the ray enters the box at the top of their overlap.
        ivec2 boxMin = ivec2(int(fWorld0.x), int(fWorld0.y));
        int z0 = int(fWorld0.z);
        int sizes = int(fWorld0.w);
        ivec2 boxMax = boxMin + ivec2(sizes & 63, (sizes >> 6) & 63) - 1;
        int z1 = z0 + (sizes >> 12);
        vec2 viewF = vec2(fragCoord.x + fWorld1.x, fragCoord.y + fWorld1.y) * fWorld1.w;
        ivec2 view = ivec2(int(floor(viewF.x)), int(floor(viewF.y)));
        ivec2 w0 = viewportPosToMapPos(view, 0, rotation);
        ivec2 d = viewportPosToMapPos(view, 1, rotation) - w0;
        // The Z interval over which the ray is inside the box's footprint.
        int lo = -100000;
        int hi = 100000;
        for (int axis = 0; axis < 2; axis++)
        {
            int a = axis == 0 ? w0.x : w0.y;
            int da = axis == 0 ? d.x : d.y;
            int bmin = axis == 0 ? boxMin.x : boxMin.y;
            int bmax = axis == 0 ? boxMax.x : boxMax.y;
            if (da > 0)
            {
                lo = max(lo, bmin - a);
                hi = min(hi, bmax - a);
            }
            else
            {
                lo = max(lo, a - bmax);
                hi = min(hi, a - bmin);
            }
        }
        if (max(lo, z0) <= min(hi, z1))
        {
            worldZ = min(hi, z1);
            worldXY = w0 + d * worldZ;
        }
        else if (lo <= hi && lo > z1)
        {
            // Art above the box - the rails of a sloped track piece climb well past its bound. A track
            // runs along its box's long side with the rail centred across the short one, so the rail
            // is where the ray crosses that centre line. Capped at the box top as a card, the upper end
            // of every sloped piece cast nothing.
            bool narrowX = (boxMax.x - boxMin.x) < (boxMax.y - boxMin.y);
            int centre = narrowX ? (boxMin.x + boxMax.x + 1) / 2 : (boxMin.y + boxMax.y + 1) / 2;
            int a = narrowX ? w0.x : w0.y;
            int da = narrowX ? d.x : d.y;
            worldZ = clamp((centre - a) * da, lo, hi);
            worldXY = w0 + d * worldZ;
        }
        else
        {
            // Art beside the box: a card through its centre, floored at the box base.
            worldXY = (boxMin + boxMax + 1) / 2;
            ivec2 r = rotateCoords(worldXY, rotation);
            worldZ = max(((r.x + r.y) >> 1) - view.y, z0);
        }
        depthMode = DEPTH_MODE_BILLBOARD;
    }

    bool hasWorldPosition = isReceiverMode(depthMode) || depthMode == DEPTH_MODE_BILLBOARD;
    if (hasWorldPosition)
    {
        uint zBits = uint(clamp(worldZ, 0, int(DEPTH_Z_MASK)));
        uint surfaceData = 255u;
        if (isTerrainMode(depthMode))
        {
            // A cliff lid may still join the next terrain tile continuously on another edge.
            // Store its exact surface equation so the resolve can recognise that topology.
            // baseZ is always a multiple of 8; store it shifted so both fit 16 bits.
            surfaceData = (uint(terrainBaseZ) >> 3) | (uint(terrainSlope & 0x1F) << 8);
        }
        else if (depthMode == DEPTH_MODE_WALL)
        {
            // Bit 0 is the lighting-facing flag. Bit 1 distinguishes a terrain side from a
            // placed wall, so the resolve can clip rasterised wall endpoints against the
            // exact adjoining terrain edge.
            surfaceData = (wallFacesSun ? WALL_FACES_SUN : 0u) | (wallIsTerrainSide ? WALL_IS_TERRAIN_SIDE : 0u)
                | (wallRunsAlongY ? WALL_RUNS_ALONG_Y : 0u);
        }
        else if (depthMode == DEPTH_MODE_ROOF)
        {
            surfaceData = roofSurface;
        }
        oWorldDepth = uvec4(uint(worldXY.x), uint(worldXY.y), (depthMode << DEPTH_Z_BITS) | zBits, surfaceData);
    }
    else
    {
        // A depth-only command marks what it covers as lying under translucent UI. It is drawn
        // with an OR logic op and colour masked off, so only this bit lands in the record.
        oWorldDepth = (fFlags & FLAG_WORLD_DEPTH_ONLY) != 0 ? uvec4(0u, 0u, 0u, UNDER_TRANSLUCENT_UI) : uvec4(0u);
    }

    if (uShadowMapPass)
    {
        // The sun keeps the highest surface on each ray, with an upright surface winning a tie
        // against the floor it stands on. Rank over the full stored Z range: world Z reaches
        // about 2040, so the old 1024 ceiling made everything above it tie and left the winner
        // to paint order. Half steps over 8192 is 16384 levels, well inside a 16-bit depth.
        float rank = 0.0;
        if (hasWorldPosition)
        {
            bool isFloor = depthMode == DEPTH_MODE_FLAT || depthMode == DEPTH_MODE_ROOF || isTerrainMode(depthMode);
            rank = float(worldZ) + (isFloor ? 0.0 : 0.5);
        }
        gl_FragDepth = 1.0 - clamp(rank / float(DEPTH_Z_MASK + 1u), 0.0, 1.0);
    }
    else
    {
        gl_FragDepth = gl_FragCoord.z;
    }
}
