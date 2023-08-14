/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include "doctest.h"
#include <utils/Function.h>

using namespace rsp::utils;

static int test_func()
{
    return 84;
}

class TestClass
{
public:
    int TestMethod(int i, double d) {
        return i * d;
    }
};

TEST_CASE("Functions") {

    Function<void(void)> vv;
    CHECK_NOTHROW(vv());

    Function<int(void)> iv = []() noexcept { return 42; };
    CHECK(iv() == 42);

    iv = test_func;
    CHECK(iv() == 84);

    Function<int(int)> ii = [](int value) noexcept { return value; };
    CHECK(ii(43) == 43);

    TestClass o;
    auto my_func = Method(&o, &TestClass::TestMethod);

    CHECK(my_func(42, 2.0) == 84);

    Function<int(double)> bind_func1 = std::bind(&TestClass::TestMethod, &o, 2, std::placeholders::_1);
    CHECK(bind_func1(2.2) == 4);

    Function<int(int)> bind_func2 = std::bind(&TestClass::TestMethod, &o, std::placeholders::_1, 4.9);
    CHECK(bind_func2(2) == 9);

    Function<int(void)> bind_func3 = std::bind(&TestClass::TestMethod, &o, 3, 2.2);
    CHECK(bind_func3() == 6);
}


