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
    #include "../../actions/scenery/SmallSceneryPlaceAction.h"
    #include "../../object/ObjectEntryManager.h"
    #include "../../object/ObjectManager.h"
    #include "../../object/SmallSceneryEntry.h"
    #include "../../scripting/ScriptEngine.h"
    #include "../../world/Map.h"
    #include "../../world/tile_element/SmallSceneryElement.h"
    #include "../tile_element/SurfaceElement.h"
    #include "MapGen.h"
    #include "MapHelpers.h"
    #include "SurfaceSelection.h"

namespace OpenRCT2::World::MapGenerator
{

    template<typename T>
    static ObjectEntryIndex lookupTextureId(const JSValue textureId, const ObjectEntryIndex fallback)
    {
        if (!JS_IsNumber(textureId))
        {
            return fallback;
        }

        auto ctx = GetContext()->GetScriptEngine().GetContext();
        auto idx = static_cast<ObjectEntryIndex>(Scripting::JSToUint(ctx, textureId));
        auto maybeObject = GetContext()->GetObjectManager().GetLoadedObject<T>(idx);
        return maybeObject == nullptr ? fallback : idx;
    }

    static Drawing::Colour lookupColour(const JSValue colourId, const Drawing::Colour fallback)
    {
        if (!JS_IsNumber(colourId))
        {
            return fallback;
        }

        auto ctx = GetContext()->GetScriptEngine().GetContext();
        auto idx = static_cast<Drawing::Colour>(Scripting::JSToUint(ctx, colourId));
        return Drawing::colourIsValid(idx) ? idx : fallback;
    }

    static JSValue setupHookCtx(const Settings* settings)
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();

        JSValue settingsJs = JS_NewObject(ctx);
        JS_SetPropertyStr(ctx, settingsJs, "sizeX", JS_NewInt32(ctx, settings->mapSize.x));
        JS_SetPropertyStr(ctx, settingsJs, "sizeY", JS_NewInt32(ctx, settings->mapSize.y));
        JS_SetPropertyStr(ctx, settingsJs, "waterLevel", JS_NewInt32(ctx, settings->waterLevel));
        JS_SetPropertyStr(ctx, settingsJs, "landTexture", JS_NewInt32(ctx, settings->landTexture));
        JS_SetPropertyStr(ctx, settingsJs, "edgeTexture", JS_NewInt32(ctx, settings->edgeTexture));
        JS_SetPropertyStr(ctx, settingsJs, "minHeight", JS_NewInt32(ctx, settings->heightmapLow));
        JS_SetPropertyStr(ctx, settingsJs, "maxHeight", JS_NewInt32(ctx, settings->heightmapHigh));
        JS_SetPropertyStr(ctx, settingsJs, "beaches", JS_NewBool(ctx, settings->beaches));
        JS_SetPropertyStr(ctx, settingsJs, "trees", JS_NewBool(ctx, settings->trees));
        JS_SetPropertyStr(ctx, settingsJs, "treeToLandRatio", JS_NewInt32(ctx, settings->treeToLandRatio));
        JS_SetPropertyStr(ctx, settingsJs, "minTreeAltitude", JS_NewInt32(ctx, settings->minTreeAltitude));
        JS_SetPropertyStr(ctx, settingsJs, "maxTreeAltitude", JS_NewInt32(ctx, settings->maxTreeAltitude));
        JS_SetPropertyStr(ctx, settingsJs, "smoothTileEdges", JS_NewBool(ctx, settings->smoothTileEdges));

        JSValue tilesJs = JS_NewArray(ctx);

        JSValue contextJs = JS_NewObject(ctx);
        JS_SetPropertyStr(ctx, contextJs, "settings", settingsJs);
        JS_SetPropertyStr(ctx, contextJs, "tiles", tilesJs);

        return contextJs;
    }

    static void placeScenery(const TileCoordsXY& loc, const uint8_t quadrant, const JSValue scenery)
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();

        JSValue sceneryObject = JS_GetPropertyStr(ctx, scenery, "object");
        if (!JS_IsNumber(sceneryObject))
        {
            return;
        }

        auto sceneryIdx = Scripting::JSToUint(ctx, sceneryObject);
        auto* sceneryEntry = ObjectEntryManager::GetObjectEntry<SmallSceneryEntry>(sceneryIdx);
        if (sceneryEntry == nullptr)
        {
            return;
        }

        auto primaryColourJs = JS_GetPropertyStr(ctx, scenery, "primaryColour");
        auto secondaryColourJs = JS_GetPropertyStr(ctx, scenery, "secondaryColour");
        auto tertiaryColourJs = JS_GetPropertyStr(ctx, scenery, "tertiaryColour");

        auto direction = std::clamp<uint8_t>(Scripting::AsOrDefault(ctx, scenery, "direction", 0u), 0u, 3u);
        auto locXYZD = CoordsXYZD(loc.ToCoordsXY(), 0, direction);

        auto action = GameActions::SmallSceneryPlaceAction(
            locXYZD, quadrant, sceneryIdx, lookupColour(primaryColourJs, Drawing::Colour::yellow),
            lookupColour(secondaryColourJs, Drawing::Colour::black), lookupColour(tertiaryColourJs, Drawing::Colour::black));

        JS_FreeValue(ctx, tertiaryColourJs);
        JS_FreeValue(ctx, secondaryColourJs);
        JS_FreeValue(ctx, primaryColourJs);
        JS_FreeValue(ctx, sceneryObject);

        auto& gameState = getGameState();
        auto& park = gameState.park;

        auto queryResult = action.Query(gameState, park);
        if (queryResult.error != GameActions::Status::ok)
        {
            LOG_WARNING(
                "plugin scenery place query - %s: %s", queryResult.getErrorTitle().c_str(),
                queryResult.getErrorMessage().c_str());
            return;
        }

        auto execResult = action.Execute(gameState, park);
        if (execResult.error != GameActions::Status::ok)
        {
            LOG_WARNING(
                "plugin scenery place exec - %s: %s", execResult.getErrorTitle().c_str(), execResult.getErrorMessage().c_str());
        }
    }

    void generateViaPlugin(Settings* settings)
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();
        resetSurfaces(settings);

        auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
        if (!hookEngine.HasSubscriptions(Scripting::HookType::editorLandscapeGenerate))
        {
            LOG_ERROR("no editor.landscape.generate hooks");
            return;
        }

        auto generateCtxJs = setupHookCtx(settings);

        hookEngine.Call(Scripting::HookType::editorLandscapeGenerate, generateCtxJs, true, true);

        auto tilesJs = JS_GetPropertyStr(ctx, generateCtxJs, "tiles");

        if (!JS_IsArray(tilesJs))
        {
            LOG_ERROR("context.tiles not an array");
            JS_FreeValue(ctx, tilesJs);
            JS_FreeValue(ctx, generateCtxJs);
            return;
        }

        uint32_t fallbackLandTexture = generateSurfaceTextureId(settings);
        uint32_t fallbackEdgeTexture = generateEdgeTextureId(settings, fallbackLandTexture);

        int64_t tilesJsLength;
        JS_GetLength(ctx, tilesJs, &tilesJsLength);

        for (auto y = 1; y < settings->mapSize.y - 1; y++)
        {
            for (auto x = 1; x < settings->mapSize.x - 1; x++)
            {
                uint32_t idx = y * settings->mapSize.x + x;
                if (idx >= tilesJsLength)
                {
                    LOG_WARNING("idx %d out of range %d", idx, tilesJsLength);
                    continue;
                }

                auto surfaceElement = MapGetSurfaceElementAt(TileCoordsXY{ x, y });
                if (surfaceElement == nullptr)
                {
                    LOG_WARNING("idx %d no surface element", idx);
                    continue;
                }

                auto tileJs = JS_GetPropertyInt64(ctx, tilesJs, idx);

                auto adjustedHeight = std::round(Scripting::AsOrDefault(ctx, tileJs, "surfaceLevel", 2.0f) * 0.5f) * 2.0f;
                auto baseHeight = std::clamp<uint8_t>(adjustedHeight, 2u, 254u);
                auto waterHeight = std::clamp<uint8_t>(Scripting::AsOrDefault(ctx, tileJs, "waterLevel", 0u), 0u, 254u);

                auto landTextureJs = JS_GetPropertyStr(ctx, tileJs, "landTexture");
                auto edgeTextureJs = JS_GetPropertyStr(ctx, tileJs, "edgeTexture");
                auto landTexture = lookupTextureId<TerrainSurfaceObject>(landTextureJs, fallbackLandTexture);
                auto edgeTexture = lookupTextureId<TerrainEdgeObject>(edgeTextureJs, fallbackEdgeTexture);

                surfaceElement->baseHeight = baseHeight;
                surfaceElement->clearanceHeight = baseHeight;
                surfaceElement->SetSurfaceObjectIndex(landTexture);
                surfaceElement->SetEdgeObjectIndex(edgeTexture);
                surfaceElement->SetSlope(std::clamp<uint8_t>(Scripting::AsOrDefault(ctx, tileJs, "slope", 0u), 0u, 31u));
                surfaceElement->SetGrassLength(
                    std::clamp<uint8_t>(Scripting::AsOrDefault(ctx, tileJs, "grassLength", 0u), 0u, 6u));

                if (baseHeight < waterHeight)
                {
                    surfaceElement->SetWaterHeight(waterHeight * kCoordsZStep);
                }

                JS_FreeValue(ctx, edgeTextureJs);
                JS_FreeValue(ctx, landTextureJs);
                JS_FreeValue(ctx, tileJs);
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
                uint32_t tileIdx = y * settings->mapSize.x + x;
                if (tileIdx >= tilesJsLength)
                {
                    LOG_WARNING("idx %d out of range %d", tileIdx, tilesJsLength);
                    continue;
                }

                TileCoordsXY coords{ x, y };
                auto surfaceElement = MapGetSurfaceElementAt(coords);
                if (surfaceElement == nullptr)
                {
                    LOG_WARNING("idx %d no surface element", tileIdx);
                    continue;
                }

                auto tileJs = JS_GetPropertyInt64(ctx, tilesJs, tileIdx);
                auto sceneryJs = JS_GetPropertyStr(ctx, tileJs, "scenery");

                if (JS_IsArray(sceneryJs))
                {
                    int64_t sceneryQuadJsLength;
                    JS_GetLength(ctx, sceneryJs, &sceneryQuadJsLength);
                    for (uint8_t quadIdx = 0u; quadIdx < std::min<uint8_t>(4u, static_cast<uint8_t>(sceneryQuadJsLength));
                         ++quadIdx)
                    {
                        auto sceneryQuadJs = JS_GetPropertyInt64(ctx, tilesJs, quadIdx);
                        if (JS_IsObject(sceneryQuadJs))
                        {
                            placeScenery(coords, quadIdx, sceneryQuadJs);
                        }
                        JS_FreeValue(ctx, sceneryQuadJs);
                    }
                }
                else if (JS_IsObject(sceneryJs))
                {
                    placeScenery(coords, 0, sceneryJs);
                }

                JS_FreeValue(ctx, sceneryJs);
                JS_FreeValue(ctx, tileJs);
            }
        }

        JS_FreeValue(ctx, tilesJs);
        JS_FreeValue(ctx, generateCtxJs);
    }
} // namespace OpenRCT2::World::MapGenerator

#endif
