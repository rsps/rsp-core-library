/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2025 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#include <doctest.h>
#include <string_view>
#include <network/BinaryBody.h>
#include <network/FileBody.h>
#include <network/StringBody.h>
#include <network/ChunkStreamer.h>

using namespace rsp::network;

constexpr std::string_view cPayload = R"(
This is a string literal containing a bigger
text, used as payload for a chunked data transfer.
This could have contained a lot of the usual
"Lorem ipsum dolor sit amet, consetetur sadipscing elitr,
sed diam nonumy eirmod tempor invidunt ut labore et dolore
magna aliquyam erat, sed diam voluptua. At vero eos et accusam
et justo duo dolores et ea rebum. Stet clita kasd gubergren,
no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem
ipsum dolor sit amet, consetetur sadipscing elitr, sed diam
nonumy eirmod tempor invidunt ut labore et dolore magna
aliquyam erat, sed diam voluptua. At vero eos et accusam et
justo duo dolores et ea rebum. Stet clita kasd gubergren, no
sea takimata sanctus est Lorem ipsum dolor sit amet."
nonsense...
)";
constexpr size_t cPartial = cPayload.size() / 4;

class ChunkDataProvider : public IChunkedDataProvider
{
public:
    [[nodiscard]] bool GetChunk(std::span<char> aBuffer, size_t &arBytesWritten, size_t &arChunkIndex, size_t &arPayloadIndex) override
    {
        switch (arChunkIndex) {
            case 0:
                // Header...
                ASSERT(aBuffer.size() >= 96);
                std::memcpy(aBuffer.data(), cPayload.data(), 96);
                arBytesWritten = 96;
                arChunkIndex++;
                mOffset = 96;
                return false;

            case 1: {
                auto sz = std::min(aBuffer.size(), cPartial);
                sz = std::min(sz, cPayload.size() - mOffset);

                std::memcpy(aBuffer.data(), cPayload.data() + mOffset, sz);
                mOffset += sz;
                arBytesWritten = sz;
                arPayloadIndex++;
                return (sz == 0);
            }

            default:
                break;
        }
        return false;
    }

    [[nodiscard]] size_t GetSize() override
    {
        return cPayload.size();
    }

protected:
    size_t mOffset = 0;
};

TEST_SUITE_BEGIN("Network");

TEST_CASE("Data Providers")
{
    const std::string cTestFile("./test.txt");

    SUBCASE("Constructors") {
        CHECK_NOTHROW(StringBody sb{std::string(cPayload)});
        CHECK_NOTHROW(BinaryBody bb);
        rsp::posix::FileIO file(cTestFile, std::ios_base::out, 0664);
        CHECK_NOTHROW(FileBody fb(file));
        CHECK_NOTHROW(ChunkDataProvider provider);
    }

    SUBCASE("StringBody") {
        StringBody sb{std::string(cPayload)};
        std::string result(cPayload.size(), '*');
        auto len = sb.Read({ reinterpret_cast<std::byte*>(result.data()), result.size() });

        CHECK_EQ(len, cPayload.size());
        CHECK_EQ(result, cPayload);
        CHECK_EQ(sb.Get(), cPayload);
    }

    SUBCASE("BinaryBody") {
        BinaryBody bb;
        bb.Write({reinterpret_cast<const std::byte*>(cPayload.data()), cPayload.size()});

        std::string result(cPayload.size(), '*');
        auto len = bb.Read({ reinterpret_cast<std::byte*>(result.data()), result.size() });

        CHECK_EQ(len, cPayload.size());
        CHECK_EQ(result, cPayload);
        CHECK_EQ(bb.Get().Get().str(), cPayload);
    }

    SUBCASE("FileBody") {
        { // Write to file through FileBody provider
            rsp::posix::FileIO file(cTestFile, std::ios_base::out, 0664);
            FileBody fb(file);
            CHECK_EQ(fb.Write({reinterpret_cast<const std::byte*>(cPayload.data()), cPayload.size()}), cPayload.size());
            CHECK_NOTHROW(file.Close());
        }

        { // Verify file contents
            rsp::posix::FileIO f(cTestFile, std::ios_base::in);
            std::string result = f.GetContents();
            CHECK_NOTHROW(f.Close());
            CHECK_EQ(result.size(), cPayload.size());
            CHECK_EQ(result, cPayload);
        }

        { // Read from file through FileBody provider
            rsp::posix::FileIO file(cTestFile, std::ios_base::in);
            FileBody fb(file);
            std::string result(cPayload.size(), '*');
            CHECK_EQ(fb.Read({reinterpret_cast<std::byte*>(result.data()), result.size()}), cPayload.size());
            CHECK_NOTHROW(file.Close());
            CHECK_EQ(result.size(), cPayload.size());
            CHECK_EQ(result, cPayload);
        }

        { // Copy from file through FileBody to StringBody
            rsp::posix::FileIO file(cTestFile, std::ios_base::in);
            FileBody fb(file);
            StringBody sb;
            CHECK_EQ(sb.CopyFrom(fb), cPayload.size());
            CHECK_NOTHROW(file.Close());
            CHECK_EQ(sb.Get().size(), cPayload.size());
            CHECK_EQ(sb.Get(), cPayload);
        }
    }

    SUBCASE("ChunkStreamer") {
        ChunkDataProvider provider;
        ChunkStreamer<128> cs(provider); // Internal fifo must be bigger than minimum chunk requirement. (96 bytes for ChunkDataProvider)

        std::string result;
        std::array<char, 64> buffer{};
        while (auto sz = cs.Read({reinterpret_cast<std::byte*>(buffer.data()), buffer.size()})) {
            CHECK_EQ(cs.GetWritten(), sz);
            result += std::string(buffer.data(), sz);
        }

        CHECK_EQ(result.size(), cPayload.size());
        CHECK_EQ(result, cPayload);
        CHECK_EQ(cs.GetChunkIndex(), 1);
        CHECK_EQ(cs.GetPayloadIndex(), 8);
    }

}

TEST_SUITE_END();
