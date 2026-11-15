/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Location.hpp"
#include "AudioMixer.h"

#include <cstdint>
#include <memory>

namespace OpenRCT2::Audio
{
    struct IAudioSource;

    struct ManagedSoundParams
    {
        double rate = 1.0;
        // -1 repeats forever, 0 plays once, N repeats N times
        int32_t repeat = 0;
        // Start offset in bytes of the source
        uint64_t startOffset = 0;
        // Used to calculate volume and pan from the position, if hasLocation is set
        bool hasLocation = false;
        CoordsXYZ position{};
        int32_t sampleModifier = 0;
        // Music uses the ride music falloff rather than the sound effect falloff
        bool isMusic = false;
    };

    /**
     * Keeps hold of channels after they have been started, so that they can be stopped, changed, paused
     * and have their volume and pan updated from a world position as the viewport moves.
     */
    namespace ManagedSounds
    {
        /**
         * Starts playing the source. The owner is an opaque tag used by StopOwner.
         * @return An id for the playing sound, or 0 if it could not be played.
         */
        int32_t Play(IAudioSource* source, MixerGroup group, const ManagedSoundParams& params, std::shared_ptr<void> owner);

        // Stop a sound
        void Stop(int32_t id);

        // Stop all sounds from owner (plugin, ride, etc)
        void StopOwner(const std::shared_ptr<void>& owner);

        // Stop all sounds from a source (used when freeing a custom sound)
        void StopSource(const IAudioSource* source);

        bool SetRate(int32_t id, double rate);
        bool SetLocation(int32_t id, const CoordsXYZ& position);

        /** Stops all channels, remembering where they were so they can resume. */
        void SetPaused(bool paused);

        /** Updates volume and pan of positional sounds. Call once per frame. */
        void Update();

        /**
         * Calculates the volume and pan (in the game's DS units) of music at the given coordinates, as heard from the
         * music tracking viewport. Returns false if the music is too far away to be heard.
         */
        bool GetMusicLocationParams(const CoordsXYZ& location, int16_t& volume, int16_t& pan);
    } // namespace ManagedSounds
} // namespace OpenRCT2::Audio
