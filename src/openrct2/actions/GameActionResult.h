/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#pragma once

#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../world/Location.hpp"

#include <any>
#include <array>
#include <cstdint>
#include <limits>
#include <memory>
#include <string>
#include <type_traits>
#include <variant>

namespace GameActions
{
    /**
     * Common error codes for game actions.
     */
    enum class Status : uint16_t
    {
        Ok,
        InvalidParameters,
        Disallowed,
        GamePaused,
        InsufficientFunds,
        NotInEditorMode,

        NotOwned,
        TooLow,
        TooHigh,
        NoClearance,
        ItemAlreadyPlaced,

        NotClosed,
        Broken,

        NoFreeElements,

        Unknown = std::numeric_limits<std::underlying_type_t<Status>>::max(),
    };

#ifdef __WARN_SUGGEST_FINAL_METHODS__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wsuggest-final-methods"
#    pragma GCC diagnostic ignored "-Wsuggest-final-types"
#endif

    /**
     * Represents the result of a game action query or execution.
     */
    class Result final
    {
    public:
        using StringVariant = std::variant<std::string, rct_string_id>;

        GameActions::Status Error = GameActions::Status::Ok;
        StringVariant ErrorTitle = STR_NONE;
        StringVariant ErrorMessage = STR_NONE;
        std::array<uint8_t, 32> ErrorMessageArgs{};
        CoordsXYZ Position = { LOCATION_NULL, LOCATION_NULL, LOCATION_NULL };
        money32 Cost = 0;
        ExpenditureType Expenditure = ExpenditureType::Count;
        std::shared_ptr<void> ResultData;

        Result() = default;
        Result(GameActions::Status error, rct_string_id title, rct_string_id message, uint8_t* args = nullptr);

        std::string GetErrorTitle() const;
        std::string GetErrorMessage() const;

        // It is recommended to use strong types since a type alias such as 'using MyType = uint32_t'
        // is still just uint32_t, this guarantees the data is associated with the correct type.
        template<typename T> void SetData(const T&& data)
        {
            ResultData = std::make_shared<T>(data);
        }

        template<typename T> T GetData() const
        {
            T* res = static_cast<T*>(ResultData.get());
            return *res;
        }
    };

#ifdef __WARN_SUGGEST_FINAL_METHODS__
#    pragma GCC diagnostic pop
#endif

} // namespace GameActions
