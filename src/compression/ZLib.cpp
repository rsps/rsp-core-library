/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <rsp/compression/ZLib.h>

#include <string>
#include <vector>
#include <iostream>

#ifdef RSP_CORE_LIB_USE_ZLIB
#define ZLIB_CONST
#include <zlib.h>
#endif // RSP_CORE_LIB_USE_ZLIB

namespace rsp::compression {

#ifdef RSP_CORE_LIB_USE_ZLIB

namespace {

std::string formatError(const char* apMsg, int aErrorCode)
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

} // namespace

class ZLib::Impl
{
public:
    explicit Impl(size_t aBufferSize)
        : _buffer(aBufferSize)
    {
        // init the decompression stream
        _zstream.zalloc = Z_NULL;
        _zstream.zfree = Z_NULL;
        _zstream.opaque = Z_NULL;
        _zstream.avail_in = 0;
        _zstream.next_in = Z_NULL;

        if (int ret = ::inflateInit(&_zstream); ret != Z_OK) {
            THROW_WITH_BACKTRACE2(ZlibException, "inflateInit", ret);
        }
    }

    void Inflate(std::span<const uint8_t> aData)
    {
        // Set the starting point and total data size to be read
        _zstream.avail_in = aData.size();
        _zstream.next_in = aData.data();

        // Start decompressing
        while (_zstream.avail_in != 0) {
            _zstream.next_out = _buffer.data();
            _zstream.avail_out = _buffer.size();

            int ret = ::inflate(&_zstream, Z_NO_FLUSH);

            size_t produced = _buffer.size() - _zstream.avail_out;
            _result.insert(_result.end(), _buffer.begin(), _buffer.begin() + std::ptrdiff_t(produced));

            if (ret == Z_STREAM_END) {
                break;
            }
            else if (ret != Z_OK) {
                std::cerr << "Available: " << _zstream.avail_in << ", buffer: " << _buffer.size() << std::endl;
                THROW_WITH_BACKTRACE2(ZlibException, "inflate", ret);
            }
        }

        if (int ret = ::inflateEnd(&_zstream); ret != Z_OK) {
            THROW_WITH_BACKTRACE2(ZlibException, "inflateEnd", ret);
        }
    }

    utils::Version GetLibraryVersion() const
    {
        return utils::Version{::zlibVersion()};
    }

    const std::vector<uint8_t>& GetResult() const
    {
        return _result;
    }

private:
    z_stream _zstream{};
    std::vector<uint8_t> _buffer{};
    std::vector<uint8_t> _result{};
};

#else // !RSP_CORE_LIB_USE_ZLIB

namespace {

std::string formatError(const char* apMsg, int aErrorCode)
{
    return std::string("Error in ZLib: ") + apMsg + " (" + std::to_string(aErrorCode) + ")";
}

} // namespace

// Dummy implementation when ZLib support is not compiled in
class ZLib::Impl
{
public:
    explicit Impl(size_t)
    {
        THROW_WITH_BACKTRACE1(exceptions::NotImplementedException, "ZLib support is not compiled into this library (RSP_CORE_LIB_USE_ZLIB is OFF)");
    }

    void Inflate(std::span<const uint8_t>)
    {
        THROW_WITH_BACKTRACE1(exceptions::NotImplementedException, "ZLib support is not compiled into this library (RSP_CORE_LIB_USE_ZLIB is OFF)");
    }

    const std::vector<uint8_t>& GetResult() const
    {
        THROW_WITH_BACKTRACE1(exceptions::NotImplementedException, "ZLib support is not compiled into this library (RSP_CORE_LIB_USE_ZLIB is OFF)");
    }

    utils::Version GetLibraryVersion() const
    {
        THROW_WITH_BACKTRACE1(exceptions::NotImplementedException, "ZLib support is not compiled into this library (RSP_CORE_LIB_USE_ZLIB is OFF)");
    }
};

#endif // RSP_CORE_LIB_USE_ZLIB

ZlibException::ZlibException(const char* apMsg, int aErrorCode)
    : CoreException(formatError(apMsg, aErrorCode))
{
}

ZLib::ZLib(size_t aBufferSize)
    : _impl(std::make_unique<Impl>(aBufferSize))
{
}

ZLib::~ZLib() = default;

void ZLib::Inflate(std::span<const uint8_t> aData)
{
    _impl->Inflate(aData);
}

const std::vector<uint8_t>& ZLib::GetResult() const
{
    return _impl->GetResult();
}

utils::Version ZLib::GetLibraryVersion() const
{
    return _impl->GetLibraryVersion();
}

} // namespace rsp::compression
