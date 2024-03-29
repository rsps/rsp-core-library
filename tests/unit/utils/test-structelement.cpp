/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <string>
#include <doctest.h>
#include <json/JsonValue.h>
#include <utils/StructElement.h>
#include <linux/input.h>

using namespace rsp::utils;
using namespace rsp::json;

TEST_CASE("StructElement") {

    struct MyData {
        StructElement<int> mInteger;
        StructElement<float> mFloat;
        StructElement<double> mDouble;
        StructElement<std::string> mString;
        StructElement<bool> mBoolean;
    };

    MyData data{};

    SUBCASE("Is Null") {
        CHECK(data.mInteger.IsNull());
        CHECK(data.mFloat.IsNull());
        CHECK(data.mDouble.IsNull());
        CHECK(data.mString.IsNull());
        CHECK(data.mBoolean.IsNull());
    }

    SUBCASE("Read Null member") {
        int i;
        CHECK_THROWS_AS(i = data.mInteger.Get(), const ENullValueError&);

        CHECK_NOTHROW(i = data.mInteger.Get(defaultItem<int>::default_value()));
        CHECK(i == 0);

        std::string s;
        CHECK_NOTHROW(s = data.mString.Get(defaultItem<std::string>::default_value()));
        CHECK(s.empty());

        JsonValue js;
        CHECK_NOTHROW(js = data.mInteger);
        CHECK(js.IsNull());

        data.mInteger = 42;
        CHECK_NOTHROW(js = data.mInteger);
        CHECK(!js.IsNull());
        CHECK_EQ(int(js), 42);
    }

    SUBCASE("Set/Get value") {
        int i;
        data.mInteger = 42;
        CHECK_NOTHROW(i = data.mInteger);
        CHECK(i == 42);

        data.mInteger.Clear();
        CHECK_THROWS_AS(i = data.mInteger.Get(), const ENullValueError&);
    }

    SUBCASE("Comparison") {
        data.mInteger = 42;

        CHECK(data.mInteger == 42);
        CHECK(42 == data.mInteger);

        data.mFloat.SetMargin(0.002f);
        data.mFloat = 1.2345f;

        CHECK(data.mFloat.Compare(1.2345f));
        CHECK(data.mFloat.Compare(1.2359f));
        CHECK(data.mFloat.Compare(1.2365f));
        CHECK(data.mFloat.Compare(1.2366f) == false);

        CHECK_THROWS_AS(data.mInteger == data.mFloat, const ETypeMismatchError&);
    }
}



