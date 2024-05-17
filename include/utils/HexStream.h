/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_UTILS_HEX_STREAM_H
#define RSP_CORE_LIB_UTILS_HEX_STREAM_H

#include <ostream>

namespace rsp::utils {

class HexStream
{
public:
    /**
     * \brief Helper class to stream data into hex values
     * \param apData Pointer to data
     * \param aSize Length of data
     */
    HexStream(const uint8_t *apData, size_t aSize, size_t aElementSize)
        : mpData(apData),
          mDataSize(aSize / aElementSize),
          mSizeOf(aElementSize)
    {
    }

    /**
     * \brief Template conversion constructor to take any type object as argument
     * \tparam T
     * \param arT
     */
    template <class T>
    HexStream(const T &arT) : HexStream(arT.data(), arT.size(), 1) {} // NOLINT, Conversion constructor

    /**
     * \brief Set the size of the formatted hex elements in output
     * \param aSize
     * \return self
     */
    HexStream& SizeOfValue(size_t aSize) { mSizeOf = aSize; return *this; }

    /**
     * \brief Set the elements count per line
     * \param aCount
     * \return self
     */
    HexStream& ElementsPerLine(size_t aCount) { mElementsPerLine = aCount; return *this; }

protected:
    friend std::ostream& operator<<(std::ostream &o, const HexStream &arHX);

    const uint8_t *mpData;
    size_t mDataSize;
    size_t mSizeOf = 1;
    size_t mElementsPerLine = 16;
};

std::ostream& operator<<(std::ostream &os, const HexStream &arHX);

} /* namespace rsp::utils */

#endif // RSP_CORE_LIB_UTILS_HEX_STREAM_H
