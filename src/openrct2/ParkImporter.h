/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "common.h"
#include "core/String.hpp"
#include "object/Object.h"

#include <memory>
#include <string>
#include <vector>

struct IObjectManager;
struct IObjectRepository;
namespace OpenRCT2
{
    struct IStream;
}

struct scenario_index_entry;

struct ParkLoadResult final
{
public:
    std::vector<rct_object_entry> RequiredObjects;

    explicit ParkLoadResult(std::vector<rct_object_entry>&& requiredObjects)
        : RequiredObjects(std::move(requiredObjects))
    {
    }
};

/**
 * Interface to import scenarios and saved games.
 */
struct IParkImporter
{
public:
    virtual ~IParkImporter() = default;

    virtual ParkLoadResult Load(const utf8* path) abstract;
    virtual ParkLoadResult LoadSavedGame(const utf8* path, bool skipObjectCheck = false) abstract;
    virtual ParkLoadResult LoadScenario(const utf8* path, bool skipObjectCheck = false) abstract;
    virtual ParkLoadResult LoadFromStream(
        OpenRCT2::IStream* stream, bool isScenario, bool skipObjectCheck = false, const utf8* path = String::Empty) abstract;

    virtual void Import() abstract;
    virtual bool GetDetails(scenario_index_entry* dst) abstract;
};

namespace ParkImporter
{
    std::unique_ptr<IParkImporter> Create(const std::string& hintPath);
    std::unique_ptr<IParkImporter> CreateS4();
    std::unique_ptr<IParkImporter> CreateS6(IObjectRepository& objectRepository);
    std::unique_ptr<IParkImporter> CreateParkFile(IObjectRepository& objectRepository);

    bool ExtensionIsRCT1(const std::string& extension);
    bool ExtensionIsScenario(const std::string& extension);
} // namespace ParkImporter

class ObjectLoadException : public std::exception
{
public:
    std::vector<rct_object_entry> const MissingObjects;

    explicit ObjectLoadException(std::vector<rct_object_entry>&& missingObjects)
        : MissingObjects(std::move(missingObjects))
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

class UnsupportedRideTypeException : public std::exception
{
public:
    ObjectEntryIndex const Type;

    explicit UnsupportedRideTypeException(ObjectEntryIndex type)
        : Type(type)
    {
    }
};
