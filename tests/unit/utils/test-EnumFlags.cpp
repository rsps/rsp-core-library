/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <cstdint>
#include <doctest.h>
#include <rsp/utils/EnumFlags.h>

enum class TestFlags : uint32_t {
    None = 0,
    F1 = 1u << 0,
    F2 = 1u << 1,
    F3 = 1u << 2,
    F4 = 1u << 3,
    F5 = 1u << 4,
    F6 = 1u << 5,
    F7 = 1u << 6,
    F8 = 1u << 7,
    F9 = 1u << 8,
    F10 = 1u << 9,
    F11 = 1u << 10,
    F12 = 1u << 11,
    F13 = 1u << 12,
    F14 = 1u << 13,
    F15 = 1u << 14,
    F16 = 1u << 15,
    F32 = 1u << 31,
    // F33 = 1u << 32,
};

enum class SparseFlags {
    A = 0x01,
    B = 0x02,
    C = 0x04,
    D = A | B,
};

enum class NotFlags {
    One,
    Two,
};

enum MyUnscopedEnum {
    Foo,
    Bar,
};

static_assert(!static_cast<bool>(rsp::utils::EnumFlags<SparseFlags>{}));
static_assert(static_cast<int>(rsp::utils::EnumFlags<SparseFlags>{}) == 0);
static_assert(static_cast<int>(rsp::utils::EnumFlags<SparseFlags>{SparseFlags::A}) == 0x01);
static_assert(static_cast<int>(rsp::utils::EnumFlags<SparseFlags>{0x02}) == 0x02);
static_assert(rsp::utils::EnumFlags<SparseFlags>{0x01}.IsSet(SparseFlags::A));
static_assert(rsp::utils::EnumFlags<SparseFlags>{SparseFlags::D}.HasAll(rsp::utils::EnumFlags<SparseFlags>{SparseFlags::A, SparseFlags::B}));
static_assert(!rsp::utils::EnumFlags<SparseFlags>{SparseFlags::D}.HasAny(SparseFlags::C));

// Comparing unrelated enum types must not compile
template <typename A, typename B>
concept Comparable = requires(A a, B b) {
    a == b;
};
static_assert(!Comparable<TestFlags, SparseFlags>);

// EnumFlags works for any scoped enum, no opt-in macro required
template <typename E>
concept HasEnumFlags = requires { typename rsp::utils::EnumFlags<E>; };
static_assert(HasEnumFlags<NotFlags>);

// ... but not for unscoped enums
static_assert(!HasEnumFlags<MyUnscopedEnum>);

class MyClass
{
public:
    enum class NestedEnum {
        A = 1,
        B = 2,
    };

    static constexpr NestedEnum value = rsp::utils::EnumFlags<NestedEnum>{NestedEnum::A, NestedEnum::B};

    NestedEnum GetCombination() const noexcept;
};

MyClass::NestedEnum MyClass::GetCombination() const noexcept
{
    return rsp::utils::EnumFlags<NestedEnum>{NestedEnum::A, NestedEnum::B};
}

TEST_SUITE_BEGIN("Utils");

TEST_CASE("EnumFlags")
{
    using namespace rsp::utils;

    EnumFlags<TestFlags> flags;

    CHECK_EQ(sizeof(flags), 4u);

    CHECK_EQ(flags, TestFlags::None);
    CHECK_FALSE(static_cast<bool>(flags));

    flags |= TestFlags::F3;
    CHECK_EQ(flags, TestFlags::F3);
    CHECK(flags.IsSet(TestFlags::F3));
    CHECK(flags.HasAll(TestFlags::F3));
    CHECK(flags.HasAny(TestFlags::F3));

    flags |= TestFlags::F10;
    CHECK_EQ(flags, EnumFlags<TestFlags>{TestFlags::F3, TestFlags::F10});

    flags |= TestFlags::F16;
    CHECK_EQ(flags, EnumFlags<TestFlags>{TestFlags::F3, TestFlags::F10, TestFlags::F16});
    CHECK_EQ(flags & TestFlags::F10, TestFlags::F10);

    flags &= EnumFlags<TestFlags>{TestFlags::F3, TestFlags::F16};
    CHECK_EQ(flags, EnumFlags<TestFlags>{TestFlags::F3, TestFlags::F16});

    EnumFlags<TestFlags> other(1u << 31);
    CHECK_EQ(other, TestFlags::F32);

    flags |= other;
    CHECK_EQ(flags, EnumFlags<TestFlags>{TestFlags::F3, TestFlags::F16, TestFlags::F32});

    flags &= other;
    CHECK_EQ(flags, TestFlags::F32);

    flags ^= TestFlags::F3;
    CHECK_EQ(flags, rsp::utils::EnumFlags<TestFlags>{TestFlags::F32, TestFlags::F3});

    flags ^= TestFlags::F3;
    CHECK_EQ(flags, TestFlags::F32);

    CHECK_EQ((EnumFlags<TestFlags>{TestFlags::F3} ^ TestFlags::F10), EnumFlags<TestFlags>{TestFlags::F3, TestFlags::F10});
}

TEST_SUITE_END();
