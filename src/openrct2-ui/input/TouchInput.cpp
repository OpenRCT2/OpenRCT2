/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TouchInput.h"

#include "MouseInput.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <openrct2-ui/interface/Window.h>
#include <openrct2/Input.h>
#include <openrct2/interface/ScreenCoords.hpp>

using namespace OpenRCT2;
using namespace OpenRCT2::Ui;

namespace
{
    // How far apart the fingers must spread or close, relative to the last zoom step, before
    // zooming again. Chosen so a deliberate pinch changes zoom without a small wobble doing so.
    constexpr float kPinchZoomRatio = 1.35f;

    // Below this the two touch points are too close for their separation to be a stable signal.
    constexpr float kMinPinchDistance = 24.0f;

    // Only reachable from the non-macOS branch of HandleFinger, so it is unused on macOS.
    [[maybe_unused]] ScreenCoordsXY GetFingerPosition(const SDL_TouchFingerEvent& tfinger, int32_t width, int32_t height)
    {
        return { static_cast<int32_t>(tfinger.x * width), static_cast<int32_t>(tfinger.y * height) };
    }

    float Distance(const ScreenCoordsXY& a, const ScreenCoordsXY& b)
    {
        const auto dx = static_cast<float>(a.x - b.x);
        const auto dy = static_cast<float>(a.y - b.y);
        return std::sqrt((dx * dx) + (dy * dy));
    }

} // namespace

bool TouchInputManager::HandleEvent(const SDL_Event& e, CursorState& cursorState, int32_t width, int32_t height)
{
    switch (e.type)
    {
        case SDL_FINGERMOTION:
        case SDL_FINGERDOWN:
        case SDL_FINGERUP:
            return HandleFinger(e, cursorState, width, height);
        case SDL_MULTIGESTURE:
            return HandleMultiGesture(e, width);
        default:
            return false;
    }
}

bool TouchInputManager::HandleFinger(
    [[maybe_unused]] const SDL_Event& e, [[maybe_unused]] CursorState& cursorState, [[maybe_unused]] int32_t width,
    [[maybe_unused]] int32_t height)
{
    // Apple sends touchscreen events for trackpads, so ignore these events on macOS.
    // Removing this guard reintroduces the double-input bugs of #7760.
#ifdef __MACOSX__
    return false;
#else
    const auto fingerPos = GetFingerPosition(e.tfinger, width, height);

    switch (e.type)
    {
        case SDL_FINGERMOTION:
            OnFingerMotion(e, cursorState, fingerPos);
            return true;
        case SDL_FINGERDOWN:
            OnFingerDown(e, cursorState, fingerPos);
            return true;
        case SDL_FINGERUP:
            OnFingerUp(e, cursorState, fingerPos);
            return true;
        default:
            return false;
    }
#endif
}

void TouchInputManager::OnFingerDown(const SDL_Event& e, CursorState& cursorState, const ScreenCoordsXY& pos)
{
    if (!_fingers[0].active && !_fingers[1].active)
    {
        // Nothing was being tracked, so no gesture can be in progress. SDL does not always
        // deliver the final SDL_FINGERUP -- backgrounding the app on Android eats it -- and
        // a gesture left set would swallow all input from here on.
        if (_pinching)
        {
            _pinching = false;
            InputSetTouchGestureActive(false);
        }
    }

    if (!_fingers[0].active)
    {
        _fingers[0] = { e.tfinger.fingerId, pos, true };
    }
    else if (!_fingers[1].active)
    {
        _fingers[1] = { e.tfinger.fingerId, pos, true };
        BeginPinch(cursorState);
        return;
    }
    else
    {
        // A third finger adds nothing; ignore it rather than letting it start a gesture.
        return;
    }

    if (_pinching)
        return;

    // Unlike a mouse, a finger produces no motion events before it touches down.
    // Without this, ProcessMouseTool() keeps previewing the tool ghost at the
    // previous touch location while the tool commits at the new one. See #17101.
    cursorState.position = pos;

    // No finger gesture maps to a right click. Right clicking a viewport removes whatever is
    // under it immediately, which is the wrong amount of ceremony for something you can do by
    // resting a finger on the glass. Removal goes through Clear Scenery and an explicit
    // Destroy button instead, so exactly one thing in the whole interface commits an action.
    StoreMouseInput(MouseState::leftPress, pos);
    cursorState.left = CURSOR_PRESSED;
    cursorState.old = 1;
    cursorState.touch = true;
}

void TouchInputManager::OnFingerMotion(const SDL_Event& e, CursorState& cursorState, const ScreenCoordsXY& pos)
{
    for (auto& finger : _fingers)
    {
        if (finger.active && finger.id == e.tfinger.fingerId)
        {
            finger.position = pos;
            break;
        }
    }

    if (_pinching)
    {
        UpdatePinch(cursorState);
        return;
    }

    // Only the finger that started the interaction drives the cursor.
    if (_fingers[0].active && _fingers[0].id != e.tfinger.fingerId)
        return;

    cursorState.position = pos;
}

void TouchInputManager::OnFingerUp(const SDL_Event& e, CursorState& cursorState, const ScreenCoordsXY& pos)
{
    bool wasTracked = false;
    for (auto& finger : _fingers)
    {
        if (finger.active && finger.id == e.tfinger.fingerId)
        {
            finger = {};
            wasTracked = true;
            break;
        }
    }

    if (_pinching)
    {
        // Stay in the pinch until the screen is clear, so lifting one finger of a pinch
        // does not hand the remaining finger a pan or a tap it never started.
        if (!_fingers[0].active && !_fingers[1].active)
        {
            _pinching = false;
            InputSetTouchGestureActive(false);
        }
        return;
    }

    if (!wasTracked)
        return;

    // Keep the surviving finger, if any, in the first slot so it remains the tracked one.
    if (!_fingers[0].active && _fingers[1].active)
    {
        _fingers[0] = _fingers[1];
        _fingers[1] = {};
    }

    StoreMouseInput(MouseState::leftRelease, pos);
    cursorState.left = CURSOR_RELEASED;
    cursorState.old = 3;
    cursorState.touch = true;
}

void TouchInputManager::BeginPinch(CursorState& cursorState)
{
    _pinching = true;
    _pinchReferenceDistance = Distance(_fingers[0].position, _fingers[1].position);

    // The first finger may already have armed a tool or started a pan. A second finger claims
    // the gesture exclusively, so drop that interaction rather than compounding the two.
    InputSetTouchGestureActive(true);
    cursorState.left = CURSOR_RELEASED;
}

void TouchInputManager::UpdatePinch(CursorState& cursorState)
{
    const auto distance = Distance(_fingers[0].position, _fingers[1].position);
    if (distance < kMinPinchDistance || _pinchReferenceDistance < kMinPinchDistance)
    {
        _pinchReferenceDistance = std::max(distance, kMinPinchDistance);
        return;
    }

    const auto ratio = distance / _pinchReferenceDistance;
    if (ratio < kPinchZoomRatio && ratio > (1.0f / kPinchZoomRatio))
        return;

    // Zoom about the point between the fingers rather than the screen centre, so the map
    // feature being pinched stays under them. MainWindowZoom reads the cursor position.
    cursorState.position = { (_fingers[0].position.x + _fingers[1].position.x) / 2,
                             (_fingers[0].position.y + _fingers[1].position.y) / 2 };
    Windows::MainWindowZoom(ratio > 1.0f, true);
    _pinchReferenceDistance = distance;
}

bool TouchInputManager::HandleMultiGesture(const SDL_Event& e, int32_t width)
{
    // Note: unlike the finger events above, this is not excluded on macOS — trackpad
    // pinch-to-zoom goes through here and is intentionally supported.
    if (e.mgesture.numFingers != 2)
        return true;

    // A finger pinch is driven by the finger events above, which can zoom about the pinch
    // centre rather than about wherever the cursor happens to be. This path is left for
    // trackpads, which report no finger events outside macOS.
    if (_pinching)
        return true;

    if (e.mgesture.timestamp > _lastGestureTimestamp + 1000)
    {
        _gestureRadius = 0;
    }
    _lastGestureTimestamp = e.mgesture.timestamp;
    _gestureRadius += e.mgesture.dDist;

    // Zoom gesture
    constexpr int32_t kTolerance = 128;
    int32_t gesturePixels = static_cast<int32_t>(_gestureRadius * width);
    if (abs(gesturePixels) > kTolerance)
    {
        _gestureRadius = 0;
        Windows::MainWindowZoom(gesturePixels > 0, true);
    }
    return true;
}
