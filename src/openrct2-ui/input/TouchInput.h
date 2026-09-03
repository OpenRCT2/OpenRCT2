/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <SDL_events.h>
#include <cstdint>
#include <openrct2/interface/ScreenCoords.hpp>

namespace OpenRCT2
{
    struct CursorState;
}

namespace OpenRCT2::Ui
{
    /**
     * Translates SDL finger and gesture events into the engine's mouse input stream.
     *
     * This is deliberately the only place that knows about SDL_FingerID and the
     * normalised coordinates SDL reports for touch, so that the SDL2 -> SDL3 event
     * differences stay contained to one file.
     */
    class TouchInputManager
    {
    public:
        /**
         * Handles a single SDL event.
         *
         * @param e The SDL event to inspect.
         * @param cursorState The engine cursor state to update.
         * @param width Window width in pixels; SDL reports finger positions normalised to this.
         * @param height Window height in pixels.
         * @return true if the event was a touch/gesture event and has been fully handled.
         */
        bool HandleEvent(const SDL_Event& e, CursorState& cursorState, int32_t width, int32_t height);

    private:
        bool HandleFinger(const SDL_Event& e, CursorState& cursorState, int32_t width, int32_t height);
        bool HandleMultiGesture(const SDL_Event& e, int32_t width);

        void OnFingerDown(const SDL_Event& e, CursorState& cursorState, const ScreenCoordsXY& pos);
        void OnFingerMotion(const SDL_Event& e, CursorState& cursorState, const ScreenCoordsXY& pos);
        void OnFingerUp(const SDL_Event& e, CursorState& cursorState, const ScreenCoordsXY& pos);

        void BeginPinch(CursorState& cursorState);
        void UpdatePinch(CursorState& cursorState);

        static constexpr int32_t kMaxTrackedFingers = 2;

        struct Finger
        {
            SDL_FingerID id{};
            ScreenCoordsXY position{};
            bool active{};
        };

        // Only the first two fingers matter: one pans, two pinch. Further fingers are ignored
        // rather than tracked, so a palm resting on the screen cannot start a third gesture.
        Finger _fingers[kMaxTrackedFingers]{};

        // Set once two fingers are down, until every finger has lifted. While set, the
        // single-finger path is suppressed so a pinch cannot also drag or place anything.
        bool _pinching{};

        // Finger separation at the last zoom step. Zoom is driven by the ratio against this
        // rather than an absolute distance, so it behaves the same at any starting spread.
        float _pinchReferenceDistance{};

        uint32_t _lastGestureTimestamp{};
        float _gestureRadius{};
    };
} // namespace OpenRCT2::Ui
