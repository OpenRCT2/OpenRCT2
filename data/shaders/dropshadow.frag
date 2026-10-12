#version 330 core

// Dual isometric shadow map.
// The camera G-buffer holds a real world XYZ for every pixel that has one. Project that
// position into the sun view (yaw +90 degrees) and compare against the surface the sun pass
// recorded there.

#include "worlddepth.glsl"

uniform usampler2D uCameraDepthTex;
uniform usampler2D uSunDepthTex;
uniform vec2 uSunViewPos;  // top-left of the sun capture, in view-space units (zoom-independent)
uniform vec2 uSunViewSize; // sun FBO size, in TEXELS
uniform int uSunRotation;
uniform int uCameraRotation;
// The sun normally stands 90 degrees clockwise of the camera, lighting the right-hand wall faces.
// A lightning flash can light the scene from the other side, where the left-hand faces are lit.
uniform bool uSunFromLeft;
// View-space units per sun-buffer texel and per camera pixel. Together they give the size of
// one camera pixel's footprint in the sun buffer - see the sampling note in main().
uniform float uSunZoomScale;
uniform float uCameraZoomScale;
uniform float uShadowStrength;

in vec2 fTextureCoordinate;

out float oShadow;

int terrainCornerHeight(uvec4 surface, int corner)
{
    int baseZ = int(surface.a & 0xFFu) << 3;
    int slope = int((surface.a >> 8) & 0x1Fu);
    int raised = slope & 0x0F;
    int relativeHeight = 0;
    if ((slope & 0x10) == 0)
    {
        relativeHeight = ((raised & (1 << corner)) != 0) ? 1 : 0;
    }
    else
    {
        // A diagonal corner-down slope has one corner at 0, its opposite at 2 and the two
        // adjacent corners at 1. The low four slope bits identify the one missing corner.
        int missing = 0;
        for (int i = 0; i < 4; i++)
        {
            if ((raised & (1 << i)) == 0)
                missing = i;
        }
        relativeHeight = (corner == missing) ? 0 : ((corner == ((missing + 2) & 3)) ? 2 : 1);
    }
    return baseZ + relativeHeight * 16;
}

bool terrainSurfacesMeet(uvec4 receiver, uvec4 caster)
{
    if (!isTerrainMode(depthMode(receiver)) || !isTerrainMode(depthMode(caster)))
        return false;

    ivec2 receiverTile = ivec2(int(receiver.r), int(receiver.g)) & ~31;
    ivec2 casterTile = ivec2(int(caster.r), int(caster.g)) & ~31;
    ivec2 delta = casterTile - receiverTile;
    if (abs(delta.x) > 32 || abs(delta.y) > 32 || all(equal(delta, ivec2(0))))
        return false;

    // Raw terrain slope corners are N, E, S, W. Edge neighbours must match at both
    // endpoints; diagonal neighbours meet at one corner.
    if (all(equal(delta, ivec2(32, 0))))
        return terrainCornerHeight(receiver, 1) == terrainCornerHeight(caster, 2)
            && terrainCornerHeight(receiver, 0) == terrainCornerHeight(caster, 3);
    if (all(equal(delta, ivec2(-32, 0))))
        return terrainCornerHeight(receiver, 2) == terrainCornerHeight(caster, 1)
            && terrainCornerHeight(receiver, 3) == terrainCornerHeight(caster, 0);
    if (all(equal(delta, ivec2(0, 32))))
        return terrainCornerHeight(receiver, 3) == terrainCornerHeight(caster, 2)
            && terrainCornerHeight(receiver, 0) == terrainCornerHeight(caster, 1);
    if (all(equal(delta, ivec2(0, -32))))
        return terrainCornerHeight(receiver, 2) == terrainCornerHeight(caster, 3)
            && terrainCornerHeight(receiver, 1) == terrainCornerHeight(caster, 0);
    if (all(equal(delta, ivec2(32, 32))))
        return terrainCornerHeight(receiver, 0) == terrainCornerHeight(caster, 2);
    if (all(equal(delta, ivec2(32, -32))))
        return terrainCornerHeight(receiver, 1) == terrainCornerHeight(caster, 3);
    if (all(equal(delta, ivec2(-32, 32))))
        return terrainCornerHeight(receiver, 3) == terrainCornerHeight(caster, 1);
    if (all(equal(delta, ivec2(-32, -32))))
        return terrainCornerHeight(receiver, 2) == terrainCornerHeight(caster, 0);
    return false;
}

int interpolateTerrainEdge(int heightA, int heightB, int t)
{
    // TileElementHeight samples the 32-unit edge at integer positions 0..31. Keeping the
    // denominator at 32 reproduces that half-open raster: the conceptual far corner is at
    // t=32 and the last texel immediately before it is t=31.
    return (heightA * (32 - t) + heightB * t) / 32;
}

bool terrainWallCoveredByReceiver(uvec4 receiver, uvec4 wall)
{
    if (!isTerrainMode(depthMode(receiver)) || (wall.a & WALL_IS_TERRAIN_SIDE) == 0u)
        return false;

    ivec2 receiverTile = ivec2(int(receiver.r), int(receiver.g)) & ~31;
    ivec2 wallTile = ivec2(int(wall.r), int(wall.g)) & ~31;
    ivec2 delta = wallTile - receiverTile;
    if (abs(delta.x) > 32 || abs(delta.y) > 32 || all(equal(delta, ivec2(0))))
        return false;

    ivec2 wallLocal = ivec2(int(wall.r), int(wall.g)) - wallTile;
    int boundaryHeight = -1;

    // Raw corners: N=0, E=1, S=2, W=3. Along each receiver edge, t runs from
    // its first named endpoint toward the second in increasing world X or Y.
    if (all(equal(delta, ivec2(32, 0))) && wallLocal.x == 0)
        boundaryHeight = interpolateTerrainEdge(
            terrainCornerHeight(receiver, 1), terrainCornerHeight(receiver, 0), wallLocal.y);
    else if (all(equal(delta, ivec2(-32, 0))) && wallLocal.x == 31)
        boundaryHeight = interpolateTerrainEdge(
            terrainCornerHeight(receiver, 2), terrainCornerHeight(receiver, 3), wallLocal.y);
    else if (all(equal(delta, ivec2(0, 32))) && wallLocal.y == 0)
        boundaryHeight = interpolateTerrainEdge(
            terrainCornerHeight(receiver, 3), terrainCornerHeight(receiver, 0), wallLocal.x);
    else if (all(equal(delta, ivec2(0, -32))) && wallLocal.y == 31)
        boundaryHeight = interpolateTerrainEdge(
            terrainCornerHeight(receiver, 2), terrainCornerHeight(receiver, 1), wallLocal.x);
    else if (all(equal(delta, ivec2(32, 32))) && wallLocal.x == 0 && wallLocal.y == 0)
        boundaryHeight = terrainCornerHeight(receiver, 0);
    else if (all(equal(delta, ivec2(32, -32))) && wallLocal.x == 0 && wallLocal.y == 31)
        boundaryHeight = terrainCornerHeight(receiver, 1);
    else if (all(equal(delta, ivec2(-32, 32))) && wallLocal.x == 31 && wallLocal.y == 0)
        boundaryHeight = terrainCornerHeight(receiver, 3);
    else if (all(equal(delta, ivec2(-32, -32))) && wallLocal.x == 31 && wallLocal.y == 31)
        boundaryHeight = terrainCornerHeight(receiver, 2);

    return boundaryHeight >= 0 && depthWorldZ(wall) <= boundaryHeight;
}

// Cosine between a surface's normal and the direction toward the sun, below which it counts as
// grazing. Flat ground is 0.58 and a one-step slope turned away from the sun 0.26 - lit, and it
// takes shadow. A raised corner pointing at the sun is 0, running exactly along the rays, and
// steeper faces are below that: a shadow lands on those patchily and the art draws them dark
// already, so they take no cast shadow.
const float GRAZING_COSINE = 0.1;

// Roofs are held to a looser limit. Their pieces are small and their slopes meet at ridges, so a
// back slope that terrain would call grazing reads as a lit gap through a shadow crossing the roof.
const float ROOF_GRAZING_COSINE = 0.05;

float cosineToSun(vec2 gradient)
{
    // Rotate the normal's horizontal part into the sun's view, where the direction toward the
    // viewer - the sun - is (1,1,1): screen position stays put along it.
    ivec2 r = rotateCoords(ivec2(round(-gradient * 1024.0)), uSunRotation);
    vec3 normal = vec3(vec2(r) / 1024.0, 1.0);
    return dot(normal, vec3(1.0)) / (length(normal) * sqrt(3.0));
}

bool surfaceFacesAwayFromSun(vec2 gradient, float limit)
{
    return cosineToSun(gradient) < limit;
}

/** The slope of the terrain triangle this point lies on. Per triangle: every slope type is two
    triangles, and a raised-corner tile is half flat. Central differences over the solver's own
    height function, kept a unit inside the tile; its half-unit truncation steps average out. */
vec2 terrainGradient(uvec4 surface)
{
    ivec2 position = ivec2(int(surface.r), int(surface.g));
    ivec2 tile = position & ~31;
    ivec2 p = tile + clamp(position - tile, ivec2(1), ivec2(30));
    int baseZ = int(surface.a & 0xFFu) << 3;
    int slope = int((surface.a >> 8) & 0x1Fu);
    return vec2(
               float(tileElementHeight(p + ivec2(1, 0), baseZ, slope) - tileElementHeight(p - ivec2(1, 0), baseZ, slope)),
               float(tileElementHeight(p + ivec2(0, 1), baseZ, slope) - tileElementHeight(p - ivec2(0, 1), baseZ, slope)))
        * 0.5;
}

bool terrainFacesAwayFromSun(uvec4 surface)
{
    return surfaceFacesAwayFromSun(terrainGradient(surface), GRAZING_COSINE);
}

// The terrain art has its lighting baked in, from the same side as this sun: a slope turned one
// step away is drawn a shade darker than flat ground, a grazing one two shades, and a slope facing
// the sun a shade lighter. Modelled as ambient plus direct light on the slope's cosine to the sun,
// fitted to those shades (one shade is about 0.85 of the brightness; flat ground is cosine 0.58),
// relative to flat ground.
float bakedTerrainBrightness(float cosine)
{
    return 0.728 + 0.469 * max(cosine, 0.0);
}

// A shadow takes away the direct light and leaves the ambient, so everything in shadow should end
// as dark as shadowed flat ground does, whatever its tilt. Applying the full strength to a slope
// the art had already darkened took it darker than that; this divides the baked shading back out.
float shadowedBrightness(uvec4 receiver, uint mode)
{
    float flatShadowed = 1.0 - uShadowStrength;
    // A lightning flash comes from a side the art was not lit from.
    if (!isTerrainMode(mode) || uSunFromLeft)
        return flatShadowed;
    return min(1.0, flatShadowed / bakedTerrainBrightness(cosineToSun(terrainGradient(receiver))));
}

bool texelOccludes(uvec4 sun, uvec4 receiver)
{
    uint sunMode = depthMode(sun);
    // Nothing was drawn on this ray, so nothing stands between this point and the sun.
    if (sunMode == DEPTH_MODE_NONE)
        return false;

    // A sun-facing wall takes its light from in front, so nothing lying in its own plane can
    // stand between it and the sun. Without this, the face of the next tile along a cliff -
    // a unit higher after reconstruction - shades a 1px seam at every tile boundary, and the
    // sliver of a back-facing cliff's top edge that peeks over a rim.
    if (wallsCoplanar(receiver, sun))
        return false;

    ivec2 receiverTile = ivec2(int(receiver.r), int(receiver.g)) & ~31;
    ivec2 sunXY = ivec2(int(sun.r), int(sun.g));
    // Terrain may not shadow its own tile: a terrain side IS the vertical face of the tile it
    // belongs to, and letting it shade that tile's top is self-shadowing. A placed wall is
    // exempt. It stands on a tile boundary, so depending on which edge it occupies its shadow
    // falls back across its own tile, and the height test below already leaves its base lit.
    bool sunIsTerrainSide = sunMode == DEPTH_MODE_WALL && (sun.a & WALL_IS_TERRAIN_SIDE) != 0u;
    if ((sunIsTerrainSide || isTerrainMode(sunMode)) && (sunXY & ~31) == receiverTile)
        return false;

    uint receiverMode = depthMode(receiver);

    // A roof piece is convex, so its top cannot shade itself. The slope turned away from the
    // sun is dark in the art already and is skipped as grazing before it gets here.
    if (receiverMode == DEPTH_MODE_ROOF && (sunXY & ~31) == receiverTile
        && (sunMode == DEPTH_MODE_ROOF || sunMode == DEPTH_MODE_BILLBOARD))
        return false;

    // A tile classified as a cliff lid may still meet this receiver perfectly along another
    // edge. That continuous join is the same surface, not an occluder. Plain terrain is held
    // to the same test, since it now casts as well.
    if (isTerrainMode(sunMode) && terrainSurfacesMeet(receiver, sun))
        return false;

    // A sloped terrain-wall sprite can retain a one-texel endpoint where its triangular
    // face tapers into an adjoining terrain corner. It is below that receiver surface, so
    // it is buried geometry rather than an occluder. Placed walls are deliberately excluded.
    if (sunMode == DEPTH_MODE_WALL && terrainWallCoveredByReceiver(receiver, sun))
        return false;

    // A roof stores its true position, so it can be held to geometry: whatever shades a point lies
    // toward the sun from it. The footprint takes in a texel either side of the exact ray, and
    // where a roof meets the ground that let the roof just across a tile edge - on the side away
    // from the sun - speck the ground beside it.
    if (sunMode == DEPTH_MODE_ROOF && isTerrainMode(receiverMode))
    {
        ivec2 towardSun = rotateCoords(ivec2(1, 1), (4 - uSunRotation) & 3);
        ivec2 offset = sunXY - ivec2(int(receiver.r), int(receiver.g));
        if (offset.x * towardSun.x + offset.y * towardSun.y < 0)
            return false;
    }

    // A wall receiving here faces the sun, so whatever shades it stands on its sun side. A wall is a
    // single texel thick in the sun's buffer and does not hide what is behind it, so without this
    // a path deck or a tree behind a castle wall, standing higher than the face, shaded the face.
    // Measured from the tile edge the wall stands on: its stored position is clamped a unit or two
    // inside the owning tile, where a path on that same tile would read as in front of it.
    if (receiverMode == DEPTH_MODE_WALL)
    {
        ivec2 towardSun = rotateCoords(ivec2(1, 1), (4 - uSunRotation) & 3);
        bool alongY = (receiver.a & WALL_RUNS_ALONG_Y) != 0u;
        int position = alongY ? int(receiver.r) : int(receiver.g);
        int plane = (position & ~31) + ((position & 31) >= 16 ? 32 : 0);
        int casterPosition = alongY ? sunXY.x : sunXY.y;
        int toward = alongY ? towardSun.x : towardSun.y;
        // The caster's tile edge counts as in front: something standing on it is not behind the wall.
        if ((casterPosition - plane) * toward < (toward > 0 ? 0 : -1))
            return false;
    }

    int sunWorldZ = depthWorldZ(sun);
    int worldZ = depthWorldZ(receiver);

    // Points that project to the same sun pixel lie on one sun ray, and along that ray world
    // Z increases toward the sun-viewer (larger x+y is nearer the viewer in this isometric
    // projection, and z rises with it). So the sun's nearest-surface sample occludes us
    // exactly when it sits higher than we do.
    //
    // NB: do NOT also treat "sun saw a different world XY" as occlusion. On a slope the sun
    // legitimately sees the receiver's own surface at a noticeably different XY (the ray
    // grazes the slope), so that test manufactured shadow across every sloped tile.
    //
    // Plain terrain casts too, or a pit's shadow starts a tile away from the wall throwing it:
    // that wall faces away from the sun and never reaches the sun buffer, so what occludes the
    // floor beside it is ordinary flat ground set back from the rim. The bias keeps that safe.
    // Terrain rises at most one land step per tile (gradient 0.5) while the sun ray gains a
    // full unit of Z per unit travelled, so continuous ground can never truly occlude itself and
    // anything within the margin is reconstruction noise. 32 was swept, not guessed: 16 and 24
    // leave single-pixel acne across slopes. A real cliff's lid is a cap and casts unbiased.
    // A neighbouring tile is the exception. Its join with the receiver is checked exactly first
    // (terrainSurfacesMeet), so one still here does not join it: there is a real step between
    // them, and it casts with the small margin. That matters where a sun ray runs through a tile
    // corner into the diagonal neighbour of a pit floor, past the two cliff lids beside it; with
    // 32 there, a pit shallower than that lost its shadow in tile-shaped steps.
    //
    // Two contacts are level by construction and differ only by that same noise, a unit at
    // most: a path lying on the ground beside the ground it sits on, and a cliff face's top
    // edge beside the terrain it is the face of - which also covers the sliver of a hidden
    // cliff showing over a rim. A raised deck or a real step clears by a land step at least.
    //
    // A roof is reconstructed to a unit in Z like the rest, and long roofs are built from many
    // pieces that meet edge to edge, so what lands on a roof gets the same small margin. A roof
    // onto the ground gets none: a margin there cut the first pixels off its shadow - the corner
    // missing at the foot of every gable. Its contact noise is handled by direction, above.
    int castBias = 0;
    if (sunMode == DEPTH_MODE_TERRAIN)
    {
        ivec2 tileDistance = abs((sunXY >> 5) - (receiverTile >> 5));
        castBias = max(tileDistance.x, tileDistance.y) <= 1 ? 2 : 32;
    }
    else if (receiverMode == DEPTH_MODE_ROOF)
        castBias = 3;

    else if (
        (sunMode == DEPTH_MODE_FLAT && isTerrainMode(receiverMode))
        || (isTerrainMode(sunMode) && receiverMode == DEPTH_MODE_WALL))
        castBias = 2;
    else if (sunMode == DEPTH_MODE_BILLBOARD && isTerrainMode(receiverMode))
        // An upright prop's texel on the very ray through a ground point, at that point's own
        // height, is the same point: the prop's foot on the ground. That is contact, and leaving it
        // lit opened a gap a few pixels wide between every sign or building and its shadow.
        castBias = -1;
    return sunWorldZ > worldZ + castBias;
}

void main()
{
    uvec4 cam = texture(uCameraDepthTex, fTextureCoordinate);
    uint mode = depthMode(cam);
    // A wall face turned away from the sun never catches the light in the first place, and the
    // art already draws it dark. That is a lighting question, not an occlusion one, and
    // attenuating it a second time just muddies the face.
    bool wallFacesSun = ((cam.a & WALL_FACES_SUN) != 0u) != uSunFromLeft;
    if (!isReceiverMode(mode) || (mode == DEPTH_MODE_WALL && !wallFacesSun))
    {
        oShadow = 1.0;
        return;
    }

    // The grazing part of a terrain tile, or a roof slope turned away, takes no cast shadow.
    if ((isTerrainMode(mode) && terrainFacesAwayFromSun(cam))
        || (mode == DEPTH_MODE_ROOF && surfaceFacesAwayFromSun(roofGradient(cam), ROOF_GRAZING_COSINE)))
    {
        oShadow = 1.0;
        return;
    }

    ivec2 worldXY = ivec2(int(cam.r), int(cam.g));
    int worldZ = depthWorldZ(cam);

    // View-space units -> sun buffer texels.
    vec2 sunPixel = (vec2(translate3DTo2D(worldXY, worldZ, uSunRotation)) - uSunViewPos) / uSunZoomScale;
    int sunX = int(floor(sunPixel.x));
    int sunY = int(uSunViewSize.y) - 1 - int(floor(sunPixel.y));

    // One camera pixel's footprint in sun texels. With the sun 90 degrees round, for a
    // receiver at fixed z:
    //     camSx = b - a          sunSx = -(a + b)     = -2*camSy - 2z
    //     camSy = (a + b)/2 - z  sunSy = (b - a)/2 - z =  0.5*camSx - z
    // so camera Y maps onto sun X at 2x and camera X onto sun Y at 0.5x - axis-aligned in sun
    // space, 2*zoom wide by 0.5*zoom tall. The sun buffer is captured one zoom level finer
    // than the camera, so this is 2x1 at zoom 0 and 4x1 at every zoom out.
    int sampleCount = max(1, int(floor(2.0 * uCameraZoomScale / uSunZoomScale + 0.5)));
    int rowCount = max(1, int(floor(0.5 * uCameraZoomScale / uSunZoomScale + 0.5)));
    // Integer reconstruction puts the reference point on alternating sides of the footprint
    // for odd and even camera columns, so pick the matching run of texels. The phase alternates
    // per camera PIXEL, not per view-space unit - at zoom 1+ those differ.
    int cameraX = translate3DTo2D(worldXY, worldZ, uCameraRotation).x;
    int cameraPixelX = int(floor(float(cameraX) / uCameraZoomScale));
    int firstX = sunX - (sampleCount / 2) + (cameraPixelX & 1);
    int firstY = sunY - (rowCount / 2);

    ivec2 sunSize = ivec2(uSunViewSize);
    int occludedCount = 0;
    int validCount = 0;
    for (int j = 0; j < rowCount; j++)
    {
        for (int i = 0; i < sampleCount; i++)
        {
            ivec2 t = ivec2(firstX + i, firstY + j);
            if (any(lessThan(t, ivec2(0))) || any(greaterThanEqual(t, sunSize)))
                continue;
            validCount++;
            uvec4 sun = texelFetch(uSunDepthTex, t, 0);
            // Ordinary ground and cliff faces also have to block the light passing two texels higher.
            // Light that went into the ground - past a pit's rim - still meets it there, on the ground
            // or the cliff top behind; light that only skims it - along a pyramid's ridge, which
            // rises exactly as steeply as the light - clears it, and was shading a line of pixels at
            // the ridge's foot. Cliff tops are exempt: their edge is the shadow's true edge.
            bool skimmable = depthMode(sun) == DEPTH_MODE_TERRAIN
                || (depthMode(sun) == DEPTH_MODE_WALL && (sun.a & WALL_IS_TERRAIN_SIDE) != 0u);
            if (texelOccludes(sun, cam)
                && (!skimmable || t.y + 2 >= sunSize.y || texelOccludes(texelFetch(uSunDepthTex, t + ivec2(0, 2), 0), cam)))
                occludedCount++;
        }
    }

    // A two-texel footprint shadows on either texel; anything larger shadows on a majority.
    // "Any of eight" fattened every caster by up to a footprint and welded neighbouring shadows
    // together; a majority keeps the shadow's true extent. The result stays binary, so no
    // half-shaded pixels appear.
    bool occluded = (sampleCount * rowCount <= 2) ? occludedCount > 0 : (validCount > 0 && occludedCount * 2 >= validCount);
    oShadow = occluded ? shadowedBrightness(cam, mode) : 1.0;
}
