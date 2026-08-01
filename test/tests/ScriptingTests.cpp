/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TestData.h"

#include <gtest/gtest.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/scripting/ScriptEngine.h>
#include <openrct2/scripting/bindings/world/ScTileElement.hpp>
#include <openrct2/world/Map.h>
#include <openrct2/world/tile_element/TileElement.h>
#include <quickjs.h>

using namespace OpenRCT2;
using namespace OpenRCT2::Scripting;

class ScriptingTests : public testing::Test
{
protected:
    void SetUp() override
    {
        gOpenRCT2Headless = true;
        gOpenRCT2NoGraphics = true;
        _context = CreateContext();
        _context->Initialise();
    }

    std::unique_ptr<IContext> _context;
};

#ifdef ENABLE_SCRIPTING

TEST_F(ScriptingTests, MultipleSubscribersToSameEventShouldNotCrash)
{
    auto& scriptEngine = static_cast<ScriptEngine&>(_context->GetScriptEngine());

    // Register a plugin that subscribes twice to the same event
    const char* pluginCode = R"(
        registerPlugin({
            name: 'test-plugin-multiple-subscribers',
            version: '1.0.0',
            authors: ['openrct2-test'],
            type: 'remote',
            licence: 'MIT',
            minApiVersion: 110, // deliberately the version before quickjs
            targetApiVersion: 110,
            main: function () {
                context.subscribe('interval.tick', function (e) {
                    // first subscriber
                });
                context.subscribe('interval.tick', function (e) {
                    // second subscriber
                });
            }
        });
    )";

    scriptEngine.AddNetworkPlugin(pluginCode);
    scriptEngine.LoadTransientPlugins();
    scriptEngine.Tick();

    auto& hookEngine = scriptEngine.GetHookEngine();

    // We need a JSValue to pass to Call.
    JSContext* ctx = scriptEngine.GetContext();
    JSValue arg = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, arg, "test", JS_NewInt32(ctx, 1));

    // This should NOT crash.
    hookEngine.Call(HookType::intervalTick, arg, false);
}

class ScriptingTileElementTests : public testing::Test
{
protected:
    static void SetUpTestCase()
    {
        gOpenRCT2Headless = true;
        gOpenRCT2NoGraphics = true;
        _context = CreateContext();
        ASSERT_TRUE(_context->Initialise());

        // Restored on teardown so that the following tests start from the scene they expect.
        _previousScene = gLegacyScene;

        _context->LoadParkFromFile(TestData::GetParkPath("tile-element-tests.sv6"));
        GameLoadInit(); // NB: calls `setActiveScene`
    }

    static void TearDownTestCase()
    {
        if (_context != nullptr)
            _context.reset();

        gLegacyScene = _previousScene;
    }

    // Returns the coordinates of a tile that is known to have at least one element.
    static CoordsXY GetPopulatedTile()
    {
        for (int32_t y = 0; y < 64; y++)
        {
            for (int32_t x = 0; x < 64; x++)
            {
                auto coords = TileCoordsXY{ x, y }.ToCoordsXY();
                if (MapGetFirstElementAt(coords) != nullptr)
                    return coords;
            }
        }
        return {};
    }

    static std::shared_ptr<IContext> _context;
    static LegacyScene _previousScene;
};

std::shared_ptr<IContext> ScriptingTileElementTests::_context;
LegacyScene ScriptingTileElementTests::_previousScene;

/**
 * Reorganising the tile elements reallocates the vector they are stored in, invalidating every
 * outstanding TileElement pointer. A script object referencing an element must survive that.
 */
TEST_F(ScriptingTileElementTests, ElementReferenceSurvivesTileElementReorganisation)
{
    auto& scriptEngine = static_cast<ScriptEngine&>(GetContext()->GetScriptEngine());
    auto* ctx = scriptEngine.GetContext();

    const auto coords = GetPopulatedTile();
    const auto* element = MapGetFirstElementAt(coords);
    ASSERT_NE(element, nullptr);
    const auto expectedBaseHeight = element->baseHeight;

    auto jsElement = gScTileElement.New(ctx, coords, 0);

    ReorganiseTileElements();

    auto jsBaseHeight = JS_GetPropertyStr(ctx, jsElement, "baseHeight");
    int32_t actualBaseHeight{};
    JS_ToInt32(ctx, &actualBaseHeight, jsBaseHeight);
    JS_FreeValue(ctx, jsBaseHeight);
    JS_FreeValue(ctx, jsElement);

    EXPECT_EQ(actualBaseHeight, expectedBaseHeight);
}

/**
 * Referencing an element that no longer exists should report null rather than reading freed memory.
 */
TEST_F(ScriptingTileElementTests, ElementReferenceThatNoLongerExistsReturnsNull)
{
    auto& scriptEngine = static_cast<ScriptEngine&>(GetContext()->GetScriptEngine());
    auto* ctx = scriptEngine.GetContext();

    const auto coords = GetPopulatedTile();
    auto jsElement = gScTileElement.New(ctx, coords, 9999);

    auto jsBaseHeight = JS_GetPropertyStr(ctx, jsElement, "baseHeight");
    EXPECT_TRUE(JS_IsNull(jsBaseHeight));
    JS_FreeValue(ctx, jsBaseHeight);
    JS_FreeValue(ctx, jsElement);
}

#endif
