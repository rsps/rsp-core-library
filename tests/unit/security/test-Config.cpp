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
#include <utils/FixedString.h>
#include <json/Json.h>
#include <security/Config.h>
#include <TestHelpers.h>

using namespace rsp::utils;
using namespace rsp::json;
using namespace rsp::logging;
using namespace rsp::security;

struct ConfigData {
    FixedString<100> ApplicationName{};
    int PlacementCount = 3;
    int Power = 280;
    double Pi = 3.141592653589793238462643383279;
};

class MyConfig : public Config<ConfigData>
{
public:
    using Config<ConfigData>::Config;

    JsonValue ToJson() const override
    {
        JsonValue result;

        result.Add("ApplicationName", Get().ApplicationName);
        result.Add("PlacementCount", Get().PlacementCount);
        result.Add("Power", Get().Power);
        result.Add("Pi", Get().Pi);

        return result;
    }

    void FromJson(const JsonValue &arJson) override
    {
        Get().ApplicationName = arJson["ApplicationName"].AsString();
        Get().PlacementCount = arJson["PlacementCount"];
        Get().Power = arJson["Power"];
        Get().Pi = arJson["Pi"];
    }

    void Validate() override
    {
        Validator::NotEmpty(Get().ApplicationName);
        Validator::InRange(1, 6, Get().PlacementCount);
        Validator::InRange(0, 400, Get().Power);
        Validator::InRange(3.140, 3.142, Get().Pi);
    }
};

TEST_CASE("Config")
{
    const char* cFileName = "config.bin";
    const std::string cShaSeed = "HashSeed";
    SecureBuffer sb_hash(cShaSeed.data(), cShaSeed.size());

    Logger logger;
    TestHelpers::AddConsoleLogger(logger);


    MyConfig config(cFileName, sb_hash);
    config.Get().ApplicationName = "ConfigurationApp";

    CHECK_NOTHROW(config.Validate());

    SUBCASE("Json")
    {
        std::string json;
        CHECK_NOTHROW(json = config.ToJson().Encode());

//        MESSAGE(json);
        CHECK_EQ(json, R"({"ApplicationName":"ConfigurationApp","PlacementCount":3,"Power":280,"Pi":3.1415926535897931})");

        config.Get().PlacementCount = 7;
        CHECK_THROWS_AS(config.Validate(), ValidatorException);

        json = R"({"ApplicationName":"ConfigurationApp","PlacementCount":"NotANumber","Power":280,"Pi":3.1415926535897931,"ExtraValue":1.2345})";
        Json js;
        CHECK_NOTHROW(js = Json::Decode(json));
        CHECK_NOTHROW(config.FromJson(js));
        CHECK_THROWS_AS(config.Validate(), ValidatorException);

        json = R"({"ApplicationName":"ConfigurationApp","PlacementCount":"3","Power":280,"Pi":3.1415926535897931,"ExtraValue":1.2345})";
        CHECK_NOTHROW(js.Clear());
        CHECK_NOTHROW(js = js.Decode(json));
        CHECK_NOTHROW(config.FromJson(js));
    }

    SUBCASE("Storage")
    {
        config.Save();

        config.Get().ApplicationName = "";
        CHECK_THROWS_AS(config.Validate(), ValidatorException);

        CHECK_NOTHROW(config.Load());
    }

    SUBCASE("Signature Integrity") {
        /*
         * Tamper with config file. We change a single bit.
         */
        auto bitmask = TestHelpers::TamperWithFile(cFileName, config.GetSignature().GetSize() + 3, 0);
        bitmask ^= 0x04;
        TestHelpers::TamperWithFile(cFileName, config.GetSignature().GetSize() + 3, bitmask);
        CHECK_THROWS_AS(config.Load(), EInvalidSignature);

        bitmask ^= 0x04;
        TestHelpers::TamperWithFile(cFileName, config.GetSignature().GetSize() + 3, bitmask);
        CHECK_NOTHROW(config.Load());
    }

    CHECK_NOTHROW(config.Validate());
}

