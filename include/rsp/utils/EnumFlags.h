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

#include <type_traits>
#include <utility>

namespace rsp::utils {
namespace enum_flags_detail {

template <typename E>
concept ScopedEnum = std::is_scoped_enum_v<E>;

/**
 * \brief True if E has an EnableFlags(E) overload reachable via ADL (see \ref RSP_FLAGS
 *        and \ref RSP_FLAGS_MEMBER). Gates the generic operators below to opted-in enums only.
 */
template <typename E>
concept BitmaskEnum = ScopedEnum<E> && requires(E aValue) {
    EnableFlags(aValue);
};

/**
 * \brief Bitwise operators for opted-in enums. Live here rather than as free functions
 *        so ADL can't find them for enums outside this namespace - \ref RSP_FLAGS and
 *        \ref RSP_FLAGS_MEMBER forward concrete, ADL-findable operators to these.
 */
template <BitmaskEnum E>
[[nodiscard]] constexpr E operator&(E aLhs, E aRhs) noexcept
{
    return static_cast<E>(std::to_underlying(aLhs) & std::to_underlying(aRhs));
}

template <BitmaskEnum E>
[[nodiscard]] constexpr E operator|(E aLhs, E aRhs) noexcept
{
    return static_cast<E>(std::to_underlying(aLhs) | std::to_underlying(aRhs));
}

template <BitmaskEnum E>
[[nodiscard]] constexpr E operator^(E aLhs, E aRhs) noexcept
{
    return static_cast<E>(std::to_underlying(aLhs) ^ std::to_underlying(aRhs));
}

template <BitmaskEnum E>
[[nodiscard]] constexpr E operator~(E aRhs) noexcept
{
    return static_cast<E>(~std::to_underlying(aRhs));
}

} // namespace enum_flags_detail

/**
 * \brief Wraps an enum class to add bitmask operations.
 * \tparam E Enum type to use
 */
template <enum_flags_detail::ScopedEnum E>
class EnumFlags
{
    E _value{};

public:
    using underlying_type = std::underlying_type_t<E>;

    constexpr EnumFlags() noexcept = default;

    /**
     * \brief Wraps an enumerator of E directly.
     */
    constexpr EnumFlags(E aValue) noexcept
        : _value{aValue}
    {}

    /**
     * \brief Builds a value from a raw integer bit pattern. Explicit since it bypasses
     *        the enum's usual named-value safety.
     */
    template <std::integral I>
    constexpr explicit EnumFlags(I aValue) noexcept
        : _value{static_cast<E>(aValue)}
    {}

    [[nodiscard]] constexpr operator E() const noexcept
    {
        return _value;
    }

    /**
     * \brief Explicit conversion to the underlying integer type.
     */
    [[nodiscard]] constexpr explicit operator underlying_type() const noexcept
    {
        return std::to_underlying(_value);
    }

    /**
     * \brief True if any bit is set.
     */
    [[nodiscard]] constexpr explicit operator bool() const noexcept
    {
        return std::to_underlying(_value) != 0;
    }

    [[nodiscard]] constexpr EnumFlags<E> operator&(E aValue) const noexcept
    {
        return static_cast<E>(std::to_underlying(_value) & std::to_underlying(aValue));
    }

    constexpr EnumFlags<E>& operator&=(E aValue) noexcept
    {
        _value = static_cast<E>(std::to_underlying(_value) & std::to_underlying(aValue));
        return *this;
    }

    [[nodiscard]] constexpr EnumFlags<E> operator|(E aValue) const noexcept
    {
        return static_cast<E>(std::to_underlying(_value) | std::to_underlying(aValue));
    }

    constexpr EnumFlags<E>& operator|=(E aValue) noexcept
    {
        _value = static_cast<E>(std::to_underlying(_value) | std::to_underlying(aValue));
        return *this;
    }

    [[nodiscard]] constexpr EnumFlags<E> operator^(E aValue) const noexcept
    {
        return static_cast<E>(std::to_underlying(_value) ^ std::to_underlying(aValue));
    }

    constexpr EnumFlags<E>& operator^=(E aValue) noexcept
    {
        _value = static_cast<E>(std::to_underlying(_value) ^ std::to_underlying(aValue));
        return *this;
    }

    [[nodiscard]] constexpr EnumFlags<E> operator~() const noexcept
    {
        return static_cast<E>(~std::to_underlying(_value));
    }

    [[nodiscard]] constexpr bool operator==(E aValue) const noexcept
    {
        return std::to_underlying(_value) == std::to_underlying(aValue);
    }

    [[nodiscard]] constexpr bool operator!=(E aValue) const noexcept
    {
        return std::to_underlying(_value) != std::to_underlying(aValue);
    }

    /**
     * \brief True if every flag in aValue is set.
     */
    [[nodiscard]] constexpr bool HasAll(E aValue) const noexcept
    {
        return (std::to_underlying(_value) & std::to_underlying(aValue)) == std::to_underlying(aValue);
    }

    /**
     * \brief Alias of HasAll.
     */
    [[nodiscard]] constexpr bool IsSet(E aValue) const noexcept
    {
        return HasAll(aValue);
    }

    /**
     * \brief True if at least one flag in aValue is set.
     */
    [[nodiscard]] constexpr bool HasAny(E aValue) const noexcept
    {
        return (std::to_underlying(_value) & std::to_underlying(aValue)) != 0;
    }
};

} // namespace rsp::utils

/**
 * \brief Opts EnumType into bitwise operators, e.g. `RSP_FLAGS(MyEnum);`.
 *
 * Use at namespace scope, right after the enum (or its enclosing class if
 * EnumType is nested), not inside a class or function body. Defines
 * EnableFlags(E) and the operators as free functions right there, so they are
 * found via ADL no matter what namespace EnumType lives in - a template defined
 * in rsp::utils would not be found by ADL for an enum declared in an unrelated
 * namespace.
 *
 * \code
 * enum class MyEnum { A = 1, B = 2 };
 * RSP_FLAGS(MyEnum);
 * MyEnum value = MyEnum::A | MyEnum::B;
 * \endcode
 */
#define RSP_FLAGS(EnumType)                                                           \
    consteval void EnableFlags(EnumType) noexcept                                     \
    {}                                                                                \
    [[nodiscard]] constexpr EnumType operator&(EnumType aLhs, EnumType aRhs) noexcept \
    {                                                                                 \
        return ::rsp::utils::enum_flags_detail::operator&(aLhs, aRhs);                \
    }                                                                                 \
    [[nodiscard]] constexpr EnumType operator|(EnumType aLhs, EnumType aRhs) noexcept \
    {                                                                                 \
        return ::rsp::utils::enum_flags_detail::operator|(aLhs, aRhs);                \
    }                                                                                 \
    [[nodiscard]] constexpr EnumType operator^(EnumType aLhs, EnumType aRhs) noexcept \
    {                                                                                 \
        return ::rsp::utils::enum_flags_detail::operator^(aLhs, aRhs);                \
    }                                                                                 \
    [[nodiscard]] constexpr EnumType operator~(EnumType aRhs) noexcept                \
    {                                                                                 \
        return ::rsp::utils::enum_flags_detail::operator~(aRhs);                      \
    }

/**
 * \brief Like \ref RSP_FLAGS, but for use inside the class body declaring EnumType.
 *
 * Declares EnableFlags and the operators as hidden friends, so they are found
 * via ADL wherever EnumType is used - including outside the class, without any
 * further imports.
 *
 * Caveat: a hidden friend's inline body is only complete after the class's
 * closing brace, so it can't be called from a constant expression evaluated
 * eagerly WHILE the class is still being parsed - e.g. a `static constexpr`
 * data member of the SAME class combining EnumType flags. Route through \ref
 * rsp::utils::EnumFlags in that one case instead, since it's a separate,
 * already-complete class template with no such ordering dependency:
 *
 * \code
 * class MyClass {
 *     enum class NestedEnum { A = 1, B = 2 };
 *     RSP_FLAGS_MEMBER(NestedEnum);
 *
 *     // Needs the EnumFlags<> workaround - NestedEnum isn't complete yet here.
 *     static constexpr NestedEnum value = rsp::utils::EnumFlags<NestedEnum>{NestedEnum::A} | NestedEnum::B;
 *
 *     NestedEnum GetValue() const noexcept; // Defined below, class is complete - plain `|` works fine.
 * };
 * MyClass::NestedEnum MyClass::GetValue() const noexcept { return NestedEnum::A | NestedEnum::B; }
 * \endcode
 */
#define RSP_FLAGS_MEMBER(EnumType)                                                           \
    friend consteval void EnableFlags(EnumType) noexcept                                     \
    {}                                                                                       \
    [[nodiscard]] friend constexpr EnumType operator&(EnumType aLhs, EnumType aRhs) noexcept \
    {                                                                                        \
        return ::rsp::utils::enum_flags_detail::operator&(aLhs, aRhs);                       \
    }                                                                                        \
    [[nodiscard]] friend constexpr EnumType operator|(EnumType aLhs, EnumType aRhs) noexcept \
    {                                                                                        \
        return ::rsp::utils::enum_flags_detail::operator|(aLhs, aRhs);                       \
    }                                                                                        \
    [[nodiscard]] friend constexpr EnumType operator^(EnumType aLhs, EnumType aRhs) noexcept \
    {                                                                                        \
        return ::rsp::utils::enum_flags_detail::operator^(aLhs, aRhs);                       \
    }                                                                                        \
    [[nodiscard]] friend constexpr EnumType operator~(EnumType aRhs) noexcept                \
    {                                                                                        \
        return ::rsp::utils::enum_flags_detail::operator~(aRhs);                             \
    }

#endif // RSP_CORE_LIB_UTILS_ENUM_FLAGS_H
