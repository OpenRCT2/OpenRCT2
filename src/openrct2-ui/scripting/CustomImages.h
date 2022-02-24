/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include <openrct2/drawing/ImageId.hpp>
#    include <openrct2/scripting/Duktape.hpp>

namespace OpenRCT2::Scripting
{
    DukValue DukGetImageInfo(duk_context* ctx, ImageIndex id);
    DukValue DukGetImagePixelData(duk_context* ctx, ImageIndex id);
    void DukSetPixelData(duk_context* ctx, ImageIndex id, const DukValue& dukPixelData);

} // namespace OpenRCT2::Scripting

#endif
