/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2025 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_PARSER_HELPERS_H
#define RSP_CORE_LIB_PARSER_HELPERS_H

#include <algorithm>
#include <charconv>
#include <ranges>
#include <string_view>

namespace rsp::network {

using namespace std::string_view_literals;

/**
 * \brief Whitespace characters to be ignored
 */
constexpr auto cWhitespaceCharacters = " \t\r\n"sv;

/**
 * \brief HTTP newline (CR+LF)
 */
constexpr auto cNewLine = "\r\n"sv;

/**
 * \brief HTTP empty line 2x(CR+LF)
 */
constexpr auto cHeaderEnd = "\r\n\r\n"sv;

/**
 * \brief delimiter between scheme and host
 */
constexpr auto cSchemeSuffix = "://"sv;

/**
 * \brief Transforms a range of chars into its lowercase equivalent.
 */
constexpr auto ascii_lowercase_transform = std::views::transform([](char const c) noexcept {
    return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
});

/**
 * \brief Returns whether lhs and rhs are equal, regardless of casing, assuming both are encoded in ASCII.
 * \param lhs
 * \param rhs
 * \return True if lhs and rhs are equal
 */
[[nodiscard]] constexpr bool equal_ascii_case_insensitive(std::string_view const lhs, std::string_view const rhs) noexcept {
    return std::ranges::equal(lhs | ascii_lowercase_transform, rhs | ascii_lowercase_transform);
}

/**
 * \brief Convert a string_view to the integral of type T
 * \tparam T of type std::integral
 * \param string string_view to convert
 * \param base Conversion base
 * \return optional<T>
 * \see std::from_chars
 */
template<std::integral T>
[[nodiscard]] std::optional<T> string_to_integral(std::string_view const string, int const base = 10)
{
    std::integral auto number_result = T{};
    if (std::from_chars(string.data(), string.data() + string.size(), number_result, base).ec == std::errc{}) {
        return number_result;
    }
    return {};
}

} // rsp::network

#endif //RSP_CORE_LIB_PARSER_HELPERS_H
