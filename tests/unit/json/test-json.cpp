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

TEST_CASE("Json")
{
    TestLogger logger;

    std::string_view json_object{ R"(
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

    SUBCASE("System") {
        CHECK_EQ(sizeof(float), 4);
        CHECK_EQ(sizeof(double), 8);
        CHECK_EQ(sizeof(long double), 16);
    }


    SUBCASE("Create Empty") {
        DynamicData v1;
        CHECK_EQ(Json::GetJsonType(v1), Json::Types::Null);
        CHECK_EQ("null", JsonEncoder().Encode(v1));
        CHECK_THROWS_AS(int a = v1.AsInt(), EConversionError);
        CHECK_THROWS_AS(float a = v1.AsFloat(), EConversionError);
        CHECK_THROWS_AS(double a = v1.AsDouble(), EConversionError);
        CHECK_EQ("null", v1.AsString());

//        MESSAGE("JsonValue: " << *(static_cast<Variant*>(&v1)) << "\n" << TestHelpers::ToHex(reinterpret_cast<uint8_t*>(&v1), sizeof(Variant)));
}


    SUBCASE("Create Float") {
        DynamicData v1(1.42f);
        CHECK_EQ(Json::GetJsonType(v1), Json::Types::Number);
        CHECK(IsEqual(1.42f, static_cast<float>(v1), 0.000001f));
        CHECK_EQ("1.41999996", Json::Encode(v1));
    }

    SUBCASE("Create Double") {
        DynamicData v1(456321.7651234);
        CHECK_EQ(Json::GetJsonType(v1), Json::Types::Number);
        CHECK(IsEqual(456321.7651234, static_cast<double>(v1), 0.00000001));
        CHECK_EQ("456321.76512340002", Json::Encode(v1));

//        MESSAGE("JsonValue: " << *(static_cast<Variant*>(&v1)) << "\nInt: " << v1.RawAsInt() << "\n" << TestHelpers::ToHex(reinterpret_cast<uint8_t*>(&v1), sizeof(Variant)));

        auto v2 = Json::Decode("{\"Value\": 456321.7651234}");
        auto &v3 = v2["Value"];
        CHECK_EQ(Json::GetJsonType(v3), Json::Types::Number);
        CHECK(IsEqual(456321.7651234, static_cast<double>(v3), 0.00000001));
        CHECK_EQ("456321.76512340002", Json::Encode(v3));

        // Explore output here: https://www.exploringbinary.com/floating-point-converter/
//        MESSAGE("JsonValue: " << *(static_cast<Variant*>(&v3)) << "\nInt: " << v3.RawAsInt() << "\n" << TestHelpers::ToHex(reinterpret_cast<uint8_t*>(&v3), sizeof(Variant)));
    }

    SUBCASE("Create String") {
        DynamicData v2("My World");
        CHECK_EQ(Json::GetJsonType(v2), Json::Types::String);
        CHECK_EQ("My World", v2.AsString());
        CHECK_EQ("\"My World\"", Json::Encode(v2));
    }

    SUBCASE("Create Unicode String") {
        DynamicData v3 = JsonDecoder(R"("My \u0057orld")").Decode();
        CHECK_EQ(Json::GetJsonType(v3), Json::Types::String);
        CHECK_EQ("My World", v3.AsString());

        v3 = JsonDecoder(R"("Euro sign: \u20AC")").Decode();
//            CHECK("Euro sign: €" == "Euro sign: \\u20AC");
        CHECK_EQ(int(v3.GetType()), int(Variant::Types::String));
        CHECK_EQ("Euro sign: €", v3.AsString());
    }

    SUBCASE("Ignore Whitespace") {
        std::string ws = R"({"whitespace":


null }
)";
        DynamicData v4;

        CHECK_NOTHROW(v4 = JsonDecoder(ws).Decode());

        CHECK_EQ(Json::GetJsonType(v4), Json::Types::Object);
        CHECK_EQ(v4.GetCount(), 1);
        CHECK(v4["whitespace"].IsNull());
//        MESSAGE(v4.Encode());
    }

    SUBCASE("Decode Object") {
        Json v;
        CHECK_NOTHROW(v = Json::Decode(json_object));

        SUBCASE("Static") {
        }
        SUBCASE("Dynamic") {
            CHECK_NOTHROW(v = Json(json_object));
        }

        CHECK_EQ(Json::GetJsonType(v), Json::Types::Object);
        CHECK_EQ(v.GetCount(), 7);

        CHECK(v.MemberExists("NullValue"));
        CHECK(v["NullValue"].IsNull());

        CHECK(v.MemberExists("BooleanValue"));
        CHECK_FALSE(v["BooleanValue"].IsNull());
        CHECK(static_cast<bool>(v["BooleanValue"]));

        CHECK(v.MemberExists("StringValue"));
        CHECK_FALSE(v["StringValue"].IsNull());

        CHECK(v.MemberExists("IntValue"));
        CHECK_FALSE(v["IntValue"].IsNull());
        CHECK_EQ(int(v["IntValue"].GetType()), int(Variant::Types::Uint64));
        CHECK_EQ(static_cast<int>(v["IntValue"]), 42);

        CHECK(v.MemberExists("FloatValue"));
        CHECK_FALSE(v["FloatValue"].IsNull());

        CHECK(v.MemberExists("ArrayValue"));
        DynamicData a;
        CHECK_NOTHROW(a = v["ArrayValue"]);
        CHECK_EQ(a.GetCount(), 4);
        CHECK_FALSE(a.IsNull());
        CHECK_EQ(static_cast<int>(a[0]), 32);
        CHECK_EQ(a[1].AsString(), "string");
        CHECK(static_cast<bool>(a[2]));
        CHECK(a[3].IsNull());

        CHECK_EQ(v["ArrayValue"][0].AsInt(), 32);
        CHECK_EQ(v["ArrayValue"][1].AsString(),  "string");
        CHECK(v["ArrayValue"][2].AsBool());

        CHECK(v.MemberExists("ObjectMember"));
        CHECK_FALSE(v["ObjectMember"].IsNull());
        CHECK_EQ(v["ObjectMember"].GetCount(), 3);
        CHECK_FALSE(static_cast<bool>(v["ObjectMember"]["Boolean"]));
        CHECK(v["ObjectMember"]["Empty"].IsNull());

        CHECK_THROWS_AS(v["ObjectMember"]["empty"].IsNull(), const EMemberNotExisting &);

        CHECK_NOTHROW(v["ObjectMember"]["Empty"] = 12.34);
        CHECK_FALSE(v["ObjectMember"]["Empty"].IsNull());
        CHECK(IsEqual(static_cast<double>(v["ObjectMember"]["Empty"]), 12.34, 0.00000000001));

        CHECK_EQ(v["ObjectMember"]["NestedObject"]["NestedValue"].AsString(), "Cheers");
        CHECK_NOTHROW(v["ObjectMember"]["NestedObject"]["NestedValue"] = "Cheese");
        CHECK_EQ(v["ObjectMember"]["NestedObject"]["NestedValue"].AsString(), "Cheese");
    }

    SUBCASE("Encode Object") {
        std::string orig(json_object);
        DynamicData v = JsonDecoder(json_object).Decode();

        CHECK_EQ(Json::GetJsonType(v), Json::Types::Object);
        CHECK_EQ(v.GetCount(), 7);

        std::string result;
        SUBCASE("Static") {
            result = Json::Encode(v, true);
        }
        SUBCASE("Dynamic") {
            Json js(v);
            result = js.Encode(true);
        }

        StrUtils::Trim(result);
        StrUtils::Trim(orig);
//        MESSAGE(result.length());
//        MESSAGE(orig.length());
        CHECK_EQ(result, orig);

        // Validate UCS2 code-points in output:
        orig = R"("Euro sign: \u20ac")";
        DynamicData v1 = Json::Decode(orig);
        CHECK_EQ(int(v1.GetType()), int(DynamicData::Types::String));
        CHECK_EQ("Euro sign: €", v1.AsString());
        result = Json::Encode(v1, true, true);
        CHECK_EQ(result, orig);

        CHECK(TestHelpers::ValidateJson(result));
    }

    SUBCASE("Validate") {
        CHECK_THROWS_AS(JsonDecoder(R"(1.23456.7)").Decode(), const EJsonNumberError &); // Bad number
        CHECK_THROWS_AS(JsonDecoder(R"(BadString)").Decode(), const EJsonParseError &);
        CHECK_THROWS_AS(JsonDecoder(R"("Bad Character \k")").Decode(), const EJsonFormatError &);
        CHECK_THROWS_AS(JsonDecoder(R"(TRUE)").Decode(), const EJsonParseError &);
        CHECK_NOTHROW(JsonDecoder(R"([   ])").Decode());
        CHECK_NOTHROW(JsonDecoder(R"([ null ])").Decode());
        CHECK_THROWS_AS(JsonDecoder(R"([ , ])").Decode(), const EJsonParseError &);
        CHECK_THROWS_AS(JsonDecoder(R"([ "BadArray", "Excessive Delimiter",])").Decode(), const EJsonParseError &);
        CHECK_NOTHROW(JsonDecoder(R"({   })").Decode());
        CHECK_NOTHROW(JsonDecoder(R"({ "empty":null })").Decode());
        CHECK_THROWS_AS(JsonDecoder(R"({ null })").Decode(), const EJsonParseError &);
        CHECK_THROWS_AS(JsonDecoder(R"({ , })").Decode(), const EJsonParseError &);
        CHECK_THROWS_AS(JsonDecoder(R"({ "BadObject": "Excessive Delimiter",})").Decode(), const EJsonParseError &);
    }

    SUBCASE("Copy") {
        std::string orig(json_object);
        StrUtils::Trim(orig);
        auto p = Json::Decode(json_object);

        auto dst = p;

        dst["IntValue"] = 43;

        CHECK_EQ(p.IsArray(), dst.IsArray());
        CHECK_EQ(p.IsObject(), dst.IsObject());
        CHECK(dst.IsObject());
        CHECK_EQ(p.GetCount(), dst.GetCount());
        CHECK_EQ(int(p.GetType()), int(dst.GetType()));
        CHECK_EQ(Json::GetJsonTypeAsString(Json::GetJsonType(p)), Json::GetJsonTypeAsString(Json::GetJsonType(dst)));
        CHECK_NE(p["IntValue"].AsInt(), dst["IntValue"].AsInt());

        DynamicData dst2(p);
//        DynamicData dst2(static_cast<DynamicData&>(p));

        dst["IntValue"] = 42;
        CHECK_EQ(Json::Encode(p, true), Json::Encode(dst, true));
        CHECK_EQ(Json::Encode(p, true), Json::Encode(dst2, true));
        auto result = Json::Encode(dst2, true);
        CHECK_EQ(orig, result);
    }

    SUBCASE("Move") {
        std::string orig(json_object);
        StrUtils::Trim(orig);
        auto p = JsonDecoder(json_object).Decode();
        auto dst = std::move(p);

        CHECK_EQ(orig, Json::Encode(dst, true));
        CHECK(p.IsNull()); // NOLINT

        auto dst2(std::move(dst));
        CHECK_EQ(orig, Json::Encode(dst2, true));
        CHECK(dst.IsNull()); // NOLINT
    }

    SUBCASE("Interface") {
        auto o = Json::Decode(json_object);

        CHECK(o.MemberExists("NullValue"));
        CHECK(o["NullValue"].IsNull());

        CHECK(o.MemberExists("BooleanValue"));
        CHECK_FALSE(o["BooleanValue"].IsNull());
        CHECK(static_cast<bool>(o["BooleanValue"]));

        CHECK(o.MemberExists("StringValue"));
        CHECK_FALSE(o["StringValue"].IsNull());

        CHECK(o.MemberExists("IntValue"));
        CHECK_FALSE(o["IntValue"].IsNull());
        CHECK_EQ(int(o["IntValue"].GetType()), int(Variant::Types::Uint64));
        CHECK_EQ(static_cast<int>(o["IntValue"]), 42);

        DynamicData js1(o);
        CHECK_EQ(Json::Encode(o), Json::Encode(js1));
        js1["IntValue"] = 43;

        CHECK_EQ(js1["IntValue"].AsInt(), 43);
        CHECK_EQ(o["IntValue"].AsInt(), 42);

        DynamicData js2(std::move(o));
        js2["IntValue"] = 43;

        CHECK_EQ(Json::Encode(js2), Json::Encode(js1));
        CHECK(o.IsNull()); // NOLINT
    }

    SUBCASE("No Whitespace") {
        JsonDecoder json{ R"({"Member1":1234,"Member2":{"NestedMember":"NestedValue"}})" };

        auto o = Json::Decode(json);
        CHECK(o.MemberExists("Member1"));
        CHECK(o.MemberExists("Member2"));
        CHECK(o["Member2"].MemberExists("NestedMember"));
        CHECK_EQ(o["Member2"]["NestedMember"].AsString(), "NestedValue");
    }

    SUBCASE("Optionals") {
        auto o = Json::Decode(json_object);

        int int_value;
        CHECK(o.TryAssign(int_value, "IntValue"));
        CHECK_FALSE(o.TryAssign(int_value, "NonExisting"));
        CHECK_EQ(int_value, 42);

        double dbl_value = o.TryGet("FloatValue", 0.54321);
        CHECK_EQ(dbl_value, 1.234567);
        dbl_value = o.TryGet("FloatValueX", 0.54321);
        CHECK_EQ(dbl_value, 0.54321);

        DynamicData js_value = o.TryGet("NonExisting", DynamicData().Add(1).Add(2));
        CHECK(js_value.IsArray());
        CHECK_EQ(js_value[1].AsInt(), 2);
    }

    SUBCASE("Streaming") {
        bool pretty = false;
        std::string raw = R"({"Member1":1234,"Member2":{"NestedMember":"NestedValue"}})";

        SUBCASE("Ugly") {
        }
        SUBCASE("Pretty") {
            raw = R"({
    "Member1": 1234,
    "Member2": {
        "NestedMember": "NestedValue"
    }
})";
            pretty = true;
        }

        std::stringstream ss;
        CHECK_NOTHROW(ss << Json(raw).Encode(pretty));

        CHECK_EQ(ss.str(), raw);

        ss.str("");
        ss << Json::Types::Number;
        CHECK_EQ(ss.str(), "Number");

        JsonStream js(pretty);
        js << Indent(1) << OBegin()
            << Key("Member1") << 1234 << Comma()
            << Key("Member2") << Indent(2) << OBegin()
                << Key("NestedMember") << "NestedValue"
                << Indent(1) << OEnd()
            << Indent(0) << OEnd();
        CHECK_EQ(js.str(), raw);
    }

    SUBCASE("Decode2") {
        std::string raw = R"(
{
    "Token": "",
    "ConfigFingerprint": "",
    "ItemId": 1,
    "Queue": [
        [
            3,
            "api/v1/log-entries",
            {
                "data": [
                    {
                        "device": "SN220001",
                        "severity": {
                            "slug": "critical"
                        },
                        "channel": "",
                        "message": "Unhandled exception: From '/home/steffen/Projects/tgm/tgm-device/common/Application.cpp:63'->\nThe Application::Execute method is not defined.\ntrial-app   (+0x755bb) [0x56453fb365bb])\ntrial-app   (+0x73173) [0x56453fb34173])\ntrial-app   (+0x29916) [0x",
                        "context": null,
                        "timestamp": "2023-05-24T09:01:04.016Z",
                        "context": {
                            "original-message": "Unhandled exception: From '/home/steffen/Projects/tgm/tgm-device/common/Application.cpp:63'->\nThe Application::Execute method is not defined.\ntrial-app   (+0x755bb) [0x56453fb365bb])\ntrial-app   (+0x73173) [0x56453fb34173])\ntrial-app   (+0x29916) [0x56453faea916])\ntrial-app  main (+0x5b) [0x56453fae7012])\nlibc.so   (+0x29d90) [0x7fb3c81f2d90])\nlibc.so  __libc_start_main (+0x80) [0x7fb3c81f2e40])\ntrial-app   (+0x1ffe5) [0x56453fae0fe5])\n\n"
                        }
                    }
                ]
            },
            0
        ]
    ]
})";

        auto o = Json::Decode(raw);

        CHECK(o.MemberExists("Queue"));
        CHECK(o["Queue"].IsArray());
        CHECK_EQ(o["Queue"][0][0].AsInt(), 3);
    }
}

template <typename E, E V, int I> void func_print() {
    MESSAGE(__PRETTY_FUNCTION__);
}

template <typename E, int END, int N = 0>
constexpr void func_print_all() {
    func_print<E, E(N), N>();
    if constexpr (N < END) {
        func_print_all<E, END, N+1>();
    }
}

TEST_CASE("Enum Traversal") {
    func_print<Json::Types, Json::Types::Number, int(Json::Types::Number)>();
    func_print_all<Json::Types, 10>();
}
