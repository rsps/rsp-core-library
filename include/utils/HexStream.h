/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_UTILS_HEXSTREAM_H_
#define INCLUDE_UTILS_HEXSTREAM_H_

#include <ostream>

namespace rsp::utils {

class HexStream
{
public:
    HexStream(const std::uint8_t *apData, size_t aSize)
        : mpData(apData),
          mDataSize(aSize)
    {
    }

    template <class T>
    HexStream(const T &arT) : HexStream(arT.data(), arT.size()) {}

    HexStream& SizeOfValue(size_t aSize) { mSizeOf = aSize; return *this; }
    HexStream& ElementsPerLine(size_t aCount) { mElementsPerLine = aCount; return *this; }

protected:
    friend std::ostream& operator<<(std::ostream &o, const HexStream &arHX);

    const std::uint8_t *mpData;
    size_t mDataSize;
    size_t mSizeOf = 1;
    size_t mElementsPerLine = 16;
};

std::ostream& operator<<(std::ostream &os, const HexStream &arHX);

} /* namespace rsp::utils */

#endif /* INCLUDE_UTILS_HEXSTREAM_H_ */
