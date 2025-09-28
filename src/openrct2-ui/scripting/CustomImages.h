/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include <memory>
    #include <openrct2/drawing/Image.h>
    #include <openrct2/scripting/Plugin.h>
    #include <openrct2/scripting/ScriptEngine.h>

namespace OpenRCT2::Scripting
{
    void InitialiseCustomImages(ScriptEngine& scriptEngine);
    std::optional<ImageList> AllocateCustomImages(const std::shared_ptr<Plugin>& plugin, uint32_t count);
    bool FreeCustomImages(const std::shared_ptr<Plugin>& plugin, ImageList range);
    bool DoesPluginOwnImage(const std::shared_ptr<Plugin>& plugin, ImageIndex index);
    JSValue JSGetImageInfo(JSContext* ctx, ImageIndex id);
    JSValue JSGetImagePixelData(JSContext* ctx, ImageIndex id);
    void JSSetPixelData(JSContext* ctx, ImageIndex id, JSValue jsPixelData);
    void JSDrawCustomImage(ScriptEngine& scriptEngine, ImageIndex id, ScreenSize size, const JSCallback& callback);

} // namespace OpenRCT2::Scripting

#endif
