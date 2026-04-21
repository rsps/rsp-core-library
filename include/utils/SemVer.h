/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2026 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Kim Lindberg Schwaner
 */

#ifndef RSP_CORE_LIB_UTILS_SEMVER_H
#define RSP_CORE_LIB_UTILS_SEMVER_H

#include <array>
#include <charconv>
#include <concepts>
#include <initializer_list>
#include <limits>
#include <ranges>
#include <string_view>
#include <string>
#include <system_error>
#include <tuple>
#include <type_traits>
#include <variant>
#include <vector>

namespace rsp::utils::semver {

namespace detail {
template <typename T>
concept numeric = std::is_arithmetic_v<T> && !std::is_same_v<T, bool>;

// TODO: Move to StrUtils?
[[nodiscard]] inline constexpr std::string ToString(numeric auto value)
{
    std::array<char, std::numeric_limits<decltype(value)>::digits10 + 1> buf{};
    auto [ptr, ec] = std::to_chars(buf.begin(), buf.end(), value);
    if (ec != std::errc{})
        throw std::runtime_error{std::make_error_code(ec).message()};
    return std::string{buf.data(), static_cast<std::size_t>(ptr - buf.data())};
}
} // namespace detail

class PreRelease
{
public:
    using Part = std::variant<unsigned, std::string>;

    // The higher variant index is used for std::string to give non-numeric
    // identifiers higher precedence when comparing, as per SemVer rules.
    static_assert(std::is_same_v<unsigned, std::variant_alternative_t<0, Part>>);
    static_assert(std::is_same_v<std::string, std::variant_alternative_t<1, Part>>);

    /**
     * \brief Construct empty (default).
     */
    constexpr PreRelease() = default;

    /**
     * \brief Construct from a dot-separated string of identifiers.
     */
    constexpr PreRelease(std::string_view str)
    {
        for (auto rg : std::views::split(str, '.')) {
            std::string_view part{rg};

            if (part.empty())
                throw std::invalid_argument{"Empty pre-release identifier"};

            unsigned value = 0;
            auto [ptr, ec] = std::from_chars(part.begin(), part.end(), value);
            if (ec == std::errc{} && ptr == part.end()) // OK iff entire part is a valid uint
                _parts.push_back(value);
            else
                _parts.push_back(std::string{part});
        }
    }

    [[nodiscard]] constexpr std::size_t Size() const noexcept { return _parts.size(); }
    [[nodiscard]] constexpr bool Empty() const noexcept { return _parts.empty(); }

    /**
     * \brief Convert pre-release identifiers to a dot-separated string.
     */
    [[nodiscard]] constexpr std::string ToString() const
    {
        std::string result;

        for (const auto& part : _parts) {
            if (!result.empty())
                result += '.';

            std::visit(
                [&](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, unsigned>)
                        result += detail::ToString(arg);
                    else if constexpr (std::is_same_v<T, std::string>)
                        result += arg;
                    else
                        static_assert(false, "Non-exhaustive visitor");
                },
                part);
        }

        return result;
    }

    [[nodiscard]] constexpr bool operator==(const PreRelease& other) const noexcept
    {
        return (_parts == other._parts);
    }

    [[nodiscard]] constexpr std::strong_ordering operator<=>(const PreRelease& other) const noexcept
    {
        // Compare pre-release identifiers from left to right until a difference
        // is found
        for (auto&& [a, b] : std::views::zip(_parts, other._parts)) {
            if (auto cmp = a <=> b; cmp != 0)
                return cmp;
        }

        // If the preceding identifiers are equal, then the one with more
        // identifiers has higher precedence
        return _parts.size() <=> other._parts.size();
    }

private:
    // pre-release identifiers (may be numeric or alphanumeric, dot-separated in
    // the original string)
    std::vector<Part> _parts{};
};

/**
 * \brief Utility class representing a SemVer 2.0.0 version number.
 *
 * Supports the full format: <major>.<minor>.<patch>[-<pre-release>][+<build>]
 *
 * Pre-release identifiers are dot-separated and may be numeric or alphanumeric.
 * Build metadata is stored but ignored for comparison purposes.
 */
class Version
{
public:
    /**
     * \brief Construct 0.0.0 (default).
     */
    constexpr Version() = default;

    /**
     * \brief Construct from components, with optional pre-release and build metadata.
     */
    constexpr Version(unsigned major, unsigned minor, unsigned patch, std::string_view preRelease = {}, std::string_view build = {})
        : _major{major}
        , _minor{minor}
        , _patch{patch}
        , _prerelease{preRelease}
        , _build{build}
    {
    }

    /**
     * \brief Construct from a SemVer string of format <major>.<minor>.<patch>[-<pre-release>][+<build>]
     *
     * Example: "1.2.3-alpha.1+build.321".
     */
    constexpr Version(std::string_view str)
    {
        // Use std::from_chars() to keep it constexpr
        auto parseUnsigned = [](std::string_view s) -> std::tuple<unsigned, std::string_view> {
            unsigned value = 0;
            auto [ptr, ec] = std::from_chars(s.begin(), s.end(), value);
            if (ec != std::errc{})
                throw std::invalid_argument{std::make_error_code(ec).message()};
            return {value, {ptr, s.end()}};
        };

        // Parse major.minor.patch
        std::tie(_major, str) = parseUnsigned(str);
        if (str.empty() || str.front() != '.')
            throw std::invalid_argument{"Invalid version string: missing minor version"};
        std::tie(_minor, str) = parseUnsigned(str.substr(1));
        if (str.empty() || str.front() != '.')
            throw std::invalid_argument{"Invalid version string: missing patch version"};
        std::tie(_patch, str) = parseUnsigned(str.substr(1));

        // Find pre-release and build metadata (optional)
        std::size_t pos_hyphen = str.find_first_of('-');
        std::size_t pos_plus = str.find_first_of('+');

        if ((pos_hyphen != std::string_view::npos) && (pos_hyphen < pos_plus))
            _prerelease = str.substr(pos_hyphen + 1, pos_plus - pos_hyphen - 1);

        if (pos_plus != std::string_view::npos)
            _build = str.substr(pos_plus + 1);
    }

    [[nodiscard]] constexpr unsigned GetMajor() const noexcept { return _major; }
    [[nodiscard]] constexpr unsigned GetMinor() const noexcept { return _minor; }
    [[nodiscard]] constexpr unsigned GetPatch() const noexcept { return _patch; }
    [[nodiscard]] constexpr std::string GetPreRelease() const { return _prerelease.ToString(); }
    [[nodiscard]] constexpr std::string GetBuild() const { return _build; }

    /**
     * \brief Convert the version to a string in the format <major>.<minor>.<patch>[-<pre-release>][+<build>].
     */
    [[nodiscard]] constexpr std::string ToString() const
    {
        auto result = detail::ToString(_major) + "." + detail::ToString(_minor) + "." + detail::ToString(_patch);

        if (!_prerelease.Empty())
            result += '-' + _prerelease.ToString();

        if (!_build.empty())
            result += '+' + _build;

        return result;
    }

    /**
     * \brief Compare version numbers for equality as per SemVer 2.0.0 rules.
     */
    [[nodiscard]] constexpr bool operator==(const Version& other) const noexcept
    {
        return (_major == other._major) && (_minor == other._minor) && (_patch == other._patch) && (_prerelease == other._prerelease);
        // Build metadata is ignored for equality
    }

    /**
     * \brief Compare version numbers as per SemVer 2.0.0 rules.
     */
    [[nodiscard]] constexpr std::strong_ordering operator<=>(const Version& other) const noexcept
    {
        if (auto cmp = _major <=> other._major; cmp != 0) return cmp;
        if (auto cmp = _minor <=> other._minor; cmp != 0) return cmp;
        if (auto cmp = _patch <=> other._patch; cmp != 0) return cmp;

        if (_prerelease.Empty() && other._prerelease.Empty())
            return std::strong_ordering::equal; // Both are normal versions
        else if (_prerelease.Empty())
            return std::strong_ordering::greater; // Normal version has higher precedence than pre-release
        else if (other._prerelease.Empty())
            return std::strong_ordering::less; // -||-
        else
            return _prerelease <=> other._prerelease; // Compare pre-release identifiers

        // Build metadata is ignored for precedence
    }

private:
    unsigned _major{0};
    unsigned _minor{0};
    unsigned _patch{0};
    PreRelease _prerelease{}; // pre-release identifiers
    std::string _build{};     // dot-separated build identifiers
};

} // namespace rsp::utils::semver

#endif // RSP_CORE_LIB_UTILS_SEMVER_H
