/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

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

namespace OpenRCT2::GameActions
{
    /**
     * Common error codes for game actions.
     */
    enum class Status : uint16_t
    {
        ok,
        invalidParameters,
        disallowed,
        gamePaused,
        insufficientFunds,
        notInEditorMode,

        notOwned,
        tooLow,
        tooHigh,
        noClearance,
        itemAlreadyPlaced,

        notClosed,
        broken,

        noFreeElements,

        unknown = std::numeric_limits<std::underlying_type_t<Status>>::max(),
    };

    /**
     * Represents the result of a game action query or execution.
     */
    class Result final
    {
    public:
        using StringVariant = std::variant<std::string, StringId>;

        Status error = Status::ok;
        StringVariant errorTitle = kStringIdNone;
        StringVariant errorMessage = kStringIdNone;
        std::array<uint8_t, 32> errorMessageArgs{};
        CoordsXYZ position = { kLocationNull, kLocationNull, kLocationNull };
        money64 cost = 0;
        ExpenditureType expenditure = ExpenditureType::count;

#ifdef __ANDROID__
        // Any_cast throws a bad_any_cast exception on Android
        // To avoid this in the Android release, a shared void pointer is used to store the result data.
        std::shared_ptr<void> resultData;
#else
        // Other platforms still use Any as this provides type checks
        std::any resultData;
#endif

        Result() = default;
        Result(Status status, StringId title, StringId message, uint8_t* args = nullptr);

        std::string getErrorTitle() const;
        std::string getErrorMessage() const;

        // It is recommended to use strong types since a type alias such as 'using MyType = uint32_t'
        // is still just uint32_t, this guarantees the data is associated with the correct type.
        template<typename T>
        void setData(const T&& data)
        {
#ifdef __ANDROID__
            resultData = std::make_shared<T>(data);
#else
            resultData = std::forward<const T&&>(data);
#endif
        }

        template<typename T>
        T getData() const
        {
#ifdef __ANDROID__
            return *static_cast<T*>(resultData.get());
            ;
#else
            // This function will throw std::bad_any_cast if the type mismatches.
            return std::any_cast<T>(resultData);
#endif
        }
    };

} // namespace OpenRCT2::GameActions
