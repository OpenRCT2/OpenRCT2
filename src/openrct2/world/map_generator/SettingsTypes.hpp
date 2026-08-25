/*****************************************************************************
 * Copyright (c) 2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <algorithm>
#include <exception>
#include <optional>
#include <string>

namespace OpenRCT2::World::MapGenerator
{
    enum Change
    {
        increment,
        decrement,
        set,
    };

    template<typename T, T low, T high, T step = 1>
    class NumericSetting
    {
    private:
        T value_;

    public:
        NumericSetting() = default;
        NumericSetting(T value)
        {
            set(value);
        }

        void set(T value)
        {
            value_ = std::clamp(value, low, high);
        }

        const T& get() const
        {
            return value_;
        }

        void increment()
        {
            set(value_ + step);
        }
        void decrement()
        {
            set(value_ - step);
        }

        operator const T&() const
        {
            return value_;
        }

        NumericSetting& operator=(T val)
        {
            set(val);
            return *this;
        }

        NumericSetting& operator++()
        {
            increment();
            return *this;
        }

        NumericSetting& operator--()
        {
            decrement();
            return *this;
        }

        void change(const Change& change, std::optional<T> maybeValue)
        {
            switch (change)
            {
                case Change::increment:
                    increment();
                    break;
                case Change::decrement:
                    decrement();
                    break;
                case Change::set:
                    if (maybeValue.has_value())
                    {
                        set(maybeValue.value());
                    }
                    break;
            }
        }
    };

    class SettingSerdeException : public std::exception
    {
    private:
        std::string what_;

    public:
        template<typename... Args>
        explicit SettingSerdeException(Args&&... args)
        {
            std::ostringstream oss;
            (oss << ... << std::forward<Args>(args));
            what_ = oss.str();
        }

        const char* what() const noexcept override
        {
            return what_.c_str();
        }
    };

} // namespace OpenRCT2::World::MapGenerator
