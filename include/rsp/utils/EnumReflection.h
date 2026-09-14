/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2026 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Kim Lindberg Schwaner
 */

/*
 * This file provides a constexpr implementation of enum reflection for C++23.
 * Works by parsing the compiler-generated function signature (__PRETTY_FUNCTION__)
 * of a function templated on a candidate enumerator value, for every value in the
 * range [-128, 127]. Supported on GCC and Clang.
 */

#ifndef RSP_CORE_LIB_UTILS_ENUM_REFLECTION_H
#define RSP_CORE_LIB_UTILS_ENUM_REFLECTION_H

#include <array>
#include <concepts>
#include <cstddef>
#include <optional>
#include <string_view>
#include <type_traits>
#include <utility>

namespace rsp::utils {
namespace enum_reflection_detail {

template <typename E, E V>
consteval std::string_view RawEnumName()
{
    return __PRETTY_FUNCTION__;
}

// Concept satisfied by enumeration types.
template <typename E>
concept Enum = std::is_enum_v<E>;

// Detects whether I is a value that can legally be static_cast to E as a constant
// expression. Needed because, for an unscoped enum without a fixed underlying type,
// casting a value outside its representable range is undefined behaviour, which
// some compilers (Clang) reject outright rather than merely printing a cast.
template <typename E, int I>
concept ValidEnumValue = requires { typename std::integral_constant<E, static_cast<E>(I)>; };

// Recovers the enumerator name for value I of enum E by parsing the
// compiler-generated function signature, or returns an empty view if
// I does not name a declared enumerator.
template <typename E, int I>
consteval std::string_view EnumNameForValue()
{
    if constexpr (!ValidEnumValue<E, I>) {
        return {};
    }
    else {
        constexpr std::string_view cSignature = RawEnumName<E, static_cast<E>(I)>();
        constexpr std::string_view cMarker = "V = ";

        auto pos = cSignature.find(cMarker);
        if (pos == std::string_view::npos) {
            return {};
        }
        auto value = cSignature.substr(pos + cMarker.size());
        auto end = value.find_first_of("];");
        if (end != std::string_view::npos) {
            value = value.substr(0, end);
        }
        if (value.empty() || value.front() == '(') {
            return {}; // Compiler printed a cast expression, i.e. not a named enumerator.
        }
        auto scope = value.rfind("::");
        return (scope == std::string_view::npos) ? value : value.substr(scope + 2);
    }
}

inline constexpr int cRangeMin = -128;
inline constexpr int cRangeMax = 127;

// EnumNameForValue() returns a view into the __PRETTY_FUNCTION__ string literal,
// which is not null-terminated at the end of the name. Copy it into its own
// zero-initialized (hence null-terminated) storage so callers may safely use
// EnumName(...).data() as a C-string.
template <typename E, int I>
inline constexpr std::size_t cNameLength = EnumNameForValue<E, I>().size();

template <typename E, int I>
inline constexpr std::array<char, cNameLength<E, I> + 1> cNameStorage = [] {
    std::array<char, cNameLength<E, I> + 1> arr{};
    constexpr auto cName = EnumNameForValue<E, I>();
    for (std::size_t i = 0; i < cName.size(); ++i) {
        arr[i] = cName[i];
    }
    return arr;
}();

template <typename E, int I>
consteval std::string_view SafeEnumNameForValue()
{
    return std::string_view(cNameStorage<E, I>.data(), cNameLength<E, I>);
}

// Converting an out-of-range int to an enum without a fixed underlying type is
// undefined behaviour, so only cast once ValidEnumValue confirms it's well-defined;
// otherwise fall back to the always-valid zero-initialized value (never matched,
// since its paired name is empty).
template <typename E, int I>
consteval E ValueForIndex()
{
    if constexpr (ValidEnumValue<E, I>) {
        return static_cast<E>(I);
    }
    else {
        return E{};
    }
}

template <typename E, std::size_t... I>
consteval auto BuildEntries(std::index_sequence<I...>)
{
    return std::array<std::pair<E, std::string_view>, sizeof...(I)>{
            std::pair<E, std::string_view>{
                    ValueForIndex<E, static_cast<int>(I) + cRangeMin>(),
                    SafeEnumNameForValue<E, static_cast<int>(I) + cRangeMin>()}...};
}

template <typename E>
consteval auto AllEntries()
{
    return BuildEntries<E>(std::make_index_sequence<static_cast<std::size_t>(cRangeMax - cRangeMin + 1)>{});
}

template <typename E>
inline constexpr auto cEntries = AllEntries<E>();

} // namespace enum_reflection_detail

/**
 * \brief Get the number of enumerator values of E within the range [-128, 127].
 * \tparam E Enum type
 */
template <enum_reflection_detail::Enum E>
consteval std::size_t EnumCount()
{
    std::size_t count = 0;
    for (const auto& entry : enum_reflection_detail::cEntries<E>) {
        if (!entry.second.empty()) {
            ++count;
        }
    }
    return count;
}

/**
 * \brief Get the unqualified name of an enumerator value.
 * \tparam E Enum type
 * \param aValue Value to get the name of
 * \return The enumerator's name, or an empty view if aValue does not match any
 *         enumerator of E in the range [-128, 127].
 */
template <enum_reflection_detail::Enum E>
constexpr std::string_view EnumName(E aValue)
{
    for (const auto& entry : enum_reflection_detail::cEntries<E>) {
        if (entry.first == aValue && !entry.second.empty()) {
            return entry.second;
        }
    }
    return {};
}

/**
 * \brief Find the enumerator value of E with the given name.
 * \tparam E Enum type
 * \param aName Name to look up
 * \return The matching enumerator value, or std::nullopt if no enumerator
 *         of E has that name.
 */
template <enum_reflection_detail::Enum E>
constexpr std::optional<E> EnumCast(std::string_view aName)
{
    for (const auto& entry : enum_reflection_detail::cEntries<E>) {
        if (!entry.second.empty() && entry.second == aName) {
            return entry.first;
        }
    }
    return std::nullopt;
}

/**
 * \brief Get all enumerator values of E within the range [-128, 127].
 * \tparam E Enum type
 * \return An array of the declared enumerator values, in ascending order of their
 *         underlying integer value. Duplicate values (aliases) each appear once.
 *
 * Usage:
 * \code
 * for (auto value : rsp::utils::EnumValues<MyEnum>()) { ... }
 * \endcode
 */
template <enum_reflection_detail::Enum E>
consteval std::array<E, EnumCount<E>()> EnumValues()
{
    std::array<E, EnumCount<E>()> result{};
    std::size_t index = 0;
    for (const auto& entry : enum_reflection_detail::cEntries<E>) {
        if (!entry.second.empty()) {
            result[index++] = entry.first;
        }
    }
    return result;
}

/**
 * \brief Get the names of all enumerator values of E within the range [-128, 127].
 * \tparam E Enum type
 * \return An array of the enumerator names, in ascending order of their
 *         underlying integer value, matching the order of EnumValues<E>().
 *
 * Usage:
 * \code
 * for (auto name : rsp::utils::EnumNames<MyEnum>()) { ... }
 * \endcode
 */
template <enum_reflection_detail::Enum E>
consteval std::array<std::string_view, EnumCount<E>()> EnumNames()
{
    std::array<std::string_view, EnumCount<E>()> result{};
    std::size_t index = 0;
    for (const auto& entry : enum_reflection_detail::cEntries<E>) {
        if (!entry.second.empty()) {
            result[index++] = entry.second;
        }
    }
    return result;
}

/**
 * \brief Get all enumerator value/name pairs of E within the range [-128, 127].
 * \tparam E Enum type
 * \return An array of (value, name) pairs, in ascending order of their
 *         underlying integer value, matching the order of EnumValues<E>()
 *         and EnumNames<E>().
 *
 * Usage:
 * \code
 * for (auto& [value, name] : rsp::utils::EnumEntries<MyEnum>()) { ... }
 * \endcode
 */
template <enum_reflection_detail::Enum E>
consteval std::array<std::pair<E, std::string_view>, EnumCount<E>()> EnumEntries()
{
    std::array<std::pair<E, std::string_view>, EnumCount<E>()> result{};
    std::size_t index = 0;
    for (const auto& entry : enum_reflection_detail::cEntries<E>) {
        if (!entry.second.empty()) {
            result[index++] = entry;
        }
    }
    return result;
}

} // namespace rsp::utils

#endif // RSP_CORE_LIB_UTILS_ENUM_REFLECTION_H
