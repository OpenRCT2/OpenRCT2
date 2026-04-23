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
    // Pure mirrors of the widgetIs*/widgetSet* readers/setters, sans the global
    // input-state branch in widgetIsPressed (which requires libopenrct2ui).
    // Kept in sync with src/openrct2-ui/interface/Widget.cpp.
    bool IsPressedMirror(const WindowBase& w, WidgetIndex i)
    {
        if (w.useWidgetFlags)
            return w.widgets[i].flags.has(WidgetFlag::isPressed);
        return i < 64 && (w.pressedWidgets & (1uLL << i)) != 0;
    }

    bool IsDisabledMirror(const WindowBase& w, WidgetIndex i)
    {
        if (w.useWidgetFlags)
            return w.widgets[i].flags.has(WidgetFlag::isDisabled);
        return i < 64 && (w.disabledWidgets & (1uLL << i)) != 0;
    }

    bool IsHoldableMirror(const WindowBase& w, WidgetIndex i)
    {
        if (w.useWidgetFlags)
            return w.widgets[i].flags.has(WidgetFlag::isHoldable);
        return i < 64 && (w.holdDownWidgets & (1uLL << i)) != 0;
    }

    void SetPressedMirror(WindowBase& w, WidgetIndex i, bool v)
    {
        if (w.useWidgetFlags)
        {
            w.widgets[i].flags.set(WidgetFlag::isPressed, v);
            return;
        }
        if (i >= 64)
            return;
        if (v)
            w.pressedWidgets |= (1uLL << i);
        else
            w.pressedWidgets &= ~(1uLL << i);
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

TEST(WidgetStateTest, WindowBaseDefaultHasZeroedBitmasksAndOptedOut)
{
    WindowBase w;
    ASSERT_EQ(w.pressedWidgets, 0uLL);
    ASSERT_EQ(w.disabledWidgets, 0uLL);
    ASSERT_EQ(w.holdDownWidgets, 0uLL);
    ASSERT_FALSE(w.useWidgetFlags);
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

// Default (opt-out) windows must read from the bitmask only. Any stale flag on a
// widget MUST NOT leak into the pressed state. This is the regression-prevention
// case for the Stage 0 flag-leak bug: unmigrated windows that zero the bitmask
// directly (`pressedWidgets = 0`) used to leave widget flags unchanged, and an
// OR-both-sources reader would then return a stale-true.
TEST(WidgetStateTest, OptedOutWindowIgnoresFlagAndReadsBitmaskOnly)
{
    WindowBase w;
    w.widgets.resize(5);
    // Widget flag set from a previous frame, but the bitmask is zero.
    w.widgets[2].flags.set(WidgetFlag::isPressed);
    ASSERT_FALSE(IsPressedMirror(w, 2));

    // Bitmask is the source of truth.
    w.pressedWidgets = (1uLL << 3);
    ASSERT_FALSE(IsPressedMirror(w, 2));
    ASSERT_TRUE(IsPressedMirror(w, 3));
}

TEST(WidgetStateTest, OptedInWindowIgnoresBitmaskAndReadsFlagOnly)
{
    WindowBase w;
    w.useWidgetFlags = true;
    w.widgets.resize(5);
    // Bitmask set from outside; opted-in reader must not consult it.
    w.pressedWidgets = (1uLL << 2);
    ASSERT_FALSE(IsPressedMirror(w, 2));

    w.widgets[2].flags.set(WidgetFlag::isPressed);
    ASSERT_TRUE(IsPressedMirror(w, 2));
    ASSERT_FALSE(IsPressedMirror(w, 0));
    ASSERT_FALSE(IsPressedMirror(w, 4));
}

TEST(WidgetStateTest, OptInAppliesToDisabledAndHoldable)
{
    WindowBase w;
    w.useWidgetFlags = true;
    w.widgets.resize(5);

    w.disabledWidgets = (1uLL << 3);
    w.holdDownWidgets = (1uLL << 4);
    ASSERT_FALSE(IsDisabledMirror(w, 3));
    ASSERT_FALSE(IsHoldableMirror(w, 4));

    w.widgets[1].flags.set(WidgetFlag::isDisabled);
    w.widgets[0].flags.set(WidgetFlag::isHoldable);
    ASSERT_TRUE(IsDisabledMirror(w, 1));
    ASSERT_TRUE(IsHoldableMirror(w, 0));
}

TEST(WidgetStateTest, OptedOutSetterWritesBitmaskOnly)
{
    WindowBase w;
    w.widgets.resize(5);
    SetPressedMirror(w, 2, true);
    ASSERT_EQ(w.pressedWidgets, 1uLL << 2);
    ASSERT_FALSE(w.widgets[2].flags.has(WidgetFlag::isPressed));

    SetPressedMirror(w, 2, false);
    ASSERT_EQ(w.pressedWidgets, 0uLL);
}

TEST(WidgetStateTest, OptedInSetterWritesFlagOnly)
{
    WindowBase w;
    w.useWidgetFlags = true;
    w.widgets.resize(5);
    SetPressedMirror(w, 2, true);
    ASSERT_TRUE(w.widgets[2].flags.has(WidgetFlag::isPressed));
    ASSERT_EQ(w.pressedWidgets, 0uLL);

    SetPressedMirror(w, 2, false);
    ASSERT_FALSE(w.widgets[2].flags.has(WidgetFlag::isPressed));
}

// Regression test for #26421: widgets at index >= 64 must be readable/writable
// from the flag without any bitmask shift wrapping. The scenery window's 65th
// tab has a widget index of 79, which would have shifted 1uLL << 79 == 1uLL << 15,
// falsely reporting the first tab as pressed.
TEST(WidgetStateTest, HighIndexWorksWhenOptedIn)
{
    WindowBase w;
    w.useWidgetFlags = true;
    w.widgets.resize(300);
    SetPressedMirror(w, 79, true);
    ASSERT_TRUE(IsPressedMirror(w, 79));
    // Must not false-positive at index 15 (79 % 64 == 15) or any shift-wrap alias.
    ASSERT_FALSE(IsPressedMirror(w, 15));
    ASSERT_FALSE(IsPressedMirror(w, 143)); // 143 % 64 == 15
    ASSERT_FALSE(IsPressedMirror(w, 271));
    ASSERT_EQ(w.pressedWidgets, 0uLL);
}

TEST(WidgetStateTest, SetWidgetsPreservesFlags)
{
    Widget mutableSource[3] = { Widget{}, Widget{}, Widget{} };
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

// Bulk helpers live in libopenrct2ui which the test binary does not link.
// Behaviour is tested via local mirrors that replicate the helper bodies from
// src/openrct2-ui/interface/Widget.cpp. All bulk helpers go through
// widgetSetPressed/widgetSetHoldable, which branch on useWidgetFlags.
namespace
{
    void SetHoldableOneMirror(WindowBase& w, WidgetIndex i, bool v)
    {
        if (w.useWidgetFlags)
        {
            w.widgets[i].flags.set(WidgetFlag::isHoldable, v);
            return;
        }
        if (i >= 64)
            return;
        if (v)
            w.holdDownWidgets |= (1uLL << i);
        else
            w.holdDownWidgets &= ~(1uLL << i);
    }

    void ClearPressedMirror(WindowBase& w, std::initializer_list<WidgetIndex> indices)
    {
        for (auto i : indices)
            SetPressedMirror(w, i, false);
    }

    void SetPressedExclusiveMirror(WindowBase& w, std::initializer_list<WidgetIndex> group, WidgetIndex pressed)
    {
        for (auto i : group)
            SetPressedMirror(w, i, i == pressed);
    }

    void SetHoldableBulkMirror(WindowBase& w, std::initializer_list<WidgetIndex> indices)
    {
        for (auto i : indices)
            SetHoldableOneMirror(w, i, true);
    }
} // namespace

TEST(WidgetStateTest, ClearPressedOnlyTouchesListedIndices)
{
    WindowBase w;
    w.useWidgetFlags = true;
    w.widgets.resize(6);
    for (WidgetIndex i = 0; i < 6; i++)
        w.widgets[i].flags.set(WidgetFlag::isPressed);

    ClearPressedMirror(w, { 1, 3 });

    ASSERT_TRUE(w.widgets[0].flags.has(WidgetFlag::isPressed));
    ASSERT_FALSE(w.widgets[1].flags.has(WidgetFlag::isPressed));
    ASSERT_TRUE(w.widgets[2].flags.has(WidgetFlag::isPressed));
    ASSERT_FALSE(w.widgets[3].flags.has(WidgetFlag::isPressed));
    ASSERT_TRUE(w.widgets[4].flags.has(WidgetFlag::isPressed));
    ASSERT_TRUE(w.widgets[5].flags.has(WidgetFlag::isPressed));
}

TEST(WidgetStateTest, SetPressedExclusivePressesOneAndClearsRest)
{
    WindowBase w;
    w.useWidgetFlags = true;
    w.widgets.resize(10);
    w.widgets[2].flags.set(WidgetFlag::isPressed);
    w.widgets[7].flags.set(WidgetFlag::isPressed);

    SetPressedExclusiveMirror(w, { 2, 4, 6, 7 }, 6);

    for (WidgetIndex i : { 2, 4, 7 })
        ASSERT_FALSE(w.widgets[i].flags.has(WidgetFlag::isPressed)) << "index " << i;
    ASSERT_TRUE(w.widgets[6].flags.has(WidgetFlag::isPressed));
    // Widgets outside the group are untouched.
    ASSERT_FALSE(w.widgets[0].flags.has(WidgetFlag::isPressed));
    ASSERT_FALSE(w.widgets[9].flags.has(WidgetFlag::isPressed));
}

TEST(WidgetStateTest, SetPressedExclusiveWorksForHighIndices)
{
    WindowBase w;
    w.useWidgetFlags = true;
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
    w.useWidgetFlags = true;
    w.widgets.resize(8);
    SetHoldableBulkMirror(w, { 2, 3, 5 });

    ASSERT_FALSE(w.widgets[0].flags.has(WidgetFlag::isHoldable));
    ASSERT_FALSE(w.widgets[1].flags.has(WidgetFlag::isHoldable));
    ASSERT_TRUE(w.widgets[2].flags.has(WidgetFlag::isHoldable));
    ASSERT_TRUE(w.widgets[3].flags.has(WidgetFlag::isHoldable));
    ASSERT_FALSE(w.widgets[4].flags.has(WidgetFlag::isHoldable));
    ASSERT_TRUE(w.widgets[5].flags.has(WidgetFlag::isHoldable));
}
