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

TEST(WidgetStateTest, WidgetDefaultConstructedHasNoFlags)
{
    Widget widget{};
    ASSERT_TRUE(widget.flags.isEmpty());
    ASSERT_FALSE(widget.flags.has(WidgetFlag::isPressed));
    ASSERT_FALSE(widget.flags.has(WidgetFlag::isDisabled));
    ASSERT_FALSE(widget.flags.has(WidgetFlag::isHoldable));
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

// Regression guard for #26421: widgets at index >= 64 must round-trip cleanly.
// The scenery window's 65th tab has widget index 79, which under the old uint64
// bitmask wrapped (1uLL << 79 == 1uLL << 15), falsely reporting the first tab
// as pressed. Per-widget flags have no such cap.
TEST(WidgetStateTest, HighIndexRoundTripsWithoutShiftWrap)
{
    WindowBase w;
    w.widgets.resize(300);
    w.widgets[79].flags.set(WidgetFlag::isPressed);

    ASSERT_TRUE(w.widgets[79].flags.has(WidgetFlag::isPressed));
    // Must not false-positive at any prior shift-wrap alias.
    ASSERT_FALSE(w.widgets[15].flags.has(WidgetFlag::isPressed));  // 79 % 64
    ASSERT_FALSE(w.widgets[143].flags.has(WidgetFlag::isPressed)); // 143 % 64 == 15
    ASSERT_FALSE(w.widgets[271].flags.has(WidgetFlag::isPressed));
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

TEST(WidgetStateTest, MakeSpinnerWidgetsHasNoHoldableFlag)
{
    using Ui::makeSpinnerWidgets;
    using Ui::WindowColour;

    constexpr auto widgets = makeSpinnerWidgets({ 0, 0 }, { 100, 14 }, WidgetType::spinner, WindowColour::primary);
    static_assert(!widgets[0].flags.has(WidgetFlag::isHoldable));
    static_assert(!widgets[1].flags.has(WidgetFlag::isHoldable));
    static_assert(!widgets[2].flags.has(WidgetFlag::isHoldable));
}

TEST(WidgetStateTest, MakeHoldableSpinnerWidgetsHasHoldableOnIncrementButtons)
{
    using Ui::makeHoldableSpinnerWidgets;
    using Ui::WindowColour;

    constexpr auto widgets = makeHoldableSpinnerWidgets({ 0, 0 }, { 100, 14 }, WidgetType::spinner, WindowColour::primary);
    // Element [0] is the spinner input field; increment/decrement buttons are
    // [1] and [2] and must be holdable for press-and-hold repeat.
    static_assert(!widgets[0].flags.has(WidgetFlag::isHoldable));
    static_assert(widgets[1].flags.has(WidgetFlag::isHoldable));
    static_assert(widgets[2].flags.has(WidgetFlag::isHoldable));
}
