/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <doctest.h>
#include <utils/Config.h>
#include <utils/FixedString.h>
#include <json/Json.h>
#include <TestHelpers.h>

using namespace rsp::utils;
using namespace rsp::json;
using namespace rsp::logging;

struct ConfigData {
    FixedString<100> ApplicationName{};
    int PlacementCount = 3;
    int Power = 280;
    double Pi = 3.141592653589793238462643383279;
};

class MyConfig : public Config<ConfigData>
{
public:
    std::unique_ptr<JsonValue> ToJson() const override
    {
        std::unique_ptr<JsonValue> result = std::make_unique<JsonObject>();

        result->AsObject().Add("ApplicationName", new JsonValue(Get().ApplicationName));
        result->AsObject().Add("PlacementCount", new JsonValue(Get().PlacementCount));
        result->AsObject().Add("Power", new JsonValue(Get().Power));
        result->AsObject().Add("Pi", new JsonValue(Get().Pi));

        return result;
    }

    void FromJson(const JsonValue &arJson) override
    {
        Get().ApplicationName = arJson.AsObject()["ApplicationName"].AsString();
        Get().PlacementCount = arJson.AsObject()["PlacementCount"];
        Get().Power = arJson.AsObject()["Power"];
        Get().Pi = arJson.AsObject()["Pi"];
    }

    void Validate() override
    {
        Validator::NotEmpty(Get().ApplicationName);
        Validator::InRange(1, 6, Get().PlacementCount);
        Validator::InRange(0, 400, Get().Power);
        Validator::InRange(3.140, 3.142, Get().Pi);
    }
};

static void TamperWithFile(const std::string& arFileName, std::size_t aOffset, std::uint8_t aValue)
{
    rsp::posix::FileIO f(arFileName, std::ios_base::in | std::ios_base::out);
    f.Seek(aOffset); // Offset to PlacementCount
    f.ExactWrite(&aValue, sizeof(aValue));
}

TEST_CASE("Config")
{
    const char* cFileName = "config.bin";
    const char* cShaSeed = "HashSeed";

    Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    MyConfig config;
    config.Get().ApplicationName = "ConfigurationApp";

    CHECK_EQ(config.GetHeader().Flags, ContainerFlags::Extended);

    CHECK_NOTHROW(config.Validate());

    SUBCASE("Json")
    {
        std::string json = config.ToJson()->Encode();

//        MESSAGE(json);
        CHECK_EQ(json, R"({"ApplicationName":"ConfigurationApp","PlacementCount":3,"Power":280,"Pi":3.141593})");

        config.Get().PlacementCount = 7;
        CHECK_THROWS_AS(config.Validate(), ValidatorException);

        Json js;
        js.Decode(json);
        config.FromJson(js.Get());
    }

    SUBCASE("Storage")
    {
        config.Save(cFileName, cShaSeed);

        config.Get().ApplicationName = "";
        CHECK_THROWS_AS(config.Validate(), ValidatorException);

        CHECK_NOTHROW(config.Load(cFileName, cShaSeed));
    }

    SUBCASE("CRC Integrity") {
        /*
         * Tamper with config file, increase PlacementCount to 4
         */
        TamperWithFile(cFileName, 0xb4, 4);
        CHECK_THROWS_AS(config.Load(cFileName, cShaSeed), EInvalidCRC);

        TamperWithFile(cFileName, 0xb4, 3);
        CHECK_NOTHROW(config.Load(cFileName, cShaSeed));
    }

    SUBCASE("Signature Integrity") {
        /*
         * Tamper with config file: CRC takes precedence, so invalidate signature directly
         */
        TamperWithFile(cFileName, 0x10, 0x58);
        CHECK_THROWS_AS(config.Load(cFileName, cShaSeed), EInvalidSignature);

        TamperWithFile(cFileName, 0x10, 0x59);
        CHECK_NOTHROW(config.Load(cFileName, cShaSeed));
    }

    CHECK_NOTHROW(config.Validate());
}

