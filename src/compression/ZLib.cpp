/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <compression/ZLib.h>

#ifdef USE_ZLIB

namespace rsp::compression {

std::string ZlibException::formatError(const char *apMsg, int aErrorCode)
{
    std::string result("Error in ZLib: ");
    result += std::string(apMsg) + " (" + std::to_string(aErrorCode) + "): ";

    switch (aErrorCode) {
        case Z_OK:
            result += "OK";
            break;

        case Z_STREAM_END:
            result += "End of stream";
            break;

        case Z_NEED_DICT:
            result += "Need dictionary";
            break;

        case Z_ERRNO:
            result += "Errno: " + std::to_string(errno);
            break;

        case Z_STREAM_ERROR:
            result += "Stream error";
            break;

        case Z_DATA_ERROR:
            result += "Data error";
            break;

        case Z_MEM_ERROR:
            result += "Memory error";
            break;

        case Z_BUF_ERROR:
            result += "Buffer error";
            break;

        case Z_VERSION_ERROR:
            result += "Version error";
            break;

        default:
            result += "Unknown error!!";
            break;
    }
    return result;
}

ZLib::ZLib(size_t aBufferSize)
    : mBuffer(aBufferSize)
{
    // init the decompression stream
    mZStream.zalloc = Z_NULL;
    mZStream.zfree = Z_NULL;
    mZStream.opaque = Z_NULL;
    mZStream.avail_in = 0;
    mZStream.next_in = Z_NULL;

    //    int ret = inflateInit2_(&mZStream, -MAX_WBITS, ZLIB_VERSION, int(sizeof(z_stream)));
    int ret = inflateInit(&mZStream);
    if (ret != Z_OK) {
        THROW_WITH_BACKTRACE2(ZlibException, "inflateInit", ret);
    }
}

void ZLib::Inflate(const std::uint8_t *apData, size_t aSize)
{
    int ret;

    // Set the starting point and total data size to be read
    mZStream.avail_in = aSize;
    mZStream.next_in = apData;

    // Start decompressing
    while (mZStream.avail_in != 0) {
        mZStream.next_out = mBuffer.data();
        mZStream.avail_out = mBuffer.size();

        ret = inflate(&mZStream, Z_NO_FLUSH);

        if (ret == Z_STREAM_END) {
            // only store the data we have left in the stream
            mResult.write(reinterpret_cast<const char*>(mBuffer.data()), std::streamsize(mBuffer.size() - mZStream.avail_out));
            break;
        }
        else if (ret == Z_OK) {
            mResult.write(reinterpret_cast<const char*>(mBuffer.data()), static_cast<std::streamsize>(mBuffer.size()));
        }
        else {
            std::cerr << "Available: " << mZStream.avail_in << ", buffer: " << mBuffer.size() << std::endl;
            THROW_WITH_BACKTRACE2(ZlibException, "inflate", ret);
        }
    }

    ret = inflateEnd(&mZStream);
    if (ret != Z_OK) {
        THROW_WITH_BACKTRACE2(ZlibException, "inflateEnd", ret);
    }
}

const std::ostream& ZLib::GetResult()
{
    return mResult;
}

} /* namespace rsp::compression */

#endif /* USE_ZLIB */
