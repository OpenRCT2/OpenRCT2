/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ParkSetParameterAction.h"

#include "../Diagnostic.h"
#include "../GameState.h"
#include "../core/EnumUtils.hpp"
#include "../ride/ShopItem.h"
#include "../ui/WindowManager.h"
#include "../world/Park.h"

namespace OpenRCT2::GameActions
{
    ParkSetParameterAction::ParkSetParameterAction(ParkParameter parameter, uint64_t value)
        : _parameter(parameter)
        , _value(value)
    {
    }

    void ParkSetParameterAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit("parameter", _parameter);
        visitor.Visit("value", _value);
    }

    uint16_t ParkSetParameterAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void ParkSetParameterAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_parameter) << DS_TAG(_value);
    }

    Result ParkSetParameterAction::Query(GameState_t& gameState) const
    {
        if (_parameter >= ParkParameter::Count)
        {
            LOG_ERROR("Invalid park parameter %d", _parameter);
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
        }

        auto res = Result();
        res.errorTitle = kErrorTitles[EnumValue(_parameter)];
        return res;
    }

    Result ParkSetParameterAction::Execute(GameState_t& gameState) const
    {
        auto& park = gameState.park;
        auto* windowMgr = Ui::GetWindowManager();

        switch (_parameter)
        {
            case ParkParameter::Close:
                if (park.flags & PARK_FLAGS_PARK_OPEN)
                {
                    park.flags &= ~PARK_FLAGS_PARK_OPEN;
                    windowMgr->InvalidateByClass(WindowClass::parkInformation);
                }
                break;
            case ParkParameter::Open:
                if (!(park.flags & PARK_FLAGS_PARK_OPEN))
                {
                    park.flags |= PARK_FLAGS_PARK_OPEN;
                    windowMgr->InvalidateByClass(WindowClass::parkInformation);
                }
                break;
            case ParkParameter::SamePriceInPark:
                park.samePriceThroughoutPark = _value;
                windowMgr->InvalidateByClass(WindowClass::ride);
                break;
            default:
                LOG_ERROR("Invalid park parameter %d", _parameter);
                return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
        }

        auto res = Result();
        res.errorTitle = kErrorTitles[EnumValue(_parameter)];
        return res;
    }
} // namespace OpenRCT2::GameActions
