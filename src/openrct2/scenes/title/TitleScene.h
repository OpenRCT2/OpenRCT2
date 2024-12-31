/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../drawing/Drawing.h"
#include "../Scene.h"

struct ITitleSequencePlayer;

namespace OpenRCT2
{
    class TitleScene final : public Scene
    {
    public:
        using Scene::Scene;

        ITitleSequencePlayer* GetSequencePlayer();
        size_t GetCurrentSequence();
        bool PreviewSequence(size_t value);
        void StopPreviewingSequence();
        bool IsPreviewingSequence();

        void Load() override;
        void Tick() override;
        void Stop() override;
        void CreateWindows();
        void ChangePresetSequence(size_t preset);

    private:
        ITitleSequencePlayer* _sequencePlayer = nullptr;
        size_t _loadedTitleSequenceId = SIZE_MAX;
        size_t _currentSequence = SIZE_MAX;
        bool _previewingSequence = false;

        void TitleInitialise();
        bool TryLoadSequence(bool loadPreview = false);
    };
} // namespace OpenRCT2

// When testing title sequences within a normal game
extern bool gPreviewingTitleSequenceInGame;

void TitleCreateWindows();
void* TitleGetSequencePlayer();
void TitleSequenceChangePreset(size_t preset);
size_t TitleGetConfigSequence();
size_t TitleGetCurrentSequence();
bool TitlePreviewSequence(size_t value);
void TitleStopPreviewingSequence();
bool TitleIsPreviewingSequence();
