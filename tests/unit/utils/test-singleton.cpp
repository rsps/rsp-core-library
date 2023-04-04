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
#include <utils/Singleton.h>

using namespace rsp::utils;
using namespace rsp::exceptions;

TEST_CASE("Singleton") {

    struct MyClass : Singleton<MyClass> {
        bool Compare(int a, int b) {
            return a == b;
        }
    };

    SUBCASE("Self Owned") {
        CHECK_THROWS_AS(MyClass::GetInstance(), const ENoInstance&);

        CHECK_NOTHROW(MyClass::CreateInstance());
        CHECK_NOTHROW(MyClass::GetInstance());
        CHECK(MyClass::GetInstance().Compare(0, 0));

        CHECK_THROWS_AS(MyClass::CreateInstance(), const ESingletonViolation&);
        CHECK_THROWS_AS(MyClass::SetInstance(static_cast<MyClass*>(nullptr)), const ESingletonViolation&);

        CHECK_NOTHROW(MyClass::DestroyInstance());
        CHECK_THROWS_AS(MyClass::GetInstance(), const ENoInstance&);
    }

    SUBCASE("External Owned") {
        CHECK_THROWS_AS(MyClass::GetInstance(), const ENoInstance&);

        MyClass o;
        CHECK_NOTHROW(MyClass::SetInstance(&o));
        CHECK_NOTHROW(MyClass::GetInstance());
        CHECK(MyClass::GetInstance().Compare(1, 1));

        CHECK_THROWS_AS(MyClass::CreateInstance(), const ESingletonViolation&);
        CHECK_NOTHROW(MyClass::DestroyInstance());
        CHECK_THROWS_AS(MyClass::CreateInstance(), const ESingletonViolation&);

        CHECK_NOTHROW(MyClass::SetInstance(static_cast<MyClass*>(nullptr)));
        CHECK_THROWS_AS(MyClass::GetInstance(), const ENoInstance&);
    }
}



