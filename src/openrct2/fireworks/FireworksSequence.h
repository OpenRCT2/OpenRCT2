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

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace OpenRCT2
{
    namespace Fireworks
    {
        constexpr uint32_t MaximumDelay = 0xFFFFFFFF;
        struct Event
        {
            uint32_t Id;
            std::string ObjectId;
            uint32_t SpawnerId;
            uint32_t Height;
            uint32_t Time;
            colour_t Palette1;
            colour_t Palette2;
            colour_t Palette3;

            friend bool operator==(const Event& e0, const Event& e1)
            {
                return e0.Id == e1.Id && e0.ObjectId == e1.ObjectId && e0.SpawnerId == e1.SpawnerId && e0.Height == e1.Height
                    && e0.Time == e1.Time && e0.Palette1 == e1.Palette1 && e0.Palette2 == e1.Palette2
                    && e0.Palette3 == e1.Palette3;
            }
        };

        struct Spawner
        {
            uint32_t Id;
            TileCoordsXY Location;
        };

        class IFireworkHandler
        {
        public:
            virtual ~IFireworkHandler(){};
            virtual void OnEvent(const Event& e) = 0;
            virtual void OnUpdate() = 0;
            virtual void OnMusicLaunch(const std::string& musicId) = 0;
            virtual void OnReset() = 0;
        };

        class FireworksSequence
        {
        public:
            FireworksSequence();
            FireworksSequence(const FireworksSequence& sequence) = delete;
            virtual ~FireworksSequence();

            std::string Name;
            std::string BackgroundMusic;

            void Start();
            void Update(const uint32_t steps);
            void Stop();

            Spawner AddSpawner(const TileCoordsXY& location);
            bool RemoveSpawner(const TileCoordsXY& location);
            const Spawner* GetSpawner(const uint32_t id) const;

            Event AddEvent(
                const std::string& objectId, const Spawner& spawner, const uint32_t height, const uint32_t time,
                const colour_t palette1 = COLOUR_NULL, const colour_t palette2 = COLOUR_NULL,
                const colour_t palette3 = COLOUR_NULL);
            bool RemoveEvent(const uint32_t time, const Spawner& spawnerId);

            bool HasFinished() const;

            void SetHandler(IFireworkHandler& handler);

        private:
            std::vector<Spawner> _spawners;
            std::vector<Event> _events;

            uint32_t _currentTime;
            uint32_t _currentEvent;

            IFireworkHandler* _handler;
            bool _playing;
        };
    } // namespace Fireworks

} // namespace OpenRCT2
