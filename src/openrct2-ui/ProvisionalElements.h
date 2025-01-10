/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

namespace OpenRCT2::Ui
{
    void ProvisionalElementsRemove();
    void ProvisionalElementsRestore();

    namespace Windows
    {
        /**
         * Called after building something else, which might affect the ability to build paths there.
         */
        void FootpathRecheckProvisional();
        /**
         * Permanently remove provisional footpaths - e.g. after successful placement, or when cancelling placement.
         */
        void FootpathRemoveProvisional();
        void FootpathUpdateProvisional();
        /**
         * Used when performing actions on footpaths where provisional elements may interfere, and where the provisional
         * elements must be restored later on (which can be done by calling `WindowFootpathRestoreProvisional()`.
         */
        void FootpathRemoveProvisionalTemporarily();
        /**
         * Restore provisional elements that have been removed using `WindowFootpathRemoveProvisionalTemporarily()`.
         */
        void FootpathRestoreProvisional();
    } // namespace Windows

} // namespace OpenRCT2::Ui
