/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include "doctest.h"
#include <sstream>
#include <utils/StrUtils.h>
#undef NDEBUG
#include <utils/CoreException.h>

using namespace rsp::utils;

TEST_CASE("Core Exceptions") {


    SUBCASE("Test backtraced exceptions") {

        std::stringstream result1;
        std::stringstream result2;
        std::stringstream result3;
        std::stringstream result4;

        try {
            try {
                try {
                    try {
                        try {
                            THROW_WITH_BACKTRACE1(CoreException, "This is an example!");
                        }
                        catch(const std::exception &e) {
                            throw; // Check if type changes by re-throwing same
                        }
                    }
                    catch(const CoreException &e) {
                        result1 << "First caught this exception:\n" << e.what()
                                << std::endl;
                        RETHROW_WITH_BACKTRACE("Rethrowing of type CoreException: ", e);
                    };
                }
                catch (const CoreException &e) {  // can also catch normally.
                    result2 << "Got exception of type " << typeid(e).name() << ":\n" << e.what() << std::endl;
                    // and even rethrow again, with backtrace:
                    RETHROW_WITH_BACKTRACE("Rethrowing as type rsp::utils::CoreException: ", e);
                };
            }
            catch (const std::runtime_error &e) {  // can also catch normally.
                result3 << "Got exception of type " << typeid(e).name() << ":\n" << e.what() << std::endl;
                // and even rethrow again, with backtrace:
                RETHROW_WITH_BACKTRACE("Rethrowing as type std::runtime_error: ", e);
            };
        }
        catch (const std::exception &e) {
            result4 << "Finally, got this exception of type " << typeid(e).name() << ":\n" << e.what() << std::endl;
        };

        MESSAGE(result1.str());
        CHECK(StrUtils::EndsWith(result1.str(), "This is an example!\n") == true);
        CHECK(StrUtils::Contains(result1.str(), "test-exceptions.cpp:34") == true);

        MESSAGE(result2.str());
        CHECK(StrUtils::EndsWith(result2.str(), "This is an example!\n") == true);
        CHECK(StrUtils::Contains(result2.str(), "test-exceptions.cpp:43") == true);

        MESSAGE(result3.str());
        CHECK(StrUtils::EndsWith(result3.str(), "This is an example!\n") == true);
        CHECK(StrUtils::Contains(result3.str(), "test-exceptions.cpp:49") == true);

        MESSAGE(result4.str());
        CHECK(StrUtils::EndsWith(result4.str(), "This is an example!\n") == true);
        CHECK(StrUtils::Contains(result4.str(), "test-exceptions.cpp:55") == true);
    }


    SUBCASE("Test backtraced system exception") {
        std::stringstream result1;
        errno = EIO;
        try {
            THROW_SYSTEM("File IO error");
        }
        catch(const std::exception &e) {
            result1 << "Caught exception:\n" << e.what() << std::endl;
        }

        MESSAGE(result1.str());
        CHECK(StrUtils::EndsWith(result1.str(), "File IO error: Input/output error\n") == true);
        CHECK(StrUtils::Contains(result1.str(), "test-exceptions.cpp:84") == true);
    }

    SUBCASE("Assertions") {
        std::stringstream result1;
        try {
            ASSERT(true == false);
        }
        catch(const std::exception &e) {
            result1 << "Caught exception:\n" << e.what() << std::endl;
        }

        MESSAGE(result1.str());
        CHECK(StrUtils::EndsWith(result1.str(), "true == false\n") == true);
        CHECK(StrUtils::Contains(result1.str(), "test-exceptions.cpp:98") == true);

        CHECK_NOTHROW(ASSERT(true));
    }
}


