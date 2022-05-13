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

