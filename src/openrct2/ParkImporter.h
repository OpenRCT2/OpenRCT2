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

#include "common.h"

#include "object/Object.h"

enum PARK_LOAD_ERROR
{
    PARK_LOAD_ERROR_OK,
    PARK_LOAD_ERROR_MISSING_OBJECTS,
    PARK_LOAD_ERROR_INVALID_EXTENSION,
    PARK_LOAD_ERROR_UNSUPPORTED_RCTC_FLAG,
    PARK_LOAD_ERROR_UNKNOWN = 255
};

#include <memory>
#include <string>
#include <vector>
#include "core/String.hpp"

interface IObjectManager;
interface IObjectRepository;
interface IStream;
struct scenario_index_entry;

struct ParkLoadResult final
{
public:
    std::vector<rct_object_entry> const RequiredObjects;

    explicit ParkLoadResult(std::vector<rct_object_entry>&& requiredObjects)
        : RequiredObjects(requiredObjects)
    {
    }
};

/**
 * Interface to import scenarios and saved games.
 */
interface IParkImporter
{
public:
    virtual ~IParkImporter() = default;

    virtual ParkLoadResult  Load(const utf8 * path) abstract;
    virtual ParkLoadResult  LoadSavedGame(const utf8 * path, bool skipObjectCheck = false) abstract;
    virtual ParkLoadResult  LoadScenario(const utf8 * path, bool skipObjectCheck = false) abstract;
    virtual ParkLoadResult  LoadFromStream(IStream * stream,
                                           bool isScenario,
                                           bool skipObjectCheck = false,
                                           const utf8 * path = String::Empty) abstract;

    virtual void Import() abstract;
    virtual bool GetDetails(scenario_index_entry * dst) abstract;
};

namespace ParkImporter
{
    std::unique_ptr<IParkImporter> Create(const std::string &hintPath);
    std::unique_ptr<IParkImporter> CreateS4();
    std::unique_ptr<IParkImporter> CreateS6(std::shared_ptr<IObjectRepository> objectRepository, std::shared_ptr<IObjectManager> objectManager);

    bool ExtensionIsRCT1(const std::string &extension);
    bool ExtensionIsScenario(const std::string &extension);
}

class ObjectLoadException : public std::exception
{
public:
    std::vector<rct_object_entry> const MissingObjects;

    explicit ObjectLoadException(std::vector<rct_object_entry>&& missingObjects)
        : MissingObjects(missingObjects)
    {
    }
};

class UnsupportedRCTCFlagException : public std::exception
{
public:
    uint8 const Flag;

    explicit UnsupportedRCTCFlagException(uint8 flag)
        : Flag(flag)
    {
    }
};
