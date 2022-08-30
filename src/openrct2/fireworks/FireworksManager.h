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
#include "FireworksSequence.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

struct Firework;
namespace OpenRCT2
{
    namespace Fireworks
    {
        class FireworksManager: public IFireworkHandler
        {
        public:
            FireworksManager();
            FireworksManager(const FireworksManager& obj) = delete;
            virtual ~FireworksManager();

            virtual void OnEvent(const Event& e) override;
            virtual void OnUpdate() override;
            virtual void OnMusicLaunch(const std::string& musicId) override;
            virtual void OnReset() override;

            void Update();
            FireworksSequence& GetSequence();
        private:
            FireworksSequence _sequence;
            std::vector<Firework*> _fireworks;
        };
    } // namespace Fireworks

} // namespace OpenRCT2
