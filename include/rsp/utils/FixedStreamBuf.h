/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2026 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 */

#ifndef RSP_CORE_LIB_UTILS_FIXED_STREAM_BUF_H
#define RSP_CORE_LIB_UTILS_FIXED_STREAM_BUF_H

#include <algorithm>
#include <cstddef>
#include <streambuf>

namespace rsp::utils {

/**
 * \brief Streambuf operating directly on a caller-owned, fixed-size memory block.
 *
 * Replaces std::stringbuf::pubsetbuf(), which is implementation defined and
 * ignored by libc++.
 *
 * Writing beyond the capacity fails (overflow returns EOF). No terminating null
 * is written.
 */
class FixedStreamBuf : public std::streambuf
{
public:
    FixedStreamBuf() = default;

    FixedStreamBuf(char* apBuffer, std::streamsize aSize)
    {
        Reset(apBuffer, aSize);
    }

    /**
     * \brief Use the given memory block, discarding any previous content.
     */
    void Reset(char* apBuffer, std::streamsize aSize)
    {
        mWritten = 0;
        setg(apBuffer, apBuffer, apBuffer);
        setp(apBuffer, apBuffer + aSize);
    }

protected:
    int_type underflow() override
    {
        SyncGetArea();
        return (gptr() < egptr()) ? traits_type::to_int_type(*gptr()) : traits_type::eof();
    }

    std::streamsize showmanyc() override
    {
        SyncGetArea();
        return egptr() - gptr();
    }

    pos_type seekoff(off_type aOff, std::ios_base::seekdir aDir, std::ios_base::openmode aWhich) override
    {
        SyncGetArea();
        const bool in = (aWhich & std::ios_base::in) != 0;
        const bool out = (aWhich & std::ios_base::out) != 0;
        if (in == out && aDir == std::ios_base::cur) { // Neither, or both (ambiguous)
            return pos_type(off_type(-1));
        }
        if (!in && !out) {
            return pos_type(off_type(-1));
        }

        off_type base = 0;
        switch (aDir) {
            case std::ios_base::beg:
                base = 0;
                break;
            case std::ios_base::cur:
                base = in ? (gptr() - eback()) : (pptr() - pbase());
                break;
            case std::ios_base::end:
                base = mWritten;
                break;
            default:
                return pos_type(off_type(-1));
        }

        const off_type target = base + aOff;
        const off_type capacity = epptr() - pbase();
        if (target < 0 || target > (in ? mWritten : capacity)) {
            return pos_type(off_type(-1));
        }

        if (in) {
            setg(eback(), eback() + target, eback() + mWritten);
        }
        if (out) {
            char* begin = pbase();
            char* end = epptr();
            setp(begin, end);
            for (off_type left = target; left > 0;) {
                const int step = int(std::min<off_type>(left, 0x7FFFFFFF));
                pbump(step);
                left -= step;
            }
        }
        return pos_type(target);
    }

    pos_type seekpos(pos_type aPos, std::ios_base::openmode aWhich) override
    {
        return seekoff(off_type(aPos), std::ios_base::beg, aWhich);
    }

private:
    // Expose everything written so far through the get area
    void SyncGetArea()
    {
        mWritten = std::max<std::streamoff>(mWritten, pptr() - pbase());
        setg(eback(), gptr(), eback() + mWritten);
    }

    std::streamoff mWritten = 0;
};

} // namespace rsp::utils

#endif // RSP_CORE_LIB_UTILS_FIXED_STREAM_BUF_H
