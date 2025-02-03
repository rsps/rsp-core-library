/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include <doctest.h>
#include <network/MultipartBoundary.h>

using namespace rsp::network;

TEST_SUITE_BEGIN("Network");

TEST_CASE("MultipartBoundary")
{
    MultipartBoundary mb1;
    CHECK_EQ(mb1.GetBoundary().size(), 32);
    MESSAGE("Random boundary: " << mb1.GetBoundary());
    for (auto c: mb1.GetBoundary()) {
        CHECK(std::isprint(c));
    }

    MultipartBoundary mb("ABC");
    CHECK_EQ(mb.GetBoundary(), "ABC");

    CHECK_EQ(mb.GetContentTypeHeader(), std::string("multipart/form-data; boundary=ABC"));

    CHECK_EQ(mb.MakeContentDisposition(""), std::string("\r\n--ABC\r\n\r\n"));
    CHECK_EQ(mb.MakeContentDisposition("Field1"), std::string("\r\n--ABC\r\nContent-Disposition: form-data; name=\"Field1\"\r\n\r\n"));
    CHECK_EQ(mb.MakeContentDisposition("File", "my-file.bin"), std::string("\r\n--ABC\r\nContent-Disposition: form-data; name=\"File\"; filename=\"my-file.bin\"\r\n\r\n"));
    CHECK_EQ(mb.MakeContentDisposition("File", "my-file.bin", "application/octet-stream"),
             std::string("\r\n--ABC\r\nContent-Disposition: form-data; name=\"File\"; filename=\"my-file.bin\"\r\nContent-Type: application/octet-stream\r\n\r\n"));

    CHECK_EQ(mb.GetEndBoundary(), std::string("\r\n--ABC--"));
}

TEST_SUITE_END();
