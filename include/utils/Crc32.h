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
#include <stdexcept>

namespace rsp {


class ECrcError : public std::runtime_error {
public:
    ECrcError(std::string aMsg) : std::runtime_error(aMsg) {};
};

class Crc32 {
public:
    static std::uint32_t Calc(const void* aBuf, std::size_t aLen, std::uint32_t aInitial = 0);

protected:
    static std::uint32_t* getTable();
};


}; /* namespace rsp */

#endif /* SRC_UTILS_CRC32_H_ */
