#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#include "../common.h"

#include "../drawing/Drawing.h"

interface ITitleSequencePlayer;

namespace OpenRCT2
{
    class GameState;

    class TitleScreen final
    {
    public:
        TitleScreen(GameState& gameState);
        ~TitleScreen();

        ITitleSequencePlayer *  GetSequencePlayer();
        size_t                  GetCurrentSequence();
        bool                    PreviewSequence(size_t value);
        void                    StopPreviewingSequence();
        bool                    IsPreviewingSequence();
        bool                    ShouldHideVersionInfo();
        void                    SetHideVersionInfo(bool value);

        void Load();
        void Update();
        void CreateWindows();
        void ChangePresetSequence(size_t preset);

    private:
        GameState&              _gameState;

        ITitleSequencePlayer *  _sequencePlayer = nullptr;
        size_t                  _loadedTitleSequenceId = SIZE_MAX;
        size_t                  _currentSequence = SIZE_MAX;
        bool                    _hideVersionInfo = false;
        bool                    _previewingSequence = false;

        void TitleInitialise();
        bool TryLoadSequence(bool loadPreview = false);
    };
}

void title_load();
void title_create_windows();
void * title_get_sequence_player();
void title_sequence_change_preset(size_t preset);
bool title_should_hide_version_info();
void title_set_hide_version_info(bool value);
size_t title_get_config_sequence();
size_t title_get_current_sequence();
bool title_preview_sequence(size_t value);
void title_stop_previewing_sequence();
bool title_is_previewing_sequence();
void DrawOpenRCT2(rct_drawpixelinfo * dpi, sint32 x, sint32 y);

