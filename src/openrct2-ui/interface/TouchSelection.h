/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <openrct2/Context.h>
#include <openrct2/Input.h>
#include <optional>

namespace OpenRCT2::Ui
{
    /**
     * Splits picking an item out of a list into looking and choosing, for touchscreens.
     *
     * With a mouse, hovering a ride or a scenery item shows its name, cost and stats, and the
     * click that follows is a separate, deliberate act. A finger cannot hover: the first touch
     * is already the choice, so there is no way to browse a list without committing to it.
     *
     * The first tap on an item previews it. Tapping the same item again chooses it. Tapping a
     * different item just moves the preview. There is deliberately no time limit: a double-tap
     * window would make a slow second tap silently do nothing, and nothing else in the touch
     * interface is timing-sensitive either.
     */
    template<typename T>
    class TouchTwoStepSelection
    {
    public:
        /**
         * @return true if this tap should choose the item, false if it should only preview it.
         *         Always true for mouse input or when the option is off, so those paths are
         *         unchanged.
         */
        bool shouldCommit(const T& item)
        {
            if (!IsTwoStepSelectionActive())
            {
                _previewed.reset();
                return true;
            }

            if (_previewed.has_value() && _previewed.value() == item)
            {
                _previewed.reset();
                return true;
            }

            _previewed = item;
            return false;
        }

        /**
         * Forgets the pending preview, so a reopened or refiltered list starts from looking again
         * rather than treating a tap on whatever now sits under the finger as a confirmation.
         */
        void reset()
        {
            _previewed.reset();
        }

    private:
        static bool IsTwoStepSelectionActive()
        {
            const CursorState* cursorState = ContextGetCursorState();
            return cursorState != nullptr && cursorState->touch;
        }

        std::optional<T> _previewed;
    };
} // namespace OpenRCT2::Ui
