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

#ifdef __cplusplus
extern "C"
{
#endif
#include "park_load_result_types.h"
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include <string>
#include "scenario/ScenarioRepository.h"

interface IObjectManager;
interface IObjectRepository;
interface IStream;

/**
 * Interface to import scenarios and saved games.
 */
interface IParkImporter
{
public:
    virtual ~IParkImporter() = default;
    virtual park_load_result * Load(const utf8 * path) abstract;
    virtual park_load_result * LoadSavedGame(const utf8 * path) abstract;
    virtual park_load_result * LoadScenario(const utf8 * path) abstract;
    virtual park_load_result * LoadFromStream(IStream * stream, bool isScenario) abstract;
    virtual void Import() abstract;
    virtual bool GetDetails(scenario_index_entry * dst) abstract;
};

namespace ParkImporter
{
    IParkImporter * Create(const std::string &hintPath);
    IParkImporter * CreateS4();
    IParkImporter * CreateS6(IObjectRepository * objectRepository, IObjectManager * objectManager);

    bool ExtensionIsRCT1(const std::string &extension);
    bool ExtensionIsScenario(const std::string &extension);
}

#endif

#ifdef __cplusplus
extern "C"
{
#endif
    void park_importer_load_from_stream(void * stream, const utf8 * hintPath);
    bool park_importer_extension_is_scenario(const utf8 * extension);

#ifdef __cplusplus
}
#endif
