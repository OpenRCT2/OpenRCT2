/*****************************************************************************
 * Copyright (c) 2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "PluginMapGen.h"

    #include "../../Context.h"
    #include "../../Diagnostic.h"
    #include "../../GameState.h"
    #include "../../object/ObjectEntryManager.h"
    #include "../../object/ObjectManager.h"
    #include "../../object/SmallSceneryEntry.h"
    #include "../../scripting/ScriptEngine.h"
    #include "../../world/Scenery.h"
    #include "../../world/tile_element/SmallSceneryElement.h"
    #include "../tile_element/SurfaceElement.h"
    #include "MapGen.h"
    #include "MapHelpers.h"
    #include "SurfaceSelection.h"
    #include "dukglue/dukvalue.h"

namespace OpenRCT2::World::MapGenerator
{

    template<typename T>
    static ObjectEntryIndex lookUpTextureId(DukValue& textureId, ObjectEntryIndex fallback)
    {
        if (textureId.type() != DukValue::NUMBER)
        {
            return fallback;
        }

        auto idx = static_cast<ObjectEntryIndex>(textureId.as_uint());
        auto maybeObject = GetContext()->GetObjectManager().GetLoadedObject<T>(idx);
        return maybeObject == nullptr ? fallback : idx;
    }

    static colour_t lookupColour(DukValue& colourId, colour_t fallback)
    {
        if (colourId.type() != DukValue::NUMBER)
        {
            return fallback;
        }

        auto idx = static_cast<colour_t>(colourId.as_uint());
        return idx >= COLOUR_COUNT ? fallback : idx;
    }

    static DukValue setup_hook_context(Settings* settings)
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();

        auto duk_settings_wip = Scripting::DukObject(ctx);
        duk_settings_wip.Set("sizeX", settings->mapSize.x);
        duk_settings_wip.Set("sizeY", settings->mapSize.y);
        duk_settings_wip.Set("waterLevel", settings->waterLevel);
        duk_settings_wip.Set("landTexture", settings->landTexture);
        duk_settings_wip.Set("edgeTexture", settings->edgeTexture);
        duk_settings_wip.Set("minHeight", settings->heightmapLow);
        duk_settings_wip.Set("maxHeight", settings->heightmapHigh);
        duk_settings_wip.Set("beaches", settings->beaches);
        duk_settings_wip.Set("trees", settings->trees);
        duk_settings_wip.Set("treeToLandRatio", settings->treeToLandRatio);
        duk_settings_wip.Set("minTreeAltitude", settings->minTreeAltitude);
        duk_settings_wip.Set("maxTreeAltitude", settings->maxTreeAltitude);
        duk_settings_wip.Set("smoothTileEdges", settings->smoothTileEdges);
        auto duk_settings = duk_settings_wip.Take();

        duk_push_array(ctx);
        auto duk_map = DukValue::take_from_stack(ctx);

        auto duk_generate_ctx_wip = Scripting::DukObject(ctx);
        duk_generate_ctx_wip.Set("settings", duk_settings);
        duk_generate_ctx_wip.Set("tiles", duk_map);
        return duk_generate_ctx_wip.Take();
    }

    static void placeScenery(TileCoordsXY loc, uint8_t quadrant, DukValue scenery, bool asFullTile)
    {
        if (scenery["object"].type() != DukValue::NUMBER)
        {
            return;
        }

        auto sceneryIdx = scenery["object"].as_uint();
        auto* sceneryEntry = ObjectManager::GetObjectEntry<SmallSceneryEntry>(sceneryIdx);
        if (sceneryEntry == nullptr)
        {
            return;
        }

        auto quadAdjustedLoc = loc.ToCoordsXY();
        uint8_t occupiedQuadrants;
        if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE))
        {
            if (!asFullTile)
            {
                return;
            }
            quadAdjustedLoc = quadAdjustedLoc.ToTileCentre();
            occupiedQuadrants = 0b1111;
        }
        else
        {
            quadAdjustedLoc.x += SceneryQuadrantOffsets[quadrant & 3].x;
            quadAdjustedLoc.y += SceneryQuadrantOffsets[quadrant & 3].y;
            occupiedQuadrants = 1 << quadrant;
        }

        int32_t surfaceZ = TileElementHeight(quadAdjustedLoc);

        auto* sceneryElement = TileElementInsert<SmallSceneryElement>({ loc.ToCoordsXY(), surfaceZ }, occupiedQuadrants);
        Guard::Assert(sceneryElement != nullptr);

        auto primaryColourDuk = scenery["primaryColour"];
        auto secondaryColourDuk = scenery["secondaryColour"];
        auto tertiaryColourDuk = scenery["tertiaryColour"];

        sceneryElement->SetClearanceZ(surfaceZ + sceneryEntry->height);
        sceneryElement->SetSceneryQuadrant(quadrant);
        sceneryElement->SetDirection(std::clamp<uint8_t>(Scripting::AsOrDefault(scenery["direction"], 0u), 0u, 3u));
        sceneryElement->SetEntryIndex(sceneryIdx);
        sceneryElement->SetAge(0);
        sceneryElement->SetPrimaryColour(lookupColour(primaryColourDuk, COLOUR_YELLOW));
        sceneryElement->SetSecondaryColour(lookupColour(secondaryColourDuk, COLOUR_BLACK));
        sceneryElement->SetTertiaryColour(lookupColour(tertiaryColourDuk, COLOUR_BLACK));
    }

    void generateViaPlugin(Settings* settings)
    {
        resetSurfaces(settings);

        auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
        if (!hookEngine.HasSubscriptions(Scripting::HookType::editorLandscapeGenerate))
        {
            LOG_ERROR("no editor.landscape.generate hooks");
            return;
        }

        auto duk_generate_ctx = setup_hook_context(settings);
        hookEngine.Call(Scripting::HookType::editorLandscapeGenerate, duk_generate_ctx, true);

        if (!duk_generate_ctx["tiles"].is_array())
        {
            LOG_ERROR("context.tiles not an array");
            return;
        }

        uint32_t fallbackLandTexture = generateSurfaceTextureId(settings);
        uint32_t fallbackEdgeTexture = generateEdgeTextureId(settings, fallbackLandTexture);

        auto tiles = duk_generate_ctx["tiles"].as_array();
        for (auto y = 1; y < settings->mapSize.y - 1; y++)
        {
            for (auto x = 1; x < settings->mapSize.x - 1; x++)
            {
                uint32_t idx = y * settings->mapSize.x + x;
                if (idx >= tiles.size())
                {
                    LOG_WARNING("idx %d out of range %d", idx, tiles.size());
                }

                auto tile = tiles[idx];

                auto surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });
                if (surfaceElement == nullptr)
                {
                    LOG_WARNING("idx %d no surface element", idx);
                    continue;
                }

                auto adjustedHeight = std::round(Scripting::AsOrDefault(tile["surfaceLevel"], 2.0f) * 0.5f) * 2.0f;
                auto baseHeight = std::clamp<uint8_t>(adjustedHeight, 2u, 254u);
                auto waterHeight = std::clamp<uint8_t>(Scripting::AsOrDefault(tile["waterLevel"], 0u), 0u, 254u);

                auto landTextureDuk = tile["landTexture"];
                auto edgeTextureDuk = tile["edgeTexture"];
                auto landTexture = lookUpTextureId<TerrainSurfaceObject>(landTextureDuk, fallbackLandTexture);
                auto edgeTexture = lookUpTextureId<TerrainEdgeObject>(edgeTextureDuk, fallbackEdgeTexture);

                surfaceElement->BaseHeight = baseHeight;
                surfaceElement->ClearanceHeight = baseHeight;
                surfaceElement->SetSurfaceObjectIndex(landTexture);
                surfaceElement->SetEdgeObjectIndex(edgeTexture);
                surfaceElement->SetSlope(std::clamp<uint8_t>(Scripting::AsOrDefault(tile["slope"], 0u), 0u, 31u));
                surfaceElement->SetGrassLength(std::clamp<uint8_t>(Scripting::AsOrDefault(tile["grassLength"], 0u), 0u, 6u));

                if (baseHeight < waterHeight)
                {
                    surfaceElement->SetWaterHeight(waterHeight * kCoordsZStep);
                }
            }
        }

        // TODO expose smooth function(s) to plugins instead?
        if (settings->smoothTileEdges)
        {
            smoothMap(settings->mapSize, smoothTileWeak);
        }

        for (auto y = 1; y < settings->mapSize.y - 1; y++)
        {
            for (auto x = 1; x < settings->mapSize.x - 1; x++)
            {
                uint32_t idx = y * settings->mapSize.x + x;
                if (idx >= tiles.size())
                {
                    LOG_WARNING("idx %d out of range %d", idx, tiles.size());
                }

                auto tile = tiles[idx];

                TileCoordsXY coords{ x, y };
                auto surfaceElement = MapGetSurfaceElementAt(coords);
                if (surfaceElement == nullptr)
                {
                    LOG_WARNING("idx %d no surface element", idx);
                    continue;
                }

                auto scenery = tile["scenery"];
                if (scenery.type() == DukValue::UNDEFINED || scenery.type() == DukValue::NULLREF)
                {
                    continue;
                }

                if (scenery.is_array())
                {
                    auto scenery_quads = scenery.as_array();
                    for (uint8_t i = 0u; i < std::min<uint8_t>(4u, static_cast<uint8_t>(scenery_quads.size())); ++i)
                    {
                        if (scenery_quads[i].type() == DukValue::UNDEFINED || scenery_quads[i].type() == DukValue::NULLREF)
                        {
                            continue;
                        }
                        placeScenery(coords, i, scenery_quads[i], false);
                    }
                }
                else
                {
                    placeScenery(coords, 0, scenery, true);
                }
            }
        }
    }
} // namespace OpenRCT2::World::MapGenerator

#endif
