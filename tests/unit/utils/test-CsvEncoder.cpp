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

static bool Formatter(std::string &arResult, const DynamicData &arValue)
{
    std::string sep;
    if (arValue.IsNull()) {
        ;
    }
    else if (arValue.IsArray()) {
        for (size_t i=0 ; i < arValue.GetCount() ; ++i) {
            arResult += sep;
            Formatter(arResult, arValue[i]);
            sep = "|";
        }
    }
    else if (arValue.IsObject()) {
        for (auto &name : arValue.GetMemberNames()) {
            arResult += sep + name + ":";
            Formatter(arResult, arValue[name]);
            sep = ",";
        }
    }
    else {
        arResult += arValue.AsString();
    }
    return false;
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

    SUBCASE("Bad Data") {
        DynamicData set;
        set.Add(42).Add(43).Add(44);
        CsvEncoder csv;
        CHECK_THROWS_AS(csv.Encode(set), EDynamicTypeError);

        set.Clear();
        DynamicData row;
        row
            .Add("StringValue", "Some text")
            .Add("NumberValue", 42);
        CHECK_THROWS_AS(csv.Encode(row), ENotAnArray);
    }

    SUBCASE("Formatted Data") {
        DynamicData set;
        set
            .Add("Headers", DynamicData())
            .Add("Data", DynamicData());
        set["Headers"].Add("Column1").Add("Column2");

        for (size_t i = 0; i < 10; i+=2) {
            set["Data"].Add(DynamicData().Add(42+i).Add(43+i));
        }

        auto result = CsvEncoder().Encode(set);
//        MESSAGE("Result:\n" << result);
        CHECK_EQ(result, "Column1,Column2\n42,43\n44,45\n46,47\n48,49\n50,51\n");
    }

    SUBCASE("Single Column") {
        DynamicData set;
        set
            .Add("Headers", DynamicData())
            .Add("Data", DynamicData());
        set["Headers"].Add("Column1");
        for (size_t i = 0; i < 5; ++i) {
            set["Data"].Add(42+i); // Data is only array of single values
        }
        auto result = CsvEncoder().Encode(set);
//        MESSAGE("Result:\n" << result);
        CHECK_EQ(result, "Column1\n42\n43\n44\n45\n46\n");
    }
}
