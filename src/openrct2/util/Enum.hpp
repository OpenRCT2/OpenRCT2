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

template<typename T, typename = void> struct IsFlagType : std::false_type
{
};

template<typename T> struct IsFlagType<T, decltype((void)T::Null, void())> : std::true_type
{
};

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
template<
    class Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>>
constexpr std::underlying_type_t<Enum> operator!(const Enum& lhs)
{
    static_assert(IsFlagType<Enum>::value, "Bitwise operators are only defined for enum classes with a Null member");


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
template<
    class Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>>
constexpr Enum operator~(const Enum& lhs)
{
    static_assert(IsFlagType<Enum>::value, "Bitwise operators are only defined for enum classes with a Null member");

    return static_cast<Enum>(~static_cast<std::underlying_type_t<Enum>>(lhs));
}

/**
 * Binary operators
 */

/**
 * Bitwise OR for enum classes
 */
template<
    class Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>>
constexpr Enum operator|(const Enum& lhs, const Enum& rhs)
{
    static_assert(IsFlagType<Enum>::value, "Bitwise operators are only defined for enum classes with a Null member");

    return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(lhs) | static_cast<std::underlying_type_t<Enum>>(rhs));
}

/**
 * Augmented bitwise OR assignment for enum classes
 */
template<
    class Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>>
constexpr Enum& operator|=(Enum& lhs, const Enum& rhs)
{
    static_assert(IsFlagType<Enum>::value, "Bitwise operators are only defined for enum classes with a Null member");

    lhs = lhs | rhs;
    return lhs;
}

/**
 * Bitwise AND for enum classes
 */
template<
    class Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>>
constexpr Enum operator&(const Enum& lhs, const Enum& rhs)
{
    static_assert(IsFlagType<Enum>::value, "Bitwise operators are only defined for enum classes with a Null member");

    return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(lhs) & static_cast<std::underlying_type_t<Enum>>(rhs));
}

/**
 * Augmented bitwise AND assignment for enum classes
 */
template<
    class Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>>
constexpr Enum& operator&=(Enum& lhs, const Enum& rhs)
{
    static_assert(IsFlagType<Enum>::value, "Bitwise operators are only defined for enum classes with a Null member");

    lhs = lhs & rhs;
    return lhs;
}

/**
 * Bitwise XOR for enum classes
 */
template<
    class Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>>
constexpr Enum operator^(const Enum& lhs, const Enum& rhs)
{
    static_assert(IsFlagType<Enum>::value, "Bitwise operators are only defined for enum classes with a Null member");

    return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(lhs) ^ static_cast<std::underlying_type_t<Enum>>(rhs));
}

/**
 * Augmented bitwise XOR assignment for enum classes
 */
template<
    class Enum, typename = std::enable_if_t<std::is_enum<Enum>::value>>
constexpr Enum& operator^=(Enum& lhs, const Enum& rhs)
{
    static_assert(IsFlagType<Enum>::value, "Bitwise operators are only defined for enum classes with a Null member");

    lhs = lhs ^ rhs;
    return lhs;
}

/**
 * Helper functions
 */

/**
 * Returns true if all types in the template parameter list are the same
 */
template<typename... Types> struct AreSameType;
template<typename First, typename Second, typename... Tail> struct AreSameType<First, Second, Tail...>
{
    static const bool value = std::is_same_v<First, Second> && AreSameType<Second, Tail...>::value;
};
template<typename First, typename Second> struct AreSameType<First, Second>
{
    static const bool value = std::is_same_v<First, Second>;
};

/**
 * Checks whether a given flag has any of the other given flags enabled
 */
template<typename Enum, typename... Enums> constexpr bool FlagHasAny(Enum flag, Enums... flags)
{
    static_assert(AreSameType<Enum, Enums...>::value, "Incompatible type passed to FlagHasAny");
    static_assert(sizeof...(Enums) > 2, "FlagHasAll must be called with at least three parameters");
    static_assert(IsFlagType<Enum>::value, "FlagHasAny is only defined for enum classes with a Null member");

    auto flagsCombined = (flags | ...);
    return (flag & flagsCombined) != Enum::Null;
};

/**
 * Checks whether a given flag has any flag enabled
 */
template<typename Enum> constexpr bool FlagHasAny(const Enum& flag)
{
    static_assert(IsFlagType<Enum>::value, "FlagHasAny is only defined for enum classes with a Null member");

    return flag != Enum::Null;
};

/**
 * Checks whether a given flag has any of the other given flags enabled
 */
template<typename Enum, typename... Enums> constexpr bool FlagHas(const Enum& flag, Enums... flags)
{
    static_assert(AreSameType<Enum, Enums...>::value, "Incompatible type passed to FlagHas");
    static_assert(IsFlagType<Enum>::value, "FlagHas is only defined for enum classes with a Null member");
    static_assert(sizeof...(Enums) > 0, "FlagHas must be called with at least two parameters");

    auto flagsCombined = (flags | ...);
    return (flag & flagsCombined) == flag;
};

/**
 * Checks whether a given flag has the other given flag enabled
 */
template<typename Enum, typename EnumOther> constexpr bool FlagHas(const Enum& flag, const EnumOther& other)
{
    static_assert(IsFlagType<Enum>::value, "FlagHas is only defined for enum classes with a Null member");

    return (flag & other) != Enum::Null;
};

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
