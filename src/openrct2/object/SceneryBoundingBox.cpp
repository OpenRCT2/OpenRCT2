/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SceneryBoundingBox.h"

static constexpr std::array<CoordsXYZ, DefaultSpriteOffsetType::CountOffset> DefaultSpriteOffsets = {
    CoordsXYZ(7, 7, 0),   // quarter tile
    CoordsXYZ(15, 15, 0), // small scenery full tile w/o VOFFSET_CENTRE
    CoordsXYZ(3, 3, 0),   // small scenery halftile/VOFFSET_CENTRE
    CoordsXYZ(1, 1, 0),   // small scenery VOFFSET_CENTER+NO_WALLS
    CoordsXYZ(0, 0, 0),   // large scenery
};

static constexpr SceneryBoundBoxes QuarterTile = {
    BoundBoxXYZ({ 7, 7, 0 }, { 2, 2, 0 }),
    BoundBoxXYZ({ 7, 7, 0 }, { 2, 2, 0 }),
    BoundBoxXYZ({ 7, 7, 0 }, { 2, 2, 0 }),
    BoundBoxXYZ({ 7, 7, 0 }, { 2, 2, 0 }),
};

static constexpr SceneryBoundBoxes HalfTile = {
    BoundBoxXYZ({ 3, 3, 0 }, { 12, 26, 0 }),
    BoundBoxXYZ({ 3, 17, 0 }, { 26, 12, 0 }),
    BoundBoxXYZ({ 17, 3, 0 }, { 12, 26, 0 }),
    BoundBoxXYZ({ 3, 3, 0 }, { 26, 12, 0 }),
};

// LargeSpecial corner/side match sub-fulltile large scenery boundboxes
static constexpr SceneryBoundBoxes FullTileNorthQuadrant = {
    BoundBoxXYZ({ 3, 3, 0 }, { 12, 12, 0 }),
    BoundBoxXYZ({ 3, 17, 0 }, { 12, 12, 0 }),
    BoundBoxXYZ({ 17, 17, 0 }, { 12, 12, 0 }),
    BoundBoxXYZ({ 17, 3, 0 }, { 12, 12, 0 }),
};
static constexpr SceneryBoundBoxes FullTileNortheastSide = {
    BoundBoxXYZ({ 3, 3, 0 }, { 12, 28, 0 }),
    BoundBoxXYZ({ 3, 17, 0 }, { 26, 12, 0 }),
    BoundBoxXYZ({ 17, 3, 0 }, { 12, 26, 0 }),
    BoundBoxXYZ({ 3, 3, 0 }, { 28, 12, 0 }),
};
static constexpr SceneryBoundBoxes FullTileEastQuadrant = {
    BoundBoxXYZ({ 3, 17, 0 }, { 12, 12, 0 }),
    BoundBoxXYZ({ 17, 17, 0 }, { 12, 12, 0 }),
    BoundBoxXYZ({ 17, 3, 0 }, { 12, 12, 0 }),
    BoundBoxXYZ({ 3, 3, 0 }, { 12, 12, 0 }),
};
static constexpr SceneryBoundBoxes FullTileSoutheastSide = {
    BoundBoxXYZ({ 3, 17, 0 }, { 26, 12, 0 }),
    BoundBoxXYZ({ 17, 3, 0 }, { 12, 26, 0 }),
    BoundBoxXYZ({ 3, 3, 0 }, { 28, 12, 0 }),
    BoundBoxXYZ({ 3, 3, 0 }, { 12, 28, 0 }),
};
static constexpr SceneryBoundBoxes FullTileSouthQuadrant = {
    BoundBoxXYZ({ 17, 17, 0 }, { 12, 12, 0 }),
    BoundBoxXYZ({ 17, 3, 0 }, { 12, 12, 0 }),
    BoundBoxXYZ({ 3, 3, 0 }, { 12, 12, 0 }),
    BoundBoxXYZ({ 3, 17, 0 }, { 12, 12, 0 }),
};
static constexpr SceneryBoundBoxes FullTileSouthwestSide = {
    BoundBoxXYZ({ 17, 3, 0 }, { 12, 26, 0 }),
    BoundBoxXYZ({ 3, 3, 0 }, { 28, 12, 0 }),
    BoundBoxXYZ({ 3, 3, 0 }, { 12, 28, 0 }),
    BoundBoxXYZ({ 3, 17, 0 }, { 26, 12, 0 }),
};
static constexpr SceneryBoundBoxes FullTileWestQuadrant = {
    BoundBoxXYZ({ 17, 3, 0 }, { 12, 12, 0 }),
    BoundBoxXYZ({ 3, 3, 0 }, { 12, 12, 0 }),
    BoundBoxXYZ({ 3, 17, 0 }, { 12, 12, 0 }),
    BoundBoxXYZ({ 17, 17, 0 }, { 12, 12, 0 }),
};
static constexpr SceneryBoundBoxes FullTileNorthwestSide = {
    BoundBoxXYZ({ 3, 3, 0 }, { 28, 12, 0 }),
    BoundBoxXYZ({ 3, 3, 0 }, { 12, 28, 0 }),
    BoundBoxXYZ({ 3, 17, 0 }, { 26, 12, 0 }),
    BoundBoxXYZ({ 17, 3, 0 }, { 12, 26, 0 }),
};

// LargeSpecialCenter matches large scenery with allowed walls and small scenery with SMALL_SCENERY_FLAG_VOFFSET_CENTRE
static constexpr SceneryBoundBoxes FullTile = {
    BoundBoxXYZ({ 3, 3, 0 }, { 26, 26, 0 }),
    BoundBoxXYZ({ 3, 3, 0 }, { 26, 26, 0 }),
    BoundBoxXYZ({ 3, 3, 0 }, { 26, 26, 0 }),
    BoundBoxXYZ({ 3, 3, 0 }, { 26, 26, 0 }),
};

// Large matches large scenery and small scenery that do not allow walls.
static constexpr SceneryBoundBoxes FullTileLarge = {
    BoundBoxXYZ({ 1, 1, 0 }, { 30, 30, 0 }),
    BoundBoxXYZ({ 1, 1, 0 }, { 30, 30, 0 }),
    BoundBoxXYZ({ 1, 1, 0 }, { 30, 30, 0 }),
    BoundBoxXYZ({ 1, 1, 0 }, { 30, 30, 0 }),
};

// Small Scenery without VOFFSET_CENTRE flag set
static constexpr SceneryBoundBoxes FullTileThin = {
    BoundBoxXYZ({ 15, 15, 0 }, { 2, 2, 0 }),
    BoundBoxXYZ({ 15, 15, 0 }, { 2, 2, 0 }),
    BoundBoxXYZ({ 15, 15, 0 }, { 2, 2, 0 }),
    BoundBoxXYZ({ 15, 15, 0 }, { 2, 2, 0 }),
};

static constexpr std::array<SceneryBoundBoxes, DefaultBoundingBoxType::CountBox> boundBoxes = {
    QuarterTile,
    HalfTile,
    FullTileNorthQuadrant,
    FullTileNortheastSide,
    FullTileEastQuadrant,
    FullTileSoutheastSide,
    FullTileSouthQuadrant,
    FullTileSouthwestSide,
    FullTileWestQuadrant,
    FullTileNorthwestSide,
    FullTile,
    FullTileLarge,
    FullTileThin,
};

#pragma endregion

static const EnumMap<DefaultBoundingBoxType> BBoxTypeLookup = {
    { "quarterTile", DefaultBoundingBoxType::QuarterTileBox },
    { "halfTile", DefaultBoundingBoxType::HalfTileBox },
    { "cornerNorth", DefaultBoundingBoxType::FullTileNorthQuadrantBox },
    { "sideNortheast", DefaultBoundingBoxType::FullTileNortheastSideBox },
    { "cornerEast", DefaultBoundingBoxType::FullTileEastQuadrantBox },
    { "sideSoutheast", DefaultBoundingBoxType::FullTileSoutheastSideBox },
    { "cornerSouth", DefaultBoundingBoxType::FullTileSouthQuadrantBox },
    { "sideSouthwest", DefaultBoundingBoxType::FullTileSouthwestSideBox },
    { "cornerEast", DefaultBoundingBoxType::FullTileWestQuadrantBox },
    { "sideNorthwest", DefaultBoundingBoxType::FullTileNorthwestSideBox },
    { "fullTile", DefaultBoundingBoxType::FullTileBox },
    { "fullTileLarge", DefaultBoundingBoxType::FullTileLargeBox },
    { "fullTileThin", DefaultBoundingBoxType::FullTileThinBox }
};

static DefaultBoundingBoxType GetBoundingBoxTypeFromString(const std::string& s)
{
    auto result = BBoxTypeLookup.find(s);
    return (result != BBoxTypeLookup.end()) ? result->second : DefaultBoundingBoxType::FullTileBox;
}

SceneryBoundBoxes GetDefaultSceneryBoundBoxes(DefaultBoundingBoxType type)
{
    if (type >= DefaultBoundingBoxType::CountBox)
        return boundBoxes[DefaultBoundingBoxType::FullTileBox];
    return boundBoxes[type];
}

static BoundBoxXYZ ReadBoundBox(json_t& box)
{
    auto jOffset = box["offset"];
    auto jLength = box["size"];

    Guard::Assert(
        jOffset.is_array() && jLength.is_array() && jOffset.size() >= 3 && jLength.size() >= 3,
        "ReadBoundBox expects arrays 'offset' and 'size' with 3 elements");
    BoundBoxXYZ boundBox = {
        {
            Json::GetNumber<int32_t>(jOffset[0], 0),
            Json::GetNumber<int32_t>(jOffset[1], 0),
            Json::GetNumber<int32_t>(jOffset[2], 0),
        },
        {
            Json::GetNumber<int32_t>(jLength[0], 0),
            Json::GetNumber<int32_t>(jLength[1], 0),
            Json::GetNumber<int32_t>(jLength[2], 0),
        },
    };
    return boundBox;
}

// Rotates a BoundBoxXYZ clockwise 90 degrees around the vertical axis.
static BoundBoxXYZ RotateBoundBox(BoundBoxXYZ box, CoordsXY rotationCenter)
{
    CoordsXYZ rotatedLength = { box.length.y, box.length.x, box.length.z };
    // equations are performed in double scale to avoid decimals
    CoordsXY relativeCentroid = box.offset * 2 + box.length - rotationCenter * 2;
    CoordsXY rotatedCentroid = { relativeCentroid.y, -relativeCentroid.x };
    CoordsXY newCorner = (rotatedCentroid - rotatedLength) / 2 + rotationCenter;
    return { { newCorner.x, newCorner.y, box.offset.z }, rotatedLength };
}

SceneryBoundBoxes ReadBoundBoxes(json_t& jBBox, int32_t defaultHeight, bool fullTile)
{
    SceneryBoundBoxes boxes;
    if (jBBox.is_array())
    {
        Guard::Assert(jBBox.size() >= 4, "boundBox arrays require four elements, one for each view angle");
        // array of four bboxes
        for (uint8_t i = 0; i < NumOrthogonalDirections; i++)
            boxes[i] = ReadBoundBox(jBBox[i]);
    }
    else if (jBBox.is_object())
    {
        // single box, rotated around (16, 16) if fulltile or (8,8) if quarter tile
        CoordsXY rotationCenter = { 8, 8 };
        if (fullTile)
        {
            rotationCenter = { 16, 16 };
        }
        auto bBox = ReadBoundBox(jBBox);
        boxes[0] = bBox;
        boxes[1] = RotateBoundBox(bBox, rotationCenter);
        boxes[2] = RotateBoundBox(boxes[1], rotationCenter);
        boxes[3] = RotateBoundBox(boxes[2], rotationCenter);
    }
    else
    {
        Guard::Assert(
            jBBox.is_string(),
            "boundBox must be an array of four boundBox objects, a single boundBox object, or a string matching the "
            "DefaultBoundingBoxType enum.");
        boxes = GetDefaultSceneryBoundBoxes(GetBoundingBoxTypeFromString(Json::GetString(jBBox)));
        for (uint8_t i = 0; i < NumOrthogonalDirections; i++)
            boxes[i].length.z = defaultHeight;
    }
    return boxes;
}

static const EnumMap<DefaultSpriteOffsetType> SpriteOffsetLookup = {
    { "quarterTile", DefaultSpriteOffsetType::QuarterTileOffset },
    { "fullTileThin", DefaultSpriteOffsetType::FullTileThinOffset },
    { "halfTile", DefaultSpriteOffsetType::FullTileOffset },
    { "fullTile", DefaultSpriteOffsetType::FullTileOffset },
    { "fullTileLarge", DefaultSpriteOffsetType::FullTileLargeOffset },
    { "largeScenery", DefaultSpriteOffsetType::LargeSceneryOffset },
};

static DefaultSpriteOffsetType GetSpriteOffsetTypeFromString(const std::string& s)
{
    auto result = SpriteOffsetLookup.find(s);
    return (result != SpriteOffsetLookup.end()) ? result->second : DefaultSpriteOffsetType::LargeSceneryOffset;
}

CoordsXYZ GetDefaultSpriteOffset(DefaultSpriteOffsetType type)
{
    if (type >= DefaultSpriteOffsetType::CountOffset)
        return DefaultSpriteOffsets[DefaultSpriteOffsetType::LargeSceneryOffset];
    return DefaultSpriteOffsets[type];
}

CoordsXYZ ReadSpriteOffset(json_t& jCoords)
{
    if (jCoords.is_string())
    {
        return DefaultSpriteOffsets[GetSpriteOffsetTypeFromString(Json::GetString(jCoords))];
    }
    Guard::Assert(jCoords.is_array() && jCoords.size() >= 3, "spriteOffsetCoordinates must be an array with three elements");
    CoordsXYZ coordinates = {
        Json::GetNumber<int32_t>(jCoords[0], 0),
        Json::GetNumber<int32_t>(jCoords[1], 0),
        Json::GetNumber<int32_t>(jCoords[2], 0),
    };
    return coordinates;
}
