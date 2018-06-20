/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <memory>
#include "../common.h"

struct rct_object_entry;

struct scenario_highscore_entry
{
    utf8 *      fileName;
    utf8 *      name;
    money32     company_value;
    datetime64  timestamp;
};

struct scenario_index_entry
{
    utf8    path[MAX_PATH];
    uint64_t  timestamp;

    // Category / sequence
    uint8_t   category;
    uint8_t   source_game;
    int16_t  source_index;
    uint16_t  sc_id;

    // Objective
    uint8_t   objective_type;
    uint8_t   objective_arg_1;
    int32_t  objective_arg_2;
    int16_t  objective_arg_3;
    scenario_highscore_entry * highscore;
    
    utf8 internal_name[64]; // Untranslated name
    utf8 name[64];          // Translated name
    utf8 details[256];
};

namespace OpenRCT2
{
    interface IPlatformEnvironment;
}

interface IScenarioRepository
{
    virtual ~IScenarioRepository() = default;

    /**
     * Scans the scenario directories and grabs the metadata for all the scenarios.
     */
    virtual void Scan(int32_t language) abstract;

    virtual size_t GetCount() const abstract;
    virtual const scenario_index_entry * GetByIndex(size_t index) const  abstract;
    virtual const scenario_index_entry * GetByFilename(const utf8 * filename) const abstract;
	/**
	    * Does not return custom scenarios due to the fact that they may have the same name.
	    */
    virtual const scenario_index_entry * GetByInternalName(const utf8 * name) const abstract;
    virtual const scenario_index_entry * GetByPath(const utf8 * path) const abstract;

    virtual bool TryRecordHighscore(int32_t language, const utf8 * scenarioFileName, money32 companyValue, const utf8 * name) abstract;
};

std::unique_ptr<IScenarioRepository> CreateScenarioRepository(const std::shared_ptr<OpenRCT2::IPlatformEnvironment>& env);
IScenarioRepository * GetScenarioRepository();

void    scenario_repository_scan();
size_t  scenario_repository_get_count();
const   scenario_index_entry *scenario_repository_get_by_index(size_t index);
bool    scenario_repository_try_record_highscore(const utf8 * scenarioFileName, money32 companyValue, const utf8 * name);
void    scenario_translate(scenario_index_entry * scenarioEntry, const struct rct_object_entry * stexObjectEntry);
