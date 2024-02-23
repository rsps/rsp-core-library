/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/

#include "doctest.h"
#include <utils/CsvEncoder.h>

using namespace rsp::utils;

static std::string Formatter(const DynamicData &arValue)
{
    std::string sep;
    std::string result;
    if (arValue.IsNull()) {
        ;
    }
    else if (arValue.IsArray()) {
        for (size_t i=0 ; i < arValue.GetCount() ; ++i) {
            result += sep + Formatter(arValue[i]);
            sep = "|";
        }
    }
    else if (arValue.IsObject()) {
        for (auto &name : arValue.GetMemberNames()) {
            result += sep + name + ":" + Formatter(arValue[name]);
            sep = ",";
        }
    }
    else {
        result = arValue.AsString();
    }
    return result;
}

TEST_CASE("CSV")
{
    SUBCASE("Construct") {
        CsvEncoder csv1;
        CsvEncoder csv2(false);
        CsvEncoder csv3(true, ',', false);
    }

    SUBCASE("Encode") {
        DynamicData row;
        row
            .Add("StringValue", "Some text")
            .Add("NumberValue", 42)
            .Add("ArrayValue", DynamicData())
            .Add("ObjectValue", DynamicData());

        DynamicData set;
        set.Add(row);
        row["NumberValue"] = 43;
        row["StringValue"] = "NotAvailable";
        row["ArrayValue"].Add(10).Add("Twenty").Add(true);
        row["ObjectValue"].Add("First", 1).Add("Second", "Two");
        set.Add(row);

        CsvEncoder csv;
        auto result = csv.SetValueFormatter(&Formatter).Encode(set);
        MESSAGE("Result:\n" << result);
        CHECK_EQ(result, "String Value,Number Value,Array Value,Object Value\nSome text,42,,\nN/A,43,10|Twenty|true,\"First:1,Second:Two\"\n");
    }
}
