/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2024 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#include "doctest.h"
#include <rsp/utils/ObjectPool.h>

using namespace rsp::utils;

TEST_SUITE_BEGIN("Utils");

static int counter = 0;

struct MyType {
    int member;
    MyType() : member(counter++) {}
};

TEST_CASE("ObjectPool")
{
    ObjectPool<MyType> pool(3);

    CHECK_EQ(pool.Available(), 3u);

    auto &e1 = pool.Get();
    CHECK_EQ(pool.Available(), 2u);
    CHECK_EQ(e1.member, 2);

    auto &e2 = pool.Get();
    CHECK_EQ(pool.Available(), 1u);
    CHECK_EQ(e2.member, 1);

    auto &e3 = pool.Get();
    CHECK_EQ(pool.Available(), 0u);
    CHECK_EQ(e3.member, 0);

    CHECK_THROWS_AS(pool.Get(), EObjectPoolException);
    CHECK_EQ(pool.Available(), 0u);

    CHECK_NOTHROW(pool.Put(e1));
    CHECK_EQ(pool.Available(), 1u);

    CHECK_NOTHROW(pool.Put(e3));
    CHECK_EQ(pool.Available(), 2u);

    CHECK_NOTHROW(pool.Put(e2));
    CHECK_EQ(pool.Available(), 3u);

    MyType local;
    CHECK_THROWS_AS(pool.Put(local), EObjectPoolException);

    auto &e4 = pool.Get();
    CHECK_EQ(pool.Available(), 2u);
    CHECK_EQ(e4.member, 1);
}

TEST_SUITE_END();
