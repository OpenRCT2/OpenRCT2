/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../interface/Colour.h"
#include "../world/Location.hpp"
#include "../entity/EntityBase.h"

struct paint_session;
namespace OpenRCT2
{
    namespace Fireworks
    {
        class Firework : EntityBase
        {
        private:
        public:
            static constexpr auto cEntityType = EntityType::Firework;

            Firework();
            virtual ~Firework();

            static void Create(
                const TileCoordsXY& tile, const uint32_t height, const std::string& objectId, const colour_t color1,
                const colour_t color2, const colour_t color3);
            void Update();
            void Paint(paint_session& session, int32_t imageDirection);
        private:
            uint32_t _currentFrame;

            uint32_t _numFrames;

            colour_t _color1;
            colour_t _color2;
            colour_t _color3;

            bool _useRemap1;
            bool _useRemap2;
            bool _useRemap3;

            uint32_t _frameWidth;
            uint32_t _frameHeight;
        };
    } // namespace Fireworks

} // namespace OpenRCT2
