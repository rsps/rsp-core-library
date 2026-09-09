/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2026 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      steffen
 */

#include <rsp/utils/EnumReflection.h>
#include <doctest.h>
#include <array>
#include <cstdint>
#include <string_view>

using namespace std::string_view_literals;

TEST_SUITE_BEGIN("Utils");

enum class States {
    One = 1,
    Two,
    Three
};

enum class Sparse {
    A = 1,
    B = 5,
    C = 100
};

enum class Negative {
    Low = -3,
    Mid = 0,
    High = 3
};

enum Unscoped {
    UNSCOPED_A,
    UNSCOPED_B
};

enum class Int8Bounds : int8_t {
    Min = -128,
    Max = 127
};

enum class OutOfRange {
    InRange = 1,
    TooHigh = 200 // outside the reflectable [-128, 127] range
};

TEST_CASE("EnumCount")
{
    SUBCASE("basic scoped enum")
    {
        constexpr auto count = rsp::utils::EnumCount<States>();

        // This will only compile if 'count' is truly constexpr
        static_assert(count == 3u);

        // Array sized by constexpr - proves it works at compile time
        std::array<int, count> arr{};
        CHECK_EQ(count, 3u);
        CHECK_EQ(arr.size(), 3u);
    }

    SUBCASE("sparse, negative and unscoped enums")
    {
        static_assert(rsp::utils::EnumCount<Sparse>() == 3u);
        static_assert(rsp::utils::EnumCount<Negative>() == 3u);
        static_assert(rsp::utils::EnumCount<Unscoped>() == 2u);
    }
}

TEST_CASE("EnumName")
{
    SUBCASE("basic scoped enum")
    {
        constexpr auto value = States::One;
        constexpr auto name = rsp::utils::EnumName(value);
        CHECK_EQ(name, "One");

        enum class Foo {
            SPAM,
            HAM
        };
        static_assert(rsp::utils::EnumName(Foo::SPAM) == "SPAM"sv);
    }

    SUBCASE("sparse and negative values")
    {
        static_assert(rsp::utils::EnumName(Sparse::B) == "B"sv);
        static_assert(rsp::utils::EnumName(Negative::Low) == "Low"sv);
        static_assert(rsp::utils::EnumName(Negative::Mid) == "Mid"sv);
        static_assert(rsp::utils::EnumName(Unscoped::UNSCOPED_A) == "UNSCOPED_A"sv);

        CHECK_EQ(rsp::utils::EnumName(Sparse::B), "B");
        CHECK_EQ(rsp::utils::EnumName(Negative::Low), "Low");
    }

    SUBCASE("value with no matching enumerator returns empty")
    {
        auto name = rsp::utils::EnumName(static_cast<Sparse>(2));
        CHECK(name.empty());
    }

    SUBCASE("value outside representable range is not found")
    {
        auto name = rsp::utils::EnumName(OutOfRange::TooHigh);
        CHECK(name.empty());
        CHECK_EQ(rsp::utils::EnumName(OutOfRange::InRange), "InRange");
    }

    SUBCASE("int8_t underlying type at range boundaries")
    {
        static_assert(rsp::utils::EnumName(Int8Bounds::Min) == "Min"sv);
        static_assert(rsp::utils::EnumName(Int8Bounds::Max) == "Max"sv);
    }

    SUBCASE("same enumerator name in different enums does not collide")
    {
        enum class Color {
            Red,
            Green
        };
        enum class Signal {
            Red,
            Yellow,
            Green
        };

        static_assert(rsp::utils::EnumName(Color::Red) == "Red"sv);
        static_assert(rsp::utils::EnumName(Signal::Red) == "Red"sv);
    }
}

TEST_CASE("EnumCast")
{
    SUBCASE("name to value lookup")
    {
        static_assert(rsp::utils::EnumCast<States>("Two") == States::Two);

        auto value = rsp::utils::EnumCast<Sparse>("C");
        REQUIRE(value.has_value());
        CHECK_EQ(*value, Sparse::C);
    }

    SUBCASE("unknown name returns nullopt")
    {
        auto value = rsp::utils::EnumCast<States>("NotAName");
        CHECK_FALSE(value.has_value());
    }

    SUBCASE("empty string returns nullopt")
    {
        CHECK_FALSE(rsp::utils::EnumCast<States>("").has_value());
    }

    SUBCASE("name in different enums does not collide")
    {
        enum class Color {
            Red,
            Green
        };
        enum class Signal {
            Red,
            Yellow,
            Green
        };

        CHECK_EQ(rsp::utils::EnumCast<Color>("Red"), Color::Red);
        CHECK_EQ(rsp::utils::EnumCast<Signal>("Red"), Signal::Red);
    }

    SUBCASE("round trip")
    {
        // EnumCast(EnumName(v)) == v
        for (auto v : {States::One, States::Two, States::Three}) {
            auto name = rsp::utils::EnumName(v);
            auto back = rsp::utils::EnumCast<States>(name);
            REQUIRE(back.has_value());
            CHECK_EQ(*back, v);
        }
    }
}

TEST_CASE("EnumValues")
{
    SUBCASE("basic scoped enum in ascending order")
    {
        constexpr auto values = rsp::utils::EnumValues<States>();

        static_assert(values.size() == 3u);
        static_assert(values[0] == States::One);
        static_assert(values[1] == States::Two);
        static_assert(values[2] == States::Three);
    }

    SUBCASE("sparse and negative enums")
    {
        constexpr auto sparse = rsp::utils::EnumValues<Sparse>();
        static_assert(sparse.size() == 3u);
        static_assert(sparse[0] == Sparse::A);
        static_assert(sparse[1] == Sparse::B);
        static_assert(sparse[2] == Sparse::C);

        constexpr auto negative = rsp::utils::EnumValues<Negative>();
        static_assert(negative.size() == 3u);
        static_assert(negative[0] == Negative::Low);
        static_assert(negative[1] == Negative::Mid);
        static_assert(negative[2] == Negative::High);
    }

    SUBCASE("range-based iteration matches EnumName")
    {
        for (auto value : rsp::utils::EnumValues<States>()) {
            CHECK_FALSE(rsp::utils::EnumName(value).empty());
        }
    }

    SUBCASE("values outside representable range are excluded")
    {
        constexpr auto values = rsp::utils::EnumValues<OutOfRange>();
        static_assert(values.size() == 1u);
        static_assert(values[0] == OutOfRange::InRange);
    }
}

TEST_CASE("EnumNames")
{
    SUBCASE("basic scoped enum in ascending order")
    {
        constexpr auto names = rsp::utils::EnumNames<States>();

        static_assert(names.size() == 3u);
        static_assert(names[0] == "One"sv);
        static_assert(names[1] == "Two"sv);
        static_assert(names[2] == "Three"sv);
    }

    SUBCASE("order matches EnumValues")
    {
        constexpr auto values = rsp::utils::EnumValues<Sparse>();
        constexpr auto names = rsp::utils::EnumNames<Sparse>();

        static_assert(values.size() == names.size());
        for (std::size_t i = 0; i < values.size(); ++i) {
            CHECK_EQ(names[i], rsp::utils::EnumName(values[i]));
        }
    }

    SUBCASE("values outside representable range are excluded")
    {
        constexpr auto names = rsp::utils::EnumNames<OutOfRange>();
        static_assert(names.size() == 1u);
        static_assert(names[0] == "InRange"sv);
    }
}

TEST_CASE("EnumEntries")
{
    SUBCASE("basic scoped enum in ascending order")
    {
        constexpr auto entries = rsp::utils::EnumEntries<States>();

        static_assert(entries.size() == 3u);
        static_assert(entries[0].first == States::One && entries[0].second == "One"sv);
        static_assert(entries[1].first == States::Two && entries[1].second == "Two"sv);
        static_assert(entries[2].first == States::Three && entries[2].second == "Three"sv);
    }

    SUBCASE("order matches EnumValues and EnumNames")
    {
        constexpr auto values = rsp::utils::EnumValues<Sparse>();
        constexpr auto names = rsp::utils::EnumNames<Sparse>();
        constexpr auto entries = rsp::utils::EnumEntries<Sparse>();

        static_assert(entries.size() == values.size());
        for (std::size_t i = 0; i < entries.size(); ++i) {
            CHECK_EQ(entries[i].first, values[i]);
            CHECK_EQ(entries[i].second, names[i]);
        }
    }

    SUBCASE("values outside representable range are excluded")
    {
        constexpr auto entries = rsp::utils::EnumEntries<OutOfRange>();
        static_assert(entries.size() == 1u);
        static_assert(entries[0].first == OutOfRange::InRange);
        static_assert(entries[0].second == "InRange"sv);
    }
}

TEST_SUITE_END();
