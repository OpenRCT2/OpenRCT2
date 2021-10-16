/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
#include <memory>
#include <string.h>

class StringVariant
{
private:
    rct_string_id StringId = STR_NONE;
    std::string String;

public:
    StringVariant() = default;

    StringVariant(rct_string_id stringId)
        : StringId(stringId)
    {
    }

    StringVariant(const std::string& s)
        : String(s)
    {
    }

    StringVariant(std::string&& s)
        : String(std::move(s))
    {
    }

    StringVariant(const char* s)
        : String(s)
    {
    }

    const std::string* AsString() const
    {
        if (!String.empty())
        {
            return &String;
        }
        return {};
    }

    const rct_string_id* AsStringId() const
    {
        if (String.empty())
        {
            return &StringId;
        }
        return {};
    }

    rct_string_id GetStringId() const
    {
        return String.empty() ? StringId : STR_NONE;
    }
};

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

        Unknown = UINT16_MAX,
    };

#ifdef __WARN_SUGGEST_FINAL_METHODS__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wsuggest-final-methods"
#    pragma GCC diagnostic ignored "-Wsuggest-final-types"
#endif

    /**
     * Represents the result of a game action query or execution.
     */
    class Result
    {
    public:
        using Ptr = std::unique_ptr<GameActions::Result>;

        GameActions::Status Error = GameActions::Status::Ok;
        StringVariant ErrorTitle;
        StringVariant ErrorMessage;
        std::array<uint8_t, 32> ErrorMessageArgs{};
        CoordsXYZ Position = { LOCATION_NULL, LOCATION_NULL, LOCATION_NULL };
        money32 Cost = 0;
        ExpenditureType Expenditure = ExpenditureType::Count;
        std::any ResultData;

        Result() = default;
        Result(GameActions::Status error, rct_string_id message);
        Result(GameActions::Status error, rct_string_id title, rct_string_id message);
        Result(GameActions::Status error, rct_string_id title, rct_string_id message, uint8_t* args);
        Result(const GameActions::Result&) = delete;
        virtual ~Result(){};

        std::string GetErrorTitle() const;
        std::string GetErrorMessage() const;

        // It is recommended to use strong types since a type alias such as 'using MyType = uint32_t'
        // is still just uint32_t, this guarantees the data is associated with the correct type.
        template<typename T> void SetData(const T&& data)
        {
            ResultData = std::forward<const T&&>(data);
        }

        // This function will throw std::bad_any_cast if the type mismatches.
        template<typename T> T GetData() const
        {
            return std::any_cast<T>(ResultData);
        }
    };

#ifdef __WARN_SUGGEST_FINAL_METHODS__
#    pragma GCC diagnostic pop
#endif

    class ConstructClearResult final : public Result
    {
    public:
        uint8_t GroundFlags{ 0 };
    };

} // namespace GameActions
