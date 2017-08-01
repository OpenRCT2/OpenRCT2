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

#ifdef __cplusplus
extern "C"
{
#endif
#include "../drawing/drawing.h"
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
interface ITitleSequencePlayer;

class TitleScreen final
{
public:
    ITitleSequencePlayer *  GetSequencePlayer();
    uint16                  GetCurrentSequence();
    void                    SetCurrentSequence(uint16 value);
    bool                    ShouldHideVersionInfo();
    void                    SetHideVersionInfo(bool value);

    TitleScreen();
    ~TitleScreen();

    void Load();
    void Update();
    void CreateWindows();
    void ChangeSequence(sint32 preset);

private:
    ITitleSequencePlayer *  _sequencePlayer = nullptr;
    uint16                  _loadedTitleSequenceId = UINT16_MAX;
    uint16                  _currentSequence = UINT16_MAX;
    bool                    _hideVersionInfo = false;

    void TitleInitialise();
    void TryLoadSequence();
};
#endif

#ifdef __cplusplus
extern "C"
{
#endif
    void title_load();
    void title_create_windows();
    void * title_get_sequence_player();
    void title_sequence_change_preset(sint32 preset);
    bool title_should_hide_version_info();
    void title_set_hide_version_info(bool value);
    uint16 title_get_current_sequence();
    void title_set_current_sequence(uint16 value);
    void DrawOpenRCT2(rct_drawpixelinfo *dpi, sint32 x, sint32 y);
#ifdef __cplusplus
}
#endif
