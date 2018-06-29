/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

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
    uint8_t const Flag;

    explicit UnsupportedRCTCFlagException(uint8_t flag)
        : Flag(flag)
    {
    }
};
