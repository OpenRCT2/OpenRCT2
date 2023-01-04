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
#include "../drawing/Drawing.h"

struct ITitleSequencePlayer;

namespace OpenRCT2
{
    class GameState;

    class TitleScreen final
    {
    public:
        TitleScreen(GameState& gameState);
        ~TitleScreen();

        ITitleSequencePlayer* GetSequencePlayer();
        size_t GetCurrentSequence();
        bool PreviewSequence(size_t value);
        void StopPreviewingSequence();
        bool IsPreviewingSequence();
        bool ShouldHideVersionInfo();
        void SetHideVersionInfo(bool value);

        void Load();
        void Tick();
        void CreateWindows();
        void ChangePresetSequence(size_t preset);

    private:
        GameState& _gameState;

        ITitleSequencePlayer* _sequencePlayer = nullptr;
        size_t _loadedTitleSequenceId = SIZE_MAX;
        size_t _currentSequence = SIZE_MAX;
        bool _hideVersionInfo = false;
        bool _previewingSequence = false;

        void TitleInitialise();
        bool TryLoadSequence(bool loadPreview = false);
    };
} // namespace OpenRCT2

// When testing title sequences within a normal game
extern bool gPreviewingTitleSequenceInGame;

void title_load();
void title_create_windows();
void* title_get_sequence_player();
void title_sequence_change_preset(size_t preset);
bool title_should_hide_version_info();
void title_set_hide_version_info(bool value);
size_t title_get_config_sequence();
size_t title_get_current_sequence();
bool title_preview_sequence(size_t value);
void title_stop_previewing_sequence();
bool title_is_previewing_sequence();
void DrawOpenRCT2(rct_drawpixelinfo* dpi, const ScreenCoordsXY& screenCoords);
