/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2026 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Kim Lindberg Schwaner
 */

#ifndef RSP_CORE_LIB_UTILS_VERSION_NUMBER_H
#define RSP_CORE_LIB_UTILS_VERSION_NUMBER_H

#include <array>
#include <charconv>
#include <limits>
#include <string_view>
#include <string>
#include <system_error>
#include <tuple>

namespace rsp::utils {

/**
 * \brief Utility class representing a SemVer version number of the form <major>.<minor>.<patch>
 *
 * \note Currently only supports the core version number, without pre-release or build metadata.
 */
class VersionNumber
{
public:
    constexpr VersionNumber() = default;

    constexpr explicit VersionNumber(unsigned major, unsigned minor, unsigned patch)
        : _major{major}, _minor{minor}, _patch{patch}
    {}

    [[nodiscard]] constexpr unsigned Major() const { return _major; }
    [[nodiscard]] constexpr unsigned Minor() const { return _minor; }
    [[nodiscard]] constexpr unsigned Patch() const { return _patch; }

    [[nodiscard]] constexpr std::string ToString() const
    {
        // Use std::to_chars() to keep it constexpr
        auto toString = [](unsigned value) -> std::string {
            std::array<char, std::numeric_limits<unsigned>::digits10 + 1> buf{};
            auto [ptr, ec] = std::to_chars(buf.begin(), buf.end(), value);
            if (ec != std::errc{}) // Should not happen: buffer is large enough to hold any unsigned value
                throw std::runtime_error{std::make_error_code(ec).message()};
            return std::string{buf.data(), static_cast<std::size_t>(ptr - buf.data())};
        };

        return toString(_major) + "." + toString(_minor) + "." + toString(_patch);
    }

    [[nodiscard]] static constexpr VersionNumber Parse(std::string_view str)
    {
        // Use std::from_chars() to keep it constexpr
        auto parseUnsigned = [](std::string_view s) -> std::tuple<unsigned, std::string_view> {
            unsigned value = 0;
            auto [ptr, ec] = std::from_chars(s.begin(), s.end(), value);
            if (ec != std::errc{})
                throw std::invalid_argument{std::make_error_code(ec).message()};
            return {value, {ptr, s.end()}};
        };

        // Strip pre-release (-) and build metadata (+)
        auto end = str.find_first_of("-+");
        if (end != std::string_view::npos)
            str = str.substr(0, end);

        auto [major, rest1] = parseUnsigned(str);
        if (rest1.empty() || rest1.front() != '.')
            throw std::invalid_argument{"Invalid version string: missing minor version"};
        auto [minor, rest2] = parseUnsigned(rest1.substr(1));
        if (rest2.empty() || rest2.front() != '.')
            throw std::invalid_argument{"Invalid version string: missing patch version"};
        auto [patch, _] = parseUnsigned(rest2.substr(1));

        return VersionNumber{major, minor, patch};
    }

    [[nodiscard]] constexpr bool operator==(const VersionNumber& other) const
    {
        return (_major == other._major) && (_minor == other._minor) && (_patch == other._patch);
    }

    [[nodiscard]] constexpr std::strong_ordering operator<=>(const VersionNumber& other) const
    {
        if (auto cmp = _major <=> other._major; cmp != 0) return cmp;
        if (auto cmp = _minor <=> other._minor; cmp != 0) return cmp;
        if (auto cmp = _patch <=> other._patch; cmp != 0) return cmp;
        return std::strong_ordering::equal;
    }

private:
    unsigned _major{0};
    unsigned _minor{0};
    unsigned _patch{0};
};

}; // namespace rsp::utils

#endif // RSP_CORE_LIB_UTILS_VERSION_NUMBER_H
