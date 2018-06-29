/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <ctime>
#include <memory>
#include "../common.h"

struct rct_drawpixelinfo;

namespace OpenRCT2
{
    namespace Drawing
    {
        interface IDrawingEngine;
    } // namespace Drawing

    namespace Ui
    {
        interface IUiContext;
    } // namespace Ui

    namespace Paint
    {
        class Painter final
        {
        private:
            std::shared_ptr<Ui::IUiContext> const _uiContext;

            time_t  _lastSecond = 0;
            int32_t  _currentFPS = 0;
            int32_t  _frames     = 0;

        public:
            explicit Painter(const std::shared_ptr<Ui::IUiContext>& uiContext);
            void Paint(Drawing::IDrawingEngine& de);

        private:
            void PaintFPS(rct_drawpixelinfo * dpi);
            void MeasureFPS();
        };
    } // namespace Paint
} // namespace OpenRCT2
