/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "Paint.h"

#include <ctime>
#include <memory>
#include <vector>

struct DrawPixelInfo;

namespace OpenRCT2
{
    namespace Drawing
    {
        struct IDrawingEngine;
    } // namespace Drawing

    namespace Ui
    {
        struct IUiContext;
    } // namespace Ui

    namespace Paint
    {
        struct Painter final
        {
        private:
            std::shared_ptr<Ui::IUiContext> const _uiContext;
            std::vector<std::unique_ptr<PaintSession>> _paintSessionPool;
            std::vector<PaintSession*> _freePaintSessions;
            PaintEntryPool _paintStructPool;
            time_t _lastSecond = 0;
            int32_t _currentFPS = 0;
            int32_t _frames = 0;

        public:
            explicit Painter(const std::shared_ptr<Ui::IUiContext>& uiContext);
            void Paint(Drawing::IDrawingEngine& de);

            PaintSession* CreateSession(DrawPixelInfo& dpi, uint32_t viewFlags);
            void ReleaseSession(PaintSession* session);
            ~Painter();

        private:
            void PaintReplayNotice(DrawPixelInfo& dpi, const char* text);
            void PaintFPS(DrawPixelInfo& dpi);
            void MeasureFPS();
        };
    } // namespace Paint
} // namespace OpenRCT2
