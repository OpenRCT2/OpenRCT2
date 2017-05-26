#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

typedef struct TitleCommand
{
    uint8 Type;
    union {
        uint8 SaveIndex;    // LOAD (this index is internal only)
        struct              // LOCATION
        {
            uint8 X;
            uint8 Y;
        };
        uint8 Rotations;    // ROTATE (counter-clockwise)
        uint8 Zoom;         // ZOOM
        uint8 Speed;        // SPEED
        uint8 Seconds;      // WAIT
    };
} TitleCommand;

typedef struct TitleSequence
{
    const utf8 * Name;
    const utf8 * Path;

    size_t         NumCommands;
    TitleCommand * Commands;

    size_t   NumSaves;
    utf8 * * Saves;

    bool IsZip;
} TitleSequence;

typedef struct TitleSequenceParkHandle
{
    const utf8 *    HintPath;
    void *          Stream;
} TitleSequenceParkHandle;

enum TITLE_SCRIPT
{
    TITLE_SCRIPT_UNDEFINED = 0xFF,
    TITLE_SCRIPT_WAIT = 0,
    TITLE_SCRIPT_LOADMM,
    TITLE_SCRIPT_LOCATION,
    TITLE_SCRIPT_ROTATE,
    TITLE_SCRIPT_ZOOM,
    TITLE_SCRIPT_RESTART,
    TITLE_SCRIPT_LOAD,
    TITLE_SCRIPT_END,
    TITLE_SCRIPT_SPEED,
    TITLE_SCRIPT_LOOP,
    TITLE_SCRIPT_ENDLOOP,
    TITLE_SCRIPT_LOADRCT1,
};

#ifdef __cplusplus
    constexpr const utf8 *  TITLE_SEQUENCE_EXTENSION = ".parkseq";
    constexpr uint8         SAVE_INDEX_INVALID = UINT8_MAX;
#else
    #define                 SAVE_INDEX_INVALID   UINT8_MAX
#endif

#ifdef __cplusplus
extern "C"
{
#endif
    TitleSequence * CreateTitleSequence();
    TitleSequence * LoadTitleSequence(const utf8 * path);
    void FreeTitleSequence(TitleSequence * seq);

    TitleSequenceParkHandle * TitleSequenceGetParkHandle(TitleSequence * seq, size_t index);

    /**
     * Close a title sequence park handle.
     * The pointer to the handle is invalid after calling this function.
     */
    void TitleSequenceCloseParkHandle(TitleSequenceParkHandle * handle);
    bool TileSequenceSave(TitleSequence * seq);
    bool TileSequenceAddPark(TitleSequence * seq, const utf8 * path, const utf8 * name);
    bool TileSequenceRenamePark(TitleSequence * seq, size_t index, const utf8 * name);
    bool TitleSequenceRemovePark(TitleSequence * seq, size_t index);

    bool TitleSequenceIsLoadCommand(const TitleCommand * command);
#ifdef __cplusplus
}
#endif
