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
#include <string>
#include <sstream>
#include <utils/StrUtils.h>
#include <utils/InsertOrderedMap.h>

using namespace rsp::utils;

TEST_CASE("InsertOrderedMap") {

    SUBCASE("int,string") {
        InsertOrderedMap<int,std::string> map{
            {2, "two"},
            {0, "zero"},
            {3, "three"}
        };
        map[1] = "one";

        CHECK_EQ(map[1], "one");
        CHECK_EQ(map[3], "three");

        std::stringstream ss;
        for(auto key : map.GetOrderList()) {
            ss << key.get() << ",";
        }
        MESSAGE(ss.str());

        ss.str("");
        for(auto key : map.GetOrderList()) {
            ss << map.at(key.get()) << ",";
        }
        MESSAGE(ss.str());
        CHECK_EQ(ss.str(), "two,zero,three,one,");
    }

    SUBCASE("string,string") {
        InsertOrderedMap<std::string,std::string> map{
            {"2", "two"},
            {"0", "zero"},
            {"3", "three"}
        };
        map["1"] = "one";

        CHECK_EQ(map["1"], "one");
        CHECK_EQ(map["3"], "three");

        std::stringstream ss;
        for(auto key : map.GetOrderList()) {
            ss << map.at(key.get()) << ",";
        }
        MESSAGE(ss.str());
        CHECK_EQ(ss.str(), "two,zero,three,one,");

        SUBCASE("remove zero") {
            ss.str("");
            map.Remove("0");
            for(auto key : map.GetOrderList()) {
                ss << map.at(key.get()) << ",";
            }
            MESSAGE(ss.str());
            CHECK_EQ(ss.str(), "two,three,one,");
        }
    }

}

