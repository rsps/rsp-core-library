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
#include <utils/StructElement.h>

using namespace rsp::utils;

TEST_CASE("Variant")
{
    Variant v;

    SUBCASE("Types") {
        CHECK(v.IsNull());
        CHECK_EQ(v.GetType(), Variant::Types::Null);

        v = true;
        CHECK(!v.IsNull());
        CHECK_EQ(v.GetType(), Variant::Types::Bool);
        CHECK(v.AsBool());

        v.Clear();
        CHECK(v.IsNull());
        CHECK_EQ(v.GetType(), Variant::Types::Null);
        CHECK(!v.AsBool());

        v= 42;
        CHECK_EQ(v.GetType(), Variant::Types::Int);
        CHECK_EQ(v.AsInt(), 42);

        v = 42.0f;
        CHECK_EQ(v.GetType(), Variant::Types::Float);
        CHECK(IsInRange(v.AsFloat(), 42.0f, 42.0f));
        CHECK_EQ(v.AsInt(), 42);
        float f(v);
        CHECK(IsInRange(f, 42.0f, 42.0f));

        StructElement<int> se;
        v = se;
        CHECK(v.IsNull());
        CHECK_EQ(v.GetType(), Variant::Types::Null);
    }

    SUBCASE("Streaming") {
        std::stringstream ss;
        v = uint64_t(42);
        ss << "Variant = " << v;

        CHECK_EQ(ss.str(), "Variant = uint64:42");
    }

    SUBCASE("Assertions") {
        v.Clear();

        CHECK_THROWS_AS(auto a = v.AsInt(), const EConversionError&);
        CHECK_THROWS_AS(auto a = v.AsDouble(), const EConversionError&);
    }

    SUBCASE("Copy") {
        v = 42;
        Variant b(v);

        CHECK_EQ(v.AsInt(), b.AsInt());

        v = 44;
        b = v;
        CHECK_EQ(v.AsInt(), b.AsInt());

        b = 43;
        CHECK_NE(v.AsInt(), b.AsInt());
        CHECK_EQ(v.AsInt(), 44);
        CHECK_EQ(b.AsInt(), 43);
    }

    SUBCASE("Move") {
        v = 42;
        Variant b(std::move(v));

        CHECK(v.IsNull()); // NOLINT
        CHECK_EQ(b.AsInt(), 42);

        Variant c{};
        c = std::move(b);

        CHECK(b.IsNull()); // NOLINT
        CHECK_EQ(c.AsInt(), 42);
    }

    SUBCASE("String Precision") {
        v = 830.3468;
        CHECK_EQ(v.AsString(), "830.34680000000003");
        Variant r(v.AsString());
        CHECK_EQ(r.AsDouble(), 830.34680000000003);

        v = -4.47836192e-09;
        CHECK_EQ(v.AsString(), "-4.4783619199999997e-09");
        r = v.AsString();
        CHECK_EQ(r.AsDouble(), -4.4783619199999997e-09);

        v = -3.18854398e-05;
        CHECK_EQ(v.AsString(), "-3.1885439799999998e-05");
        r = v.AsString();
        CHECK_EQ(r.AsDouble(), -3.1885439799999998e-05);

        v = 1.93246008e-01;
        CHECK_EQ(v.AsString(), "0.193246008");
        r = v.AsString();
        CHECK_EQ(r.AsDouble(), 0.193246008);

        v = 8.20831198e+02;
        CHECK_EQ(v.AsString(), "820.83119799999997");
        r = v.AsString();
        CHECK_EQ(r.AsDouble(), 820.83119799999997);

        v = 1.7976931348623157e+308;
        CHECK_EQ(v.AsString(), "1.7976931348623157e+308");
        r = v.AsString();
        CHECK_EQ(r.AsDouble(), 1.7976931348623157e+308);

        v = 1.234567;
        v.SetPrecision(3);
        CHECK_EQ(v.AsString(), "1.235");
        r = v.AsString(); // Example of "lost in translation" / rounding effects
        CHECK_EQ(r.AsDouble(), 1.235);
        CHECK_EQ(v.AsDouble(), 1.234567);
    }

}
