/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_UTILS_CRC32_H_
#define SRC_UTILS_CRC32_H_

#include <cstdint>
#include "CoreException.h"

namespace rsp::utils {

/**
 * \class ECrcError
 * \brief Exception thrown on CRC mismatch
 *
 */
class ECrcError : public CoreException {
public:
    ECrcError(const char *apMsg) : CoreException(apMsg) {};
};

/**
 * \class Crc32
 * \brief Implementation of the CRC32b algorithm.
 */
class Crc32 {
public:
    /**
     * \fn  Crc32(uint32_t=0)
     * \brief Constructor of a Crc32 object, with default seed of 0.
     *
     * \param aInitial
     */
    Crc32(uint32_t aInitial = 0);

    /**
     * \fn std::uint32_t Calc(const void*, std::size_t, std::uint32_t=0)
     * \brief A standalone method to calculate the CRC32 of any buffer.
     *
     * \param aBuf
     * \param aLen
     * \param aInitial
     * \return
     */
    static std::uint32_t Calc(const void* aBuf, std::size_t aLen, std::uint32_t aInitial = 0);

    /**
     * \fn std::uint32_t Add(uint8_t)
     * \brief An object method for incremental calculation of a CRC32 value.
     *
     * \param aByte
     * \return
     */
    std::uint32_t Add(uint8_t aByte);

    /**
     * \fn bool Verify(uint32_t, bool=false)
     * \brief Verifies the current CRC value with the given result.
     * \throws ECrcError It can optionally throw an exception on mismatching CRC values.
     *
     * \param aResult
     * \param aThrowOnMismatch
     * \return
     */
    bool Verify(uint32_t aResult, bool aThrowOnMismatch = false);

protected:
    static std::uint32_t* getTable();
    uint32_t mC{0};
};


} /* namespace rsp */

#endif /* SRC_UTILS_CRC32_H_ */
