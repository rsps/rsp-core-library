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

    Function<int(void)> iv = []() { return 42; };
    CHECK(iv() == 42);

    iv = test_func;
    CHECK(iv() == 84);

    Function<int(int)> ii = [](int value) { return value; };
    CHECK(ii(43) == 43);

    TestClass o;
    auto my_func = Method(&o, &TestClass::TestMethod);

    CHECK(my_func(42, 2.0) == 84);

    Function<int(int, double)> bind_func = std::bind(&TestClass::TestMethod, &o, 2, std::placeholders::_1);
//    CHECK(bind_func(2.2) == 4);
    CHECK(bind_func(21, 2.9) == 42);
}


