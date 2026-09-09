/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_UTILS_ENUM_FLAGS_H
#define RSP_CORE_LIB_UTILS_ENUM_FLAGS_H

#include <concepts>
#include <initializer_list>
#include <type_traits>
#include <utility>

namespace rsp::utils {
namespace enum_flags_detail {

template <typename E>
concept ScopedEnum = std::is_scoped_enum_v<E>;

} // namespace enum_flags_detail

/**
 * \brief Wraps an enum class to add bitmask operations.
 *
 * \code
 * enum class MyEnum { A = 1, B = 2, C = 4 };
 * rsp::utils::EnumFlags<MyEnum> flags{MyEnum::A, MyEnum::B};
 * flags |= MyEnum::C;
 * \endcode
 *
 * \tparam E Enum type to use
 */
template <enum_flags_detail::ScopedEnum E>
class EnumFlags
{
    E _value{};

public:
    using underlying_type = std::underlying_type_t<E>;

    /**
     * \brief Constructs an EnumFlags object with no flags set.
     *
     * \param aValue The enumerator to initialize the flags with.
     */
    constexpr EnumFlags() noexcept = default;

    /**
     * \brief Constructs an EnumFlags object storing the specified flag.
     */
    constexpr EnumFlags(E aValue) noexcept
        : _value{aValue}
    {}

    /**
     * \brief Constructs an EnumFlags object initialized with the supplied flags
     *        combined using the bitwise OR operator.
     */
    constexpr EnumFlags(std::initializer_list<E> aFlags) noexcept
    {
        for (E flag : aFlags) {
            _value = static_cast<E>(std::to_underlying(_value) | std::to_underlying(flag));
        }
    }

    /**
     * \brief Builds a value from a raw integer bit pattern.
     */
    template <std::integral I>
    constexpr explicit EnumFlags(I aValue) noexcept
        : _value{static_cast<E>(aValue)}
    {}

    /**
     * \brief Implicit conversion to the wrapped enum type.
     *
     * \return E
     */
    [[nodiscard]] constexpr operator E() const noexcept
    {
        return _value;
    }

    /**
     * \brief Explicit conversion to the underlying integer type.
     *
     * \return underlying_type
     */
    [[nodiscard]] constexpr explicit operator underlying_type() const noexcept
    {
        return std::to_underlying(_value);
    }

    /**
     * \brief Test if any bit is set.
     *
     * \return true if any bit is set, false otherwise.
     */
    [[nodiscard]] constexpr explicit operator bool() const noexcept
    {
        return std::to_underlying(_value) != 0;
    }

    /**
     * \brief Bitwise AND operator.
     *
     * \param aValue The flag to AND with.
     * \return The result of the bitwise AND operation.
     */
    [[nodiscard]] constexpr EnumFlags<E> operator&(E aValue) const noexcept
    {
        return static_cast<E>(std::to_underlying(_value) & std::to_underlying(aValue));
    }

    /**
     * \brief Bitwise AND assignment operator.
     *
     * \param aValue The flag to AND with.
     * \return The result of the bitwise AND assignment operation.
     */
    constexpr EnumFlags<E>& operator&=(E aValue) noexcept
    {
        _value = static_cast<E>(std::to_underlying(_value) & std::to_underlying(aValue));
        return *this;
    }

    /**
     * \brief Bitwise OR operator.
     *
     * \param aValue The flag to OR with.
     * \return The result of the bitwise OR operation.
     */
    [[nodiscard]] constexpr EnumFlags<E> operator|(E aValue) const noexcept
    {
        return static_cast<E>(std::to_underlying(_value) | std::to_underlying(aValue));
    }

    /**
     * \brief Bitwise OR assignment operator.
     *
     * \param aValue The flag to OR with.
     * \return The result of the bitwise OR assignment operation.
     */
    constexpr EnumFlags<E>& operator|=(E aValue) noexcept
    {
        _value = static_cast<E>(std::to_underlying(_value) | std::to_underlying(aValue));
        return *this;
    }

    /**
     * \brief Bitwise XOR operator.
     *
     * \param aValue The flag to XOR with.
     * \return The result of the bitwise XOR operation.
     */
    [[nodiscard]] constexpr EnumFlags<E> operator^(E aValue) const noexcept
    {
        return static_cast<E>(std::to_underlying(_value) ^ std::to_underlying(aValue));
    }

    /**
     * \brief Bitwise XOR assignment operator.
     *
     * \param aValue The flag to XOR with.
     * \return The result of the bitwise XOR assignment operation.
     */
    constexpr EnumFlags<E>& operator^=(E aValue) noexcept
    {
        _value = static_cast<E>(std::to_underlying(_value) ^ std::to_underlying(aValue));
        return *this;
    }

    /**
     * \brief Bitwise NOT operator.
     *
     * \return The result of the bitwise NOT operation.
     */
    [[nodiscard]] constexpr EnumFlags<E> operator~() const noexcept
    {
        return static_cast<E>(~std::to_underlying(_value));
    }

    /**
     * \brief Equality operator.
     *
     * \param aValue The flag to compare with.
     * \return True if the flags are equal, false otherwise.
     */
    [[nodiscard]] constexpr bool operator==(E aValue) const noexcept
    {
        return std::to_underlying(_value) == std::to_underlying(aValue);
    }

    /**
     * \brief Inequality operator.
     *
     * \param aValue The flag to compare with.
     * \return True if the flags are not equal, false otherwise.
     */
    [[nodiscard]] constexpr bool operator!=(E aValue) const noexcept
    {
        return std::to_underlying(_value) != std::to_underlying(aValue);
    }

    /**
     * \brief True if every flag in aValue is set.
     *
     * \param aValue The flag(s) to check.
     * \return True if all specified flags are set, false otherwise.
     */
    [[nodiscard]] constexpr bool HasAll(E aValue) const noexcept
    {
        return (std::to_underlying(_value) & std::to_underlying(aValue)) == std::to_underlying(aValue);
    }

    /**
     * \brief Alias of HasAll.
     *
     * \param aValue The flag(s) to check.
     * \return True if all specified flags are set, false otherwise.
     */
    [[nodiscard]] constexpr bool IsSet(E aValue) const noexcept
    {
        return HasAll(aValue);
    }

    /**
     * \brief True if at least one flag in aValue is set.
     *
     * \param aValue The flag(s) to check.
     * \return True if any of the specified flags are set, false otherwise.
     */
    [[nodiscard]] constexpr bool HasAny(E aValue) const noexcept
    {
        return (std::to_underlying(_value) & std::to_underlying(aValue)) != 0;
    }
};

} // namespace rsp::utils

#endif // RSP_CORE_LIB_UTILS_ENUM_FLAGS_H
