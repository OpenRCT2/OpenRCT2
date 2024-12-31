/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "core/DataSerialiser.h"

#include <memory>
#include <set>
#include <string>

struct GameStateSnapshot_t;

struct GameStateSpriteChange
{
    enum
    {
        REMOVED,
        ADDED,
        MODIFIED,
        EQUAL
    };

    struct Diff
    {
        size_t offset;
        size_t length;
        const char* structname;
        const char* fieldname;
        uint64_t valueA;
        uint64_t valueB;
    };

    uint8_t changeType;
    EntityType entityType;
    uint32_t spriteIndex;

    std::vector<Diff> diffs;
};

struct GameStateCompareData
{
    uint32_t tickLeft;
    uint32_t tickRight;
    uint32_t srand0Left;
    uint32_t srand0Right;
    std::vector<GameStateSpriteChange> spriteChanges;
};

/*
 * Interface to create and capture game states. It only allows one to have 32 active snapshots
 * the oldest snapshot will be removed from the buffer. Never store the snapshot pointer
 * as it may become invalid at any time when a snapshot is created, rather Link the snapshot
 * to a specific tick which can be obtained by that later again assuming its still valid.
 */
struct IGameStateSnapshots
{
    virtual ~IGameStateSnapshots() = default;

    /*
     * Removes all existing entries and starts over.
     */
    virtual void Reset() = 0;

    /*
     * Creates a new empty snapshot, oldest snapshot will be removed.
     */
    virtual GameStateSnapshot_t& CreateSnapshot() = 0;

    /*
     * Links the snapshot to a specific game tick.
     */
    virtual void LinkSnapshot(GameStateSnapshot_t& snapshot, uint32_t tick, uint32_t srand0) = 0;

    /*
     * This will fill the snapshot with the current game state in a compact form.
     */
    virtual void Capture(GameStateSnapshot_t& snapshot) = 0;

    /*
     * Returns the snapshot for a given tick in the history, nullptr if not found.
     */
    virtual const GameStateSnapshot_t* GetLinkedSnapshot(uint32_t tick) const = 0;

    /*
     * Serialisation of GameStateSnapshot_t
     */
    virtual void SerialiseSnapshot(GameStateSnapshot_t& snapshot, DataSerialiser& serialiser) const = 0;

    /*
     * Compares two states resulting GameStateCompareData with all mismatches stored.
     */
    virtual GameStateCompareData Compare(const GameStateSnapshot_t& base, const GameStateSnapshot_t& cmp) const = 0;

    /*
     * Writes the GameStateCompareData into the specified file as readable text.
     */
    virtual bool LogCompareDataToFile(const std::string& fileName, const GameStateCompareData& cmpData) const = 0;

    /*
     * Generates a string of readable text from GameStateCompareData
     */
    virtual std::string GetCompareDataText(const GameStateCompareData& cmpData) const = 0;
};

[[nodiscard]] std::unique_ptr<IGameStateSnapshots> CreateGameStateSnapshots();
