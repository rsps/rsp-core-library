/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */


#ifndef INCLUDE_UTILS_ENUMFLAGS_H_
#define INCLUDE_UTILS_ENUMFLAGS_H_

#include <cstdint>
#include <type_traits>

namespace rsp::utils {

/**
 * \brief Template class to add bit operations to any enum class with a given underlying type.
 * \tparam T Enum type to use
 * \tparam Template validation check that T is of type enum.
 */
template<typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
class EnumFlags
{
    T mValue;
public:
    using utype = typename std::underlying_type<T>::type;

    /**
     * \brief Default constructor, that assigns the value o to the enum storage.
     */
    constexpr EnumFlags() : mValue(static_cast<T>(0)) {}

    /**
     * \brief Template contructor that tries to convert other types to this enum type.
     * \tparam N Argument type. Could be int, uint, char or any other native type.
     * \param aValue The value to assign to the internal enum storage.
     */
    template <typename N>
    constexpr EnumFlags(N aValue) : mValue(static_cast<T>(aValue)) {}

    /**
     * \brief Operator to get the enum value.
     */
    constexpr operator T() const
    {
        return mValue;
    }

    /**
     * \brief Operator to the the enum value converted to its underlying integer type.
     */
    constexpr operator utype() const
    {
        return static_cast<utype>(mValue);
    }

    /**
     * \brief Operator to return true if the internal enum has any value other than 0
     */
    constexpr explicit operator bool() const
    {
        return static_cast<utype>(mValue) != 0;
    }

    /**
     * \brief Bitwise AND operation
     * \param aValue rhs value for bitwise and operation
     * \return Result of AND'ing this with aValue
     */
    constexpr EnumFlags<T> operator&(T aValue)
    {
        return static_cast<T>(
            static_cast<utype>(mValue) &
            static_cast<utype>(aValue));
    }

    /**
     * \brief Bitwise AND assignment operator
     * \param aValue rhs value to AND with this
     * \return Returns the new value of this.
     */
    constexpr EnumFlags<T>& operator&=(T aValue)
    {
        mValue = static_cast<T>(
            static_cast<utype>(mValue) &
            static_cast<utype>(aValue));
        return *this;
    }

    /**
     * \brief Bitwise OR operation
     * \param aValue rhs value for bitwise or operation
     * \return Result of OR'ing this with aValue.
     */
    constexpr EnumFlags<T> operator|(T aValue)
    {
        return static_cast<T>(
            static_cast<utype>(mValue) |
            static_cast<utype>(aValue));
    }

    /**
     * \brief Bitwise OR assignment operator
     * \param aValue rhs value to OR with this
     * \return Returns new value of this
     */
    constexpr EnumFlags<T>& operator|=(T aValue)
    {
        mValue = static_cast<T>(
            static_cast<utype>(mValue) |
            static_cast<utype>(aValue));
        return *this;
    }

    /**
     * \brief Equality operator
     * \param aValue Other value of same enum type
     * \return bool True if values are equal
     */
    constexpr bool operator==(T aValue)
    {
        return static_cast<utype>(mValue) == static_cast<utype>(aValue);
    }

    /**
     * \brief Inequality operator
     * \param aValue Other value of same enum type
     * \return bool True if values are not equal
     */
    constexpr bool operator!=(T aValue)
    {
        return static_cast<utype>(mValue) != static_cast<utype>(aValue);
    }

    /**
     * \brief Bitwise NOT operator
     * \return The result of inverting all bits in this.
     */
    constexpr EnumFlags<T> operator~()
    {
        return static_cast<T>(
            ~static_cast<utype>(mValue));
    }

} __attribute__((packed));


/**
 * \brief Global enum equality operator
 * \tparam T Enum type
 * \tparam Check that T is of type enum
 * \param lhs Left hand side enum value
 * \param rhs Right hand side enum value
 * \return bool True if the two enums have the same content
 */
template <class T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
inline bool operator==(T lhs, T rhs)
{
    return (
        static_cast<typename std::underlying_type<T>::type>(lhs) ==
        static_cast<typename std::underlying_type<T>::type>(rhs));
}

/**
 * \brief Global enum inequality operator
 * \tparam T Enum type
 * \tparam Check that T is of type enum
 * \param lhs Left hand side enum value
 * \param rhs Right hand side enum value
 * \return bool True if the two enums doest NOT have the same content
 */
template <class T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
inline bool operator!=(T lhs, T rhs)
{
    return (
        static_cast<typename std::underlying_type<T>::type>(lhs) !=
        static_cast<typename std::underlying_type<T>::type>(rhs));
}

/**
 * \brief Global bitwise AND operator
 * \tparam T Enum type
 * \tparam Check that T is of type enum
 * \param lhs Left hand side enum value
 * \param rhs Right hand side enum value
 * \return Result of AND'ing the bit patterns of the two enums.
 */
template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
constexpr T operator&(T lhs, T rhs)
{
    return static_cast<T>(
        static_cast<typename std::underlying_type<T>::type>(lhs) &
        static_cast<typename std::underlying_type<T>::type>(rhs));
}

/**
 * \brief Global bitwise OR operator
 * \tparam T Enum type
 * \tparam Check that T is of type enum
 * \param lhs Left hand side enum value
 * \param rhs Right hand side enum value
 * \return Result of OR'ing the bit patterns of the two enums.
 */
template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
constexpr T operator|(T lhs, T rhs)
{
    return static_cast<T>(
        static_cast<typename std::underlying_type<T>::type>(lhs) |
        static_cast<typename std::underlying_type<T>::type>(rhs));
}

/**
 * \brief Global bitwise NOT operator
 * \tparam T Enum type
 * \tparam Check that T is of type enum
 * \param rhs Enum value to invert
 * \return Result of inverting all bits in the given value.
 */
template <typename T, typename = typename std::enable_if<std::is_enum<T>::value, T>::type>
constexpr T operator~(T rhs)
{
    return static_cast<T>(
        ~static_cast<typename std::underlying_type<T>::type>(rhs));
}

} // namespace rsp::utils

#endif /* INCLUDE_UTILS_ENUMFLAGS_H_ */
