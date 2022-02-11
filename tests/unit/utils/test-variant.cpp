/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <sstream>
#include <doctest.h>
#include <utils/InRange.h>
#include <utils/Variant.h>

using namespace rsp::utils;

TEST_CASE("Variant")
{
    Variant v;

    SUBCASE("Types") {
        CHECK(v.IsNull());
        CHECK(v.GetType() == Variant::Types::Null);

        v = true;
        CHECK(!v.IsNull());
        CHECK(v.GetType() == Variant::Types::Bool);
        CHECK(v.AsBool());

        v.Clear();
        CHECK(v.IsNull());
        CHECK(v.GetType() == Variant::Types::Null);
        CHECK(!v.AsBool());

        v= 42;
        CHECK(v.GetType() == Variant::Types::Int);
        CHECK(v.AsInt() == 42);

        v = 42.0f;
        CHECK(v.GetType() == Variant::Types::Float);
        CHECK(IsInRange(static_cast<float>(v.AsDouble()), 42.0f, 42.0f));
        CHECK(v.AsInt() == 42);
        float f = v;
        CHECK(IsInRange(f, 42.0f, 42.0f));
    }

    SUBCASE("Streaming") {
        std::stringstream ss;
        v = 42ul;
        ss << "Variant = " << v;

        CHECK(ss.str() == "Variant = uint64:42");
    }

    SUBCASE("Assertions") {
        v.Clear();

        CHECK_THROWS_AS(v.AsInt(), const EConversionError&);
        CHECK_THROWS_AS(v.AsDouble(), const EConversionError&);
    }
}



