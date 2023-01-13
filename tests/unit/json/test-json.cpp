/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <json/Json.h>
#include <json/JsonDecoder.h>

#include "doctest.h"
#include <iostream>
#include <utils/StrUtils.h>
#include <utils/InRange.h>
#include <TestHelpers.h>

using namespace rsp::utils;
using namespace rsp::json;

TEST_CASE("Json") {

    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    std::string json_object{ R"(
{
    "NullValue": null,
    "BooleanValue": true,
    "StringValue": "Hello\tWorld",
    "IntValue": 42,
    "FloatValue": 1.234567,
    "ArrayValue": [
        32,
        "string",
        true,
        null
    ],
    "ObjectMember": {
        "Boolean": false,
        "Empty": null,
        "NestedObject": {
            "NestedValue": "Cheers"
        }
    }
}
)"  };

    SUBCASE("Create Empty") {
        JsonValue v1;
        CHECK(v1.GetJsonType() == JsonTypes::Null);
        CHECK_EQ("null", v1.Encode());
        CHECK_THROWS_AS(v1.AsInt(), EConversionError);
        CHECK_THROWS_AS(v1.AsFloat(), EConversionError);
        CHECK_THROWS_AS(v1.AsDouble(), EConversionError);
        CHECK_EQ("null", v1.AsString());
    }


    SUBCASE("Create Float") {
        JsonValue v1(1.42f);
        CHECK(v1.GetJsonType() == JsonTypes::Number);
        CHECK(IsEqual(1.42f, static_cast<float>(v1), 0.000001f));
        CHECK_EQ("1.41999996", v1.Encode());
    }

    SUBCASE("Create Double") {
        JsonValue v1(456321.7651234);
        CHECK(v1.GetJsonType() == JsonTypes::Number);
        CHECK(IsEqual(456321.7651234, static_cast<double>(v1), 0.00000001));
        CHECK_EQ("456321.76512340002", v1.Encode());
    }

    SUBCASE("Create String") {
        JsonValue v2("My World");
        CHECK(v2.GetType() == JsonValue::Types::String);
        CHECK("My World" == v2.AsString());
        CHECK(StrUtils::Contains(v2.Encode(), "\"My World\""));
        CHECK("\"My World\"" == v2.Encode());
    }

    SUBCASE("Create Unicode String") {
        JsonValue v3 = JsonDecoder("\"My \\u0057orld\"").GetValue();
        CHECK(v3.GetType() == JsonValue::Types::String);
        CHECK("My World" == v3.AsString());

        v3 = JsonDecoder("\"Euro sign: \\u20AC\"").GetValue();
//            CHECK("Euro sign: €" == "Euro sign: \\u20AC");
        CHECK(v3.GetType() == JsonValue::Types::String);
        CHECK("Euro sign: €" == v3.AsString());
    }

    SUBCASE("Ignore Whitespace") {
        std::string ws = R"({"whitespace":  

	
null }
)";
        Json v4;

        CHECK_NOTHROW(v4 = JsonDecoder(ws).GetValue());

        CHECK_EQ(v4.GetJsonType(), JsonTypes::Object);
        CHECK_EQ(v4.GetCount(), 1);
        CHECK(v4["whitespace"].IsNull());
//        MESSAGE(v4.Encode());
    }

    SUBCASE("Decode Object") {
        JsonValue v;
        CHECK_NOTHROW(v = Json::Decode(json_object));

        CHECK(v.GetJsonType() == JsonTypes::Object);
        CHECK(v.GetCount() == 7);

        CHECK(v.MemberExists("NullValue"));
        CHECK(v["NullValue"].IsNull());

        CHECK(v.MemberExists("BooleanValue"));
        CHECK(v["BooleanValue"].IsNull() == false);
        CHECK(static_cast<bool>(v["BooleanValue"]) == true);

        CHECK(v.MemberExists("StringValue"));
        CHECK(v["StringValue"].IsNull() == false);

        CHECK(v.MemberExists("IntValue"));
        CHECK(v["IntValue"].IsNull() == false);
        CHECK(v["IntValue"].GetType() == Variant::Types::Uint64);
        CHECK(static_cast<int>(v["IntValue"]) == 42);

        CHECK(v.MemberExists("FloatValue"));
        CHECK(v["FloatValue"].IsNull() == false);

        CHECK(v.MemberExists("ArrayValue"));
        JsonValue a;
        CHECK_NOTHROW(a = v["ArrayValue"]);
        CHECK(a.GetCount() == 4);
        CHECK(a.IsNull() == false);
        CHECK_EQ(static_cast<int>(a[static_cast<std::size_t>(0u)]), 32);
        CHECK(a[1].AsString() == "string");
        CHECK(static_cast<bool>(a[2]));
        CHECK(a[3].IsNull());

        CHECK_EQ(v["ArrayValue"][0].AsInt(), 32);
        CHECK_EQ(v["ArrayValue"][1].AsString(),  "string");
        CHECK(v["ArrayValue"][2].AsBool());

        CHECK(v.MemberExists("ObjectMember"));
        CHECK(v["ObjectMember"].IsNull() == false);
        CHECK(v["ObjectMember"].GetCount() == 3);
        CHECK_FALSE(static_cast<bool>(v["ObjectMember"]["Boolean"]));
        CHECK(v["ObjectMember"]["Empty"].IsNull());

        CHECK_THROWS_AS(v["ObjectMember"]["empty"].IsNull(), const EJsonException &);

        CHECK_NOTHROW(v["ObjectMember"]["Empty"] = 12.34);
        CHECK_FALSE(v["ObjectMember"]["Empty"].IsNull());
        CHECK(IsEqual(static_cast<double>(v["ObjectMember"]["Empty"]), 12.34, 0.00000000001));

        CHECK_EQ(v["ObjectMember"]["NestedObject"]["NestedValue"].AsString(), "Cheers");
        CHECK_NOTHROW(v["ObjectMember"]["NestedObject"]["NestedValue"] = "Blurp");
        CHECK_EQ(v["ObjectMember"]["NestedObject"]["NestedValue"].AsString(), "Blurp");
    }

    SUBCASE("Encode Object") {
        std::string orig = json_object;
        JsonValue v = JsonDecoder(json_object).GetValue();

        CHECK(v.GetJsonType() == JsonTypes::Object);
        CHECK(v.GetCount() == 7);

        std::string result = v.Encode(true);

        StrUtils::Trim(result);
        StrUtils::Trim(orig);
//        MESSAGE(result.length());
//        MESSAGE(orig.length());
        CHECK(result == orig);

        // Validate UCS2 code-points in output:
        orig = "\"Euro sign: \\u20ac\"";
        JsonValue v1 = JsonDecoder(orig).GetValue();
        CHECK(v1.GetType() == JsonValue::Types::String);
        CHECK("Euro sign: €" == v1.AsString());
        result = v1.Encode(true, true);
        CHECK(result == orig);

        CHECK(TestHelpers::ValidateJson(result));
    }

    SUBCASE("Validate") {
        CHECK_THROWS_AS(JsonDecoder(R"(1.23456.7)").GetValue(), const EJsonNumberError &); // Bad number
        CHECK_THROWS_AS(JsonDecoder(R"(BadString)").GetValue(), const EJsonParseError &);
        CHECK_THROWS_AS(JsonDecoder(R"("Bad Character \k")").GetValue(), const EJsonFormatError &);
        CHECK_THROWS_AS(JsonDecoder(R"(TRUE)").GetValue(), const EJsonParseError &);
        CHECK_NOTHROW(JsonDecoder(R"([   ])").GetValue());
        CHECK_NOTHROW(JsonDecoder(R"([ null ])").GetValue());
        CHECK_THROWS_AS(JsonDecoder(R"([ , ])").GetValue(), const EJsonParseError &);
        CHECK_THROWS_AS(JsonDecoder(R"([ "BadArray", "Excessive Delimiter",])").GetValue(), const EJsonParseError &);
        CHECK_NOTHROW(JsonDecoder(R"({   })").GetValue());
        CHECK_NOTHROW(JsonDecoder(R"({ "empty":null })").GetValue());
        CHECK_THROWS_AS(JsonDecoder(R"({ null })").GetValue(), const EJsonParseError &);
        CHECK_THROWS_AS(JsonDecoder(R"({ , })").GetValue(), const EJsonParseError &);
        CHECK_THROWS_AS(JsonDecoder(R"({ "BadObject": "Excessive Delimiter",})").GetValue(), const EJsonParseError &);
    }

    SUBCASE("Copy") {
        std::string orig = json_object;
        StrUtils::Trim(orig);
        JsonValue p = JsonValue::Decode(json_object);

        JsonValue dst;
        dst = p;

        dst["IntValue"] = 43;

        CHECK(p.IsArray() == dst.IsArray());
        CHECK(p.IsObject() == dst.IsObject());
        CHECK(dst.IsObject() == true);
        CHECK(p.GetCount() == dst.GetCount());
        CHECK(p.GetType() == dst.GetType());
        CHECK(p.GetJsonTypeAsString(p.GetJsonType()) == dst.GetJsonTypeAsString(dst.GetJsonType()));
        CHECK(p["IntValue"].AsInt() != dst["IntValue"].AsInt());

        Json dst2(p);

        dst["IntValue"] = 42;
        CHECK(p.Encode(true) == dst.Encode(true));
        CHECK(p.Encode(true) == dst2.Encode(true));
        CHECK(orig == dst2.Encode(true));
    }

    SUBCASE("Move") {
        std::string orig = json_object;
        StrUtils::Trim(orig);
        JsonValue p = JsonDecoder(json_object).GetValue();
        JsonValue dst = std::move(p);

        CHECK(orig == dst.Encode(true));
        CHECK(p.IsNull());

        JsonValue dst2(std::move(dst));
        CHECK(orig == dst2.Encode(true));
        CHECK(dst.IsNull());
    }

    SUBCASE("Interface") {
        auto o = Json::Decode(json_object);

        CHECK(o.MemberExists("NullValue"));
        CHECK(o["NullValue"].IsNull());

        CHECK(o.MemberExists("BooleanValue"));
        CHECK(o["BooleanValue"].IsNull() == false);
        CHECK(static_cast<bool>(o["BooleanValue"]) == true);

        CHECK(o.MemberExists("StringValue"));
        CHECK(o["StringValue"].IsNull() == false);

        CHECK(o.MemberExists("IntValue"));
        CHECK(o["IntValue"].IsNull() == false);
        CHECK(o["IntValue"].GetType() == Variant::Types::Uint64);
        CHECK(static_cast<int>(o["IntValue"]) == 42);

        Json js1(o);
        CHECK(o.Encode() == js1.Encode());
        js1["IntValue"] = 43;

        CHECK(js1["IntValue"].AsInt() == 43);
        CHECK(o["IntValue"].AsInt() == 42);

        Json js2(std::move(o));
        js2["IntValue"] = 43;

        CHECK(js2.Encode() == js1.Encode());
        CHECK(o.IsNull());
    }

    SUBCASE("No Whitespace") {
        JsonDecoder json{ R"({"Member1":1234,"Member2":{"NestedMember":"NestedValue"}})" };

        auto o = Json::Decode(json);
        CHECK(o.MemberExists("Member1"));
        CHECK(o.MemberExists("Member2"));
        CHECK(o["Member2"].MemberExists("NestedMember"));
        CHECK_EQ(o["Member2"]["NestedMember"].AsString(), "NestedValue");
    }
}

