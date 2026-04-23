/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <gtest/gtest.h>
#include <openrct2/core/FlagHolder.hpp>
#include <openrct2/interface/Widget.h>
#include <openrct2/interface/WindowBase.h>
#include <span>

using namespace OpenRCT2;

namespace
{
    // Pure mirror of widgetIsPressed's flag/bitmask coexistence logic,
    // sans the global input-state branch (which requires libopenrct2ui).
    // Kept in sync with src/openrct2-ui/interface/Widget.cpp.
    bool IsPressedMirror(const WindowBase& w, WidgetIndex i)
    {
        if (w.widgets[i].flags.has(WidgetFlag::isPressed))
            return true;
        if (i < 64 && (w.pressedWidgets & (1uLL << i)))
            return true;
        return false;
    }

    bool IsDisabledMirror(const WindowBase& w, WidgetIndex i)
    {
        if (w.widgets[i].flags.has(WidgetFlag::isDisabled))
            return true;
        if (i < 64 && (w.disabledWidgets & (1uLL << i)))
            return true;
        return false;
    }

    bool IsHoldableMirror(const WindowBase& w, WidgetIndex i)
    {
        if (w.widgets[i].flags.has(WidgetFlag::isHoldable))
            return true;
        if (i < 64 && (w.holdDownWidgets & (1uLL << i)))
            return true;
        return false;
    }
} // namespace

TEST(WidgetStateTest, WidgetDefaultConstructedHasNoFlags)
{
    Widget widget{};
    ASSERT_TRUE(widget.flags.isEmpty());
    ASSERT_FALSE(widget.flags.has(WidgetFlag::isPressed));
    ASSERT_FALSE(widget.flags.has(WidgetFlag::isDisabled));
    ASSERT_FALSE(widget.flags.has(WidgetFlag::isHoldable));
}

TEST(WidgetStateTest, WindowBaseDefaultHasZeroedBitmasks)
{
    WindowBase w;
    ASSERT_EQ(w.pressedWidgets, 0uLL);
    ASSERT_EQ(w.disabledWidgets, 0uLL);
    ASSERT_EQ(w.holdDownWidgets, 0uLL);
}

TEST(WidgetStateTest, FlagHolderSetConditional)
{
    WidgetFlags flags;
    flags.set(WidgetFlag::isPressed, true);
    ASSERT_TRUE(flags.has(WidgetFlag::isPressed));
    flags.set(WidgetFlag::isPressed, false);
    ASSERT_FALSE(flags.has(WidgetFlag::isPressed));
}

TEST(WidgetStateTest, FlagsAreIndependent)
{
    WidgetFlags flags;
    flags.set(WidgetFlag::isPressed);
    flags.set(WidgetFlag::isHoldable);
    ASSERT_TRUE(flags.has(WidgetFlag::isPressed));
    ASSERT_TRUE(flags.has(WidgetFlag::isHoldable));
    ASSERT_FALSE(flags.has(WidgetFlag::isDisabled));
    flags.unset(WidgetFlag::isPressed);
    ASSERT_FALSE(flags.has(WidgetFlag::isPressed));
    ASSERT_TRUE(flags.has(WidgetFlag::isHoldable));
}

TEST(WidgetStateTest, CoexistenceReadsFlagWhenOnlyFlagSet)
{
    WindowBase w;
    w.widgets.resize(5);
    w.widgets[2].flags.set(WidgetFlag::isPressed);
    ASSERT_TRUE(IsPressedMirror(w, 2));
    ASSERT_FALSE(IsPressedMirror(w, 0));
    ASSERT_FALSE(IsPressedMirror(w, 4));
}

TEST(WidgetStateTest, CoexistenceReadsBitmaskWhenOnlyBitmaskSet)
{
    WindowBase w;
    w.widgets.resize(5);
    w.pressedWidgets = (1uLL << 2);
    ASSERT_TRUE(IsPressedMirror(w, 2));
    ASSERT_FALSE(IsPressedMirror(w, 0));
    ASSERT_FALSE(IsPressedMirror(w, 4));
}

TEST(WidgetStateTest, CoexistenceReadsWhenBothSourcesSet)
{
    WindowBase w;
    w.widgets.resize(5);
    w.widgets[2].flags.set(WidgetFlag::isPressed);
    w.pressedWidgets = (1uLL << 2);
    ASSERT_TRUE(IsPressedMirror(w, 2));
}

TEST(WidgetStateTest, CoexistenceFalseWhenNeitherSourceSet)
{
    WindowBase w;
    w.widgets.resize(5);
    for (WidgetIndex i = 0; i < 5; i++)
        ASSERT_FALSE(IsPressedMirror(w, i));
}

TEST(WidgetStateTest, CoexistenceAppliesToDisabled)
{
    WindowBase w;
    w.widgets.resize(5);
    w.widgets[1].flags.set(WidgetFlag::isDisabled);
    w.disabledWidgets = (1uLL << 3);
    ASSERT_TRUE(IsDisabledMirror(w, 1));
    ASSERT_TRUE(IsDisabledMirror(w, 3));
    ASSERT_FALSE(IsDisabledMirror(w, 0));
    ASSERT_FALSE(IsDisabledMirror(w, 2));
    ASSERT_FALSE(IsDisabledMirror(w, 4));
}

TEST(WidgetStateTest, CoexistenceAppliesToHoldable)
{
    WindowBase w;
    w.widgets.resize(5);
    w.widgets[0].flags.set(WidgetFlag::isHoldable);
    w.holdDownWidgets = (1uLL << 4);
    ASSERT_TRUE(IsHoldableMirror(w, 0));
    ASSERT_TRUE(IsHoldableMirror(w, 4));
    ASSERT_FALSE(IsHoldableMirror(w, 1));
    ASSERT_FALSE(IsHoldableMirror(w, 2));
    ASSERT_FALSE(IsHoldableMirror(w, 3));
}

// Regression test for #26421: widgets at index >= 64 must be readable from the
// flag without the bitmask shift wrapping. The scenery window's 65th tab has a
// widget index of 79, which would have shifted 1uLL << 79 == 1uLL << 15, falsely
// reporting the first tab as pressed.
TEST(WidgetStateTest, HighIndexReadsFlagWithoutBitmaskShiftOverflow)
{
    WindowBase w;
    w.widgets.resize(300);
    // Seed the bitmask in a way that would produce a false positive if the code
    // were shifting past 63. Bit 15 set; index 79 (79 % 64 == 15) must NOT read
    // as pressed from the bitmask.
    w.pressedWidgets = (1uLL << 15);
    ASSERT_TRUE(IsPressedMirror(w, 15));
    ASSERT_FALSE(IsPressedMirror(w, 79));
    ASSERT_FALSE(IsPressedMirror(w, 128));
    ASSERT_FALSE(IsPressedMirror(w, 271));

    // Now set the flag on index 79; it must read true from the flag path only.
    w.widgets[79].flags.set(WidgetFlag::isPressed);
    ASSERT_TRUE(IsPressedMirror(w, 79));
    // And seeding a bitmask bit that would wrap to 79 must still not affect 79.
    ASSERT_FALSE(IsPressedMirror(w, 143)); // 143 % 64 == 15
}

TEST(WidgetStateTest, SetWidgetsPreservesFlags)
{
    const Widget source[3] = {
        Widget{},
        Widget{},
        Widget{},
    };
    // Pre-set a flag on the middle source widget.
    Widget mutableSource[3] = { source[0], source[1], source[2] };
    mutableSource[1].flags.set(WidgetFlag::isPressed);
    mutableSource[1].flags.set(WidgetFlag::isHoldable);

    WindowBase w;
    w.setWidgets(std::span<const Widget>(mutableSource, 3));

    ASSERT_EQ(w.widgets.size(), 3u);
    ASSERT_TRUE(w.widgets[1].flags.has(WidgetFlag::isPressed));
    ASSERT_TRUE(w.widgets[1].flags.has(WidgetFlag::isHoldable));
    ASSERT_FALSE(w.widgets[0].flags.has(WidgetFlag::isPressed));
    ASSERT_FALSE(w.widgets[2].flags.has(WidgetFlag::isPressed));
}
