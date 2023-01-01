/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include <memory>
#    include <openrct2/drawing/Image.h>
#    include <openrct2/drawing/ImageId.hpp>
#    include <openrct2/scripting/Duktape.hpp>
#    include <openrct2/scripting/Plugin.h>
#    include <openrct2/scripting/ScriptEngine.h>

namespace OpenRCT2::Scripting
{
    void InitialiseCustomImages(ScriptEngine& scriptEngine);
    std::optional<ImageList> AllocateCustomImages(const std::shared_ptr<Plugin>& plugin, uint32_t count);
    bool FreeCustomImages(const std::shared_ptr<Plugin>& plugin, ImageList range);
    bool DoesPluginOwnImage(const std::shared_ptr<Plugin>& plugin, ImageIndex index);
    DukValue DukGetImageInfo(duk_context* ctx, ImageIndex id);
    DukValue DukGetImagePixelData(duk_context* ctx, ImageIndex id);
    void DukSetPixelData(duk_context* ctx, ImageIndex id, const DukValue& dukPixelData);
    void DukDrawCustomImage(ScriptEngine& scriptEngine, ImageIndex id, ScreenSize size, const DukValue& callback);

} // namespace OpenRCT2::Scripting

#endif
