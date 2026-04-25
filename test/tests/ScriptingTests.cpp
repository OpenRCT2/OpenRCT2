/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <gtest/gtest.h>
#include <openrct2/Context.h>
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/scripting/ScriptEngine.h>
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

#endif
