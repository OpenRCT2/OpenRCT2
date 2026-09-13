/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "core/StringTypes.h"
#include "object/ObjectList.h"

#include <memory>
#include <vector>

namespace OpenRCT2
{
    struct IObjectManager;
    struct IObjectRepository;
    struct IStream;
    struct GameState_t;
    struct ParkPreview;
} // namespace OpenRCT2

struct ScenarioIndexEntry;

struct ParkLoadResult final
{
public:
    OpenRCT2::ObjectList RequiredObjects;
    bool SemiCompatibleVersion{};
    uint32_t MinVersion{};
    uint32_t TargetVersion{};

    explicit ParkLoadResult(OpenRCT2::ObjectList&& requiredObjects)
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

    virtual ParkLoadResult Load(const u8string& path, bool skipObjectCheck) = 0;
    virtual ParkLoadResult LoadSavedGame(const u8string& path, bool skipObjectCheck = false) = 0;
    virtual ParkLoadResult LoadScenario(const u8string& path, bool skipObjectCheck = false) = 0;
    virtual ParkLoadResult LoadFromStream(
        OpenRCT2::IStream* stream, bool isScenario, bool skipObjectCheck = false, const u8string& path = {})
        = 0;

    virtual void Import(OpenRCT2::GameState_t& gameState) = 0;
    virtual bool PopulateIndexEntry(ScenarioIndexEntry* dst) = 0;
    virtual OpenRCT2::ParkPreview GetParkPreview() = 0;
};

namespace OpenRCT2::ParkImporter
{
    [[nodiscard]] std::unique_ptr<IParkImporter> Create(const std::string& hintPath);
    [[nodiscard]] std::unique_ptr<IParkImporter> CreateS4();
    [[nodiscard]] std::unique_ptr<IParkImporter> CreateS6(IObjectRepository& objectRepository);
    [[nodiscard]] std::unique_ptr<IParkImporter> CreateParkFile(IObjectRepository& objectRepository);

    bool ExtensionIsOpenRCT2ParkFile(std::string_view extension);
    bool ExtensionIsRCT1(std::string_view extension);
    bool ExtensionIsScenario(std::string_view extension);
} // namespace OpenRCT2::ParkImporter

class ObjectLoadException : public std::exception
{
public:
    std::vector<OpenRCT2::ObjectEntryDescriptor> const MissingObjects;

    explicit ObjectLoadException(std::vector<OpenRCT2::ObjectEntryDescriptor>&& missingObjects)
        : MissingObjects(std::move(missingObjects))
    {
    }

    const char* what() const noexcept override
    {
        return "Missing objects";
    }
};

class UnsupportedRideTypeException : public std::exception
{
public:
    OpenRCT2::ObjectEntryIndex const Type;

    explicit UnsupportedRideTypeException(OpenRCT2::ObjectEntryIndex type)
        : Type(type)
    {
    }

    const char* what() const noexcept override
    {
        return "Invalid ride type";
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

    const char* what() const noexcept override
    {
        return "Unexpected version";
    }
};
