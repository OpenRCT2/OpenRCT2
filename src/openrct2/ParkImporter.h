/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
#include "object/ObjectList.h"

#include <memory>
#include <string>
#include <vector>

struct IObjectManager;
struct IObjectRepository;

namespace OpenRCT2
{
    struct IStream;
}

struct ScenarioIndexEntry;

struct ParkLoadResult final
{
public:
    ObjectList RequiredObjects;
    bool SemiCompatibleVersion{};
    uint32_t MinVersion{};
    uint32_t TargetVersion{};

    explicit ParkLoadResult(ObjectList&& requiredObjects)
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
    virtual bool GetDetails(ScenarioIndexEntry* dst) abstract;
};

namespace ParkImporter
{
    [[nodiscard]] std::unique_ptr<IParkImporter> Create(const std::string& hintPath);
    [[nodiscard]] std::unique_ptr<IParkImporter> CreateS4();
    [[nodiscard]] std::unique_ptr<IParkImporter> CreateS6(IObjectRepository& objectRepository);
    [[nodiscard]] std::unique_ptr<IParkImporter> CreateParkFile(IObjectRepository& objectRepository);

    bool ExtensionIsOpenRCT2ParkFile(std::string_view extension);
    bool ExtensionIsRCT1(std::string_view extension);
    bool ExtensionIsScenario(std::string_view extension);
} // namespace ParkImporter

class ObjectLoadException : public std::exception
{
public:
    std::vector<ObjectEntryDescriptor> const MissingObjects;

    explicit ObjectLoadException(std::vector<ObjectEntryDescriptor>&& missingObjects)
        : MissingObjects(std::move(missingObjects))
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

class UnsupportedVersionException : public std::exception
{
public:
    uint32_t const MinVersion;
    uint32_t const TargetVersion;

    explicit UnsupportedVersionException(uint32_t minVersion, uint32_t targetVersion)
        : MinVersion(minVersion)
        , TargetVersion(targetVersion)
    {
    }
};
