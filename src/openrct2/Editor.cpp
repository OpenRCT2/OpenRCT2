/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Editor.h"

#include "Context.h"
#include "FileClassifier.h"
#include "Game.h"
#include "GameState.h"
#include "OpenRCT2.h"
#include "ParkImporter.h"
#include "actions/GameActionRunner.h"
#include "actions/ResultWithMessage.h"
#include "actions/park/LandBuyRightsAction.h"
#include "actions/park/LandSetRightsAction.h"
#include "audio/Audio.h"
#include "core/EnumUtils.hpp"
#include "core/Path.hpp"
#include "core/String.hpp"
#include "drawing/Drawing.h"
#include "entity/EntityList.h"
#include "entity/EntityRegistry.h"
#include "entity/Guest.h"
#include "entity/PatrolArea.h"
#include "entity/Staff.h"
#include "interface/WindowBase.h"
#include "localisation/LocalisationService.h"
#include "management/Finance.h"
#include "management/NewsItem.h"
#include "object/DefaultObjects.h"
#include "object/ObjectManager.h"
#include "object/ObjectRepository.h"
#include "peep/PeepAnimations.h"
#include "rct1/RCT1.h"
#include "scenario/Scenario.h"
#include "scenes/SceneManager.h"
#include "scenes/editor/EditorController.h"
#include "scripting/ScriptEngine.h"
#include "ui/WindowManager.h"
#include "windows/Intent.h"
#include "world/Entrance.h"
#include "world/Footpath.h"
#include "world/Map.h"
#include "world/MapLimits.h"
#include "world/Park.h"
#include "world/Scenery.h"
#include "world/Weather.h"

#include <array>
#include <cassert>
#include <vector>

using namespace OpenRCT2;

namespace OpenRCT2::Editor
{
    static std::array<std::vector<uint8_t>, EnumValue(ObjectType::count)> _editorSelectedObjectFlags;

    void ObjectListLoad()
    {
        auto* context = GetContext();
        context->OpenProgress(STR_LOADING_GENERIC);

        // Unload objects first, the repository is re-populated which owns the objects.
        auto& objectManager = context->GetObjectManager();
        objectManager.UnloadAll();

        // Scan objects if necessary
        const auto& localisationService = context->GetLocalisationService();
        auto& objectRepository = context->GetObjectRepository();
        objectRepository.LoadOrConstruct(localisationService.GetCurrentLanguage());

        Audio::LoadAudioObjects();

        // Reset loaded objects to just defaults
        // Load minimum required objects (like surface and edge)
        for (const auto& entry : kMinimumRequiredObjects)
        {
            objectManager.LoadObject(entry);
        }

        context->CloseProgress();
    }

    /**
     *
     *  rct2: 0x006AB9B8
     */
    std::pair<ObjectType, StringId> CheckObjectSelection()
    {
        constexpr std::pair<ObjectType, StringId> kBasicCheckPairs[] = {
            { ObjectType::ride, STR_AT_LEAST_ONE_RIDE_OBJECT_MUST_BE_SELECTED },
            { ObjectType::station, STR_AT_LEAST_ONE_STATION_OBJECT_MUST_BE_SELECTED },
            { ObjectType::terrainSurface, STR_AT_LEAST_ONE_TERRAIN_SURFACE_OBJECT_MUST_BE_SELECTED },
            { ObjectType::terrainEdge, STR_AT_LEAST_ONE_TERRAIN_EDGE_OBJECT_MUST_BE_SELECTED },
        };

        for (auto& pair : kBasicCheckPairs)
        {
            if (!Editor::CheckObjectGroupAtLeastOneSelected(pair.first))
            {
                return { pair.first, pair.second };
            }
        }

        // No checks beyond this point apply to the track designer or track designs manager.
        const bool isTrackDesignerManager = isInTrackDesignerOrManager();
        if (isTrackDesignerManager)
        {
            return { ObjectType::none, kStringIdNone };
        }

        if (!Editor::CheckObjectGroupAtLeastOneSurfaceSelected(false))
        {
            return { ObjectType::footpathSurface, STR_AT_LEAST_ONE_FOOTPATH_NON_QUEUE_SURFACE_OBJECT_MUST_BE_SELECTED };
        }
        if (!Editor::CheckObjectGroupAtLeastOneSurfaceSelected(true))
        {
            return { ObjectType::footpathSurface, STR_AT_LEAST_ONE_FOOTPATH_QUEUE_SURFACE_OBJECT_MUST_BE_SELECTED };
        }

        constexpr std::pair<ObjectType, StringId> kParkCheckPairs[] = {
            { ObjectType::footpathRailings, STR_AT_LEAST_ONE_FOOTPATH_RAILING_OBJECT_MUST_BE_SELECTED },
            { ObjectType::parkEntrance, STR_PARK_ENTRANCE_TYPE_MUST_BE_SELECTED },
            { ObjectType::water, STR_WATER_TYPE_MUST_BE_SELECTED },
            { ObjectType::peepNames, STR_AT_LEAST_ONE_PEEP_NAMES_OBJECT_MUST_BE_SELECTED },
        };

        for (auto& pair : kParkCheckPairs)
        {
            if (!Editor::CheckObjectGroupAtLeastOneSelected(pair.first))
            {
                return { pair.first, pair.second };
            }
        }

        using OpenRCT2::AnimationPeepType;
        constexpr std::pair<AnimationPeepType, StringId> kPeepCheckPairs[] = {
            { AnimationPeepType::guest, STR_AT_LEAST_ONE_GUEST_PEEP_ANIMATIONS_OBJECT_MUST_BE_SELECTED },
            { AnimationPeepType::handyman, STR_AT_LEAST_ONE_HANDYMAN_PEEP_ANIMATIONS_OBJECT_MUST_BE_SELECTED },
            { AnimationPeepType::mechanic, STR_AT_LEAST_ONE_MECHANIC_PEEP_ANIMATIONS_OBJECT_MUST_BE_SELECTED },
            { AnimationPeepType::security, STR_AT_LEAST_ONE_SECURITY_PEEP_ANIMATIONS_OBJECT_MUST_BE_SELECTED },
            { AnimationPeepType::entertainer, STR_AT_LEAST_ONE_ENTERTAINER_PEEP_ANIMATIONS_OBJECT_MUST_BE_SELECTED },
        };

        for (auto& pair : kPeepCheckPairs)
        {
            if (!Editor::CheckObjectGroupAtLeastOneOfPeepTypeSelected(EnumValue(pair.first)))
            {
                return { ObjectType::peepAnimations, pair.second };
            }
        }

        return { ObjectType::none, kStringIdNone };
    }

    /**
     *
     *  rct2: 0x0066FEAC
     */
    ResultWithMessage CheckPark()
    {
        auto& gameState = getGameState();
        auto& park = gameState.park;
        int32_t parkSize = Park::UpdateSize(park);
        if (parkSize == 0)
        {
            return { false, STR_PARK_MUST_OWN_SOME_LAND };
        }

        if (gameState.park.entrances.empty())
        {
            return { false, STR_NO_PARK_ENTRANCES };
        }

        for (const auto& parkEntrance : gameState.park.entrances)
        {
            int32_t direction = DirectionReverse(parkEntrance.direction);

            switch (FootpathIsConnectedToMapEdge(parkEntrance, direction, 0))
            {
                case FOOTPATH_SEARCH_NOT_FOUND:
                    return { false, STR_PARK_ENTRANCE_WRONG_DIRECTION_OR_NO_PATH };
                case FOOTPATH_SEARCH_INCOMPLETE:
                case FOOTPATH_SEARCH_TOO_COMPLEX:
                    return { false, STR_PARK_ENTRANCE_PATH_INCOMPLETE_OR_COMPLEX };
                case FOOTPATH_SEARCH_SUCCESS:
                    // Run the search again and unown the path
                    FootpathIsConnectedToMapEdge(parkEntrance, direction, (1 << 5));
                    break;
            }
        }

        if (gameState.peepSpawns.empty())
        {
            return { false, STR_PEEP_SPAWNS_NOT_SET };
        }

        return { true, kStringIdNone };
    }

    uint8_t GetSelectedObjectFlags(ObjectType objectType, size_t index)
    {
        uint8_t result = 0;
        auto& list = _editorSelectedObjectFlags[EnumValue(objectType)];
        if (list.size() > index)
        {
            result = list[index];
        }
        return result;
    }

    void ClearSelectedObject(ObjectType objectType, size_t index, uint32_t flags)
    {
        auto& list = _editorSelectedObjectFlags[EnumValue(objectType)];
        if (list.size() <= index)
        {
            list.resize(index + 1);
        }
        list[index] &= ~flags;
    }

    void SetSelectedObject(ObjectType objectType, size_t index, uint32_t flags)
    {
        if (index != kObjectEntryIndexNull)
        {
            assert(static_cast<size_t>(objectType) < getObjectEntryGroupCount(ObjectType::paths));
            auto& list = _editorSelectedObjectFlags[EnumValue(objectType)];
            if (list.size() <= index)
            {
                list.resize(index + 1);
            }
            list[index] |= flags;
        }
    }
} // namespace OpenRCT2::Editor
