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

TEST_CASE("Singleton") {

    struct MyClass : Singleton<MyClass> {
        bool Compare(int a, int b) {
            return a == b;
        }
    };

    SUBCASE("Self Owned") {
        CHECK_THROWS_AS(MyClass::Get(), const ENoInstance&);

        CHECK_NOTHROW(MyClass::Create());
        CHECK_NOTHROW(MyClass::Get());
        CHECK(MyClass::Get().Compare(0, 0));

        CHECK_THROWS_AS(MyClass::Create(), const ESingletonViolation&);
        CHECK_THROWS_AS(MyClass::Set(static_cast<MyClass*>(nullptr)), const ESingletonViolation&);

        CHECK_NOTHROW(MyClass::Destroy());
        CHECK_THROWS_AS(MyClass::Get(), const ENoInstance&);
    }

    SUBCASE("External Owned") {
        CHECK_THROWS_AS(MyClass::Get(), const ENoInstance&);

        MyClass o;
        CHECK_NOTHROW(MyClass::Set(&o));
        CHECK_NOTHROW(MyClass::Get());
        CHECK(MyClass::Get().Compare(1, 1));

        CHECK_THROWS_AS(MyClass::Create(), const ESingletonViolation&);
        CHECK_NOTHROW(MyClass::Destroy());
        CHECK_THROWS_AS(MyClass::Create(), const ESingletonViolation&);

        CHECK_NOTHROW(MyClass::Set(static_cast<MyClass*>(nullptr)));
        CHECK_THROWS_AS(MyClass::Get(), const ENoInstance&);
    }
}



