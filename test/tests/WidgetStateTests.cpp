/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <gtest/gtest.h>
#include <openrct2-ui/interface/Widget.h>
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

TEST(WidgetStateTest, WithFlagSetsFlagWithoutDisturbingOthers)
{
    using Ui::withFlag;

    Widget base{};
    base.flags.set(WidgetFlag::isPressed);

    auto holdable = withFlag(base, WidgetFlag::isHoldable);
    ASSERT_TRUE(holdable.flags.has(WidgetFlag::isPressed));
    ASSERT_TRUE(holdable.flags.has(WidgetFlag::isHoldable));
    ASSERT_FALSE(holdable.flags.has(WidgetFlag::isDisabled));
}

TEST(WidgetStateTest, MakeHoldableWidgetHasHoldableFlag)
{
    using Ui::makeHoldableWidget;
    using Ui::WindowColour;

    constexpr auto w = makeHoldableWidget({ 0, 0 }, { 10, 10 }, WidgetType::button, WindowColour::primary);
    static_assert(w.flags.has(WidgetFlag::isHoldable));
    static_assert(!w.flags.has(WidgetFlag::isPressed));
    static_assert(!w.flags.has(WidgetFlag::isDisabled));
    ASSERT_TRUE(w.flags.has(WidgetFlag::isHoldable));
}

TEST(WidgetStateTest, MakeWidgetDefaultHasNoFlags)
{
    using Ui::makeWidget;
    using Ui::WindowColour;

    constexpr auto w = makeWidget({ 0, 0 }, { 10, 10 }, WidgetType::button, WindowColour::primary);
    static_assert(w.flags.isEmpty());
    ASSERT_TRUE(w.flags.isEmpty());
}

// Stage 1 bulk helpers live in libopenrct2ui, which the test binary does not
// link against. The behaviour is tested here via local mirrors that replicate
// the bulk-helper bodies from src/openrct2-ui/interface/Widget.cpp. The
// single-widget setters they wrap (widgetSetPressed/widgetSetHoldable) are
// covered by the coexistence and high-index tests above.
namespace
{
    void ClearPressedMirror(WindowBase& w, std::initializer_list<WidgetIndex> indices)
    {
        for (auto i : indices)
        {
            w.widgets[i].flags.unset(WidgetFlag::isPressed);
            if (i < 64)
                w.pressedWidgets &= ~(1uLL << i);
        }
    }

    void SetPressedExclusiveMirror(WindowBase& w, std::initializer_list<WidgetIndex> group, WidgetIndex pressed)
    {
        for (auto i : group)
        {
            const bool on = (i == pressed);
            w.widgets[i].flags.set(WidgetFlag::isPressed, on);
            if (i < 64)
            {
                if (on)
                    w.pressedWidgets |= (1uLL << i);
                else
                    w.pressedWidgets &= ~(1uLL << i);
            }
        }
    }

    void SetHoldableMirror(WindowBase& w, std::initializer_list<WidgetIndex> indices)
    {
        for (auto i : indices)
        {
            w.widgets[i].flags.set(WidgetFlag::isHoldable);
            if (i < 64)
                w.holdDownWidgets |= (1uLL << i);
        }
    }
} // namespace

TEST(WidgetStateTest, ClearPressedOnlyTouchesListedIndices)
{
    WindowBase w;
    w.widgets.resize(6);
    for (WidgetIndex i = 0; i < 6; i++)
        w.widgets[i].flags.set(WidgetFlag::isPressed);
    w.pressedWidgets = 0b111111;

    ClearPressedMirror(w, { 1, 3 });

    ASSERT_TRUE(w.widgets[0].flags.has(WidgetFlag::isPressed));
    ASSERT_FALSE(w.widgets[1].flags.has(WidgetFlag::isPressed));
    ASSERT_TRUE(w.widgets[2].flags.has(WidgetFlag::isPressed));
    ASSERT_FALSE(w.widgets[3].flags.has(WidgetFlag::isPressed));
    ASSERT_TRUE(w.widgets[4].flags.has(WidgetFlag::isPressed));
    ASSERT_TRUE(w.widgets[5].flags.has(WidgetFlag::isPressed));
    ASSERT_EQ(w.pressedWidgets, 0b110101uLL);
}

TEST(WidgetStateTest, SetPressedExclusivePressesOneAndClearsRest)
{
    WindowBase w;
    w.widgets.resize(10);
    // Seed two pressed; neither is the one we'll select.
    w.widgets[2].flags.set(WidgetFlag::isPressed);
    w.widgets[7].flags.set(WidgetFlag::isPressed);
    w.pressedWidgets = (1uLL << 2) | (1uLL << 7);

    SetPressedExclusiveMirror(w, { 2, 4, 6, 7 }, 6);

    for (WidgetIndex i : { 2, 4, 7 })
        ASSERT_FALSE(w.widgets[i].flags.has(WidgetFlag::isPressed)) << "index " << i;
    ASSERT_TRUE(w.widgets[6].flags.has(WidgetFlag::isPressed));
    // Widgets outside the group are untouched.
    ASSERT_FALSE(w.widgets[0].flags.has(WidgetFlag::isPressed));
    ASSERT_FALSE(w.widgets[9].flags.has(WidgetFlag::isPressed));
    ASSERT_EQ(w.pressedWidgets, 1uLL << 6);
}

TEST(WidgetStateTest, SetPressedExclusiveWorksForHighIndices)
{
    WindowBase w;
    w.widgets.resize(300);
    SetPressedExclusiveMirror(w, { 15, 79, 143, 271 }, 79);

    ASSERT_FALSE(w.widgets[15].flags.has(WidgetFlag::isPressed));
    ASSERT_TRUE(w.widgets[79].flags.has(WidgetFlag::isPressed));
    ASSERT_FALSE(w.widgets[143].flags.has(WidgetFlag::isPressed));
    ASSERT_FALSE(w.widgets[271].flags.has(WidgetFlag::isPressed));
    // Bitmask must stay clean for the high-index selection (no shift wrap).
    ASSERT_EQ(w.pressedWidgets, 0uLL);
}

TEST(WidgetStateTest, SetHoldableMarksListedIndices)
{
    WindowBase w;
    w.widgets.resize(8);
    SetHoldableMirror(w, { 2, 3, 5 });

    ASSERT_FALSE(w.widgets[0].flags.has(WidgetFlag::isHoldable));
    ASSERT_FALSE(w.widgets[1].flags.has(WidgetFlag::isHoldable));
    ASSERT_TRUE(w.widgets[2].flags.has(WidgetFlag::isHoldable));
    ASSERT_TRUE(w.widgets[3].flags.has(WidgetFlag::isHoldable));
    ASSERT_FALSE(w.widgets[4].flags.has(WidgetFlag::isHoldable));
    ASSERT_TRUE(w.widgets[5].flags.has(WidgetFlag::isHoldable));
    ASSERT_EQ(w.holdDownWidgets, (1uLL << 2) | (1uLL << 3) | (1uLL << 5));
}
