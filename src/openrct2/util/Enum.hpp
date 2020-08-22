/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <type_traits>

/**
 * Unary operators
 */

/**
 * Shortcut to cast a strong enum to its underlying type
 *
 * @param lhs A strong enum
 * @return Its underlying value
 */
template<class Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>>
constexpr std::underlying_type_t<Enum> operator+(const Enum& lhs)
{
    return static_cast<std::underlying_type_t<Enum>>(lhs);
}

/**
 * Shortcut to convert a strong enum flag to a boolean value
 *
 * @param lhs A strong enum
 * @return true if underlying value is 0
 */
template<class Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>>
constexpr std::underlying_type_t<Enum> operator!(const Enum& lhs)
{
    // underlying types of enum classes must be integral
    return static_cast<std::underlying_type_t<Enum>>(lhs) == 0;
}

/**
 * Pre-increment operator ++Enum, allowing use of for loops to traverse enum values
 *
 * @param lhs A strong enum
 * @return lhs + 1
 */
template<class Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>> constexpr Enum& operator++(Enum& lhs)
{
    lhs = static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(lhs) + 1);
    return lhs;
}

/**
 * Post-increment operator Enum++, allowing use of for loops to traverse enum values
 *
 * @param lhs A strong enum
 * @return lhs
 */
template<class Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>> constexpr Enum operator++(Enum& lhs, int)
{
    Enum ret = lhs;
    ++lhs;
    return ret;
}

/**
 * Bitwise inversion for enum classes
 */
template<class Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>> constexpr Enum operator~(const Enum& lhs)
{
    return static_cast<Enum>(~static_cast<std::underlying_type_t<Enum>>(lhs));
}

/**
 * Binary operators
 */

/**
 * Bitwise OR for enum classes
 */
template<class Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>>
constexpr Enum operator|(const Enum& lhs, const Enum& rhs)
{
    return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(lhs) | static_cast<std::underlying_type_t<Enum>>(rhs));
}

/**
 * Augmented bitwise OR assignment for enum classes
 */
template<class Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>>
constexpr Enum& operator|=(Enum& lhs, const Enum& rhs)
{
    lhs = lhs | rhs;
    return lhs;
}

/**
 * Bitwise AND for enum classes
 */
template<class Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>>
constexpr Enum operator&(const Enum& lhs, const Enum& rhs)
{
    return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(lhs) & static_cast<std::underlying_type_t<Enum>>(rhs));
}

/**
 * Augmented bitwise AND assignment for enum classes
 */
template<class Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>>
constexpr Enum& operator&=(Enum& lhs, const Enum& rhs)
{
    lhs = lhs & rhs;
    return lhs;
}

/**
 * Bitwise XOR for enum classes
 */
template<class Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>>
constexpr Enum operator^(const Enum& lhs, const Enum& rhs)
{
    return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(lhs) ^ static_cast<std::underlying_type_t<Enum>>(rhs));
}

/**
 * Augmented bitwise XOR assignment for enum classes
 */
template<class Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>>
constexpr Enum& operator^=(Enum& lhs, const Enum& rhs)
{
    lhs = lhs ^ rhs;
    return lhs;
}

/**
 * Left bit shift for enum classes and an integral type
 *
 * @param lhs A strong enum
 * @param rhs An integral type
 * @return A strong enum
 */
template<
    class Enum, typename Integral, typename = std::enable_if_t<std::is_enum<Enum>::value>,
    typename = std::enable_if_t<std::is_integral<Integral>::value>>
constexpr Enum operator<<(const Enum& lhs, const Integral& rhs)
{
    return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(lhs) << rhs);
}

/**
 * Right bit shift for enum classes and an integral type
 *
 * @param lhs A strong enum
 * @param rhs An integral type
 * @return A strong enum
 */
template<
    class Enum, typename Integral, typename = std::enable_if_t<std::is_enum<Enum>::value>,
    typename = std::enable_if_t<std::is_integral<Integral>::value>>
constexpr Enum operator>>(const Enum& lhs, const Integral& rhs)
{
    return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(lhs) >> rhs);
}

/**
 * Can be used to convert an index enum to a flag, e.g. the RideMode enum
 *
 * @param lhs An integral type, usually 1
 * @param rhs A strong enum
 * @return An integral type
 */
template<
    class Enum, typename Integral, typename = std::enable_if_t<std::is_enum<Enum>::value>,
    typename = std::enable_if_t<std::is_integral<Integral>::value>>
constexpr Integral operator<<(const Integral& lhs, const Enum& rhs)
{
    return lhs << static_cast<std::underlying_type_t<Enum>>(rhs);
}

/**
 * Array wrappers
 */

/**
 * Provides a wrapper for a const array and allows indexing by strong enum
 * @tparam ValueType The type of array
 * @tparam Enumeration The strong enum to use as an index
 */
template<typename ValueType, typename Enum, typename Enable = void> class ConstEnumeratedArray
{
};

template<typename ValueType, typename Enum>
class ConstEnumeratedArray<ValueType, Enum, std::enable_if_t<std::is_enum<Enum>::value>>
{
    const ValueType* _value;

public:
    ConstEnumeratedArray(const ValueType* value)
        : _value(value)
    {
    }

    constexpr const ValueType& operator[](const Enum& index) const
    {
        return _value[+index];
    }
};

/**
 * Provides a wrapper for an array and allows indexing by strong enum
 * @tparam ValueType The type of array
 * @tparam Enumeration The strong enum to use as an index
 */
template<typename ValueType, typename Enum, typename Enable = void> class EnumeratedArray
{
};

template<typename ValueType, typename Enum> class EnumeratedArray<ValueType, Enum, std::enable_if_t<std::is_enum<Enum>::value>>
{
    ValueType* _value;

public:
    EnumeratedArray(ValueType* value)
        : _value(value)
    {
    }

    EnumeratedArray()
        : _value(nullptr)
    {
    }

    inline ValueType& operator[](const Enum& index) const
    {
        return _value[+index];
    }
};
