/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <iomanip>
#include <security/SecureBuffer.h>

namespace rsp::security {

std::ostream& operator<<(std::ostream& os, const SecureBuffer &arBuffer)
{
    os << std::hex << std::setfill('0');
    for (auto c : arBuffer) {
//        os << "[" << std::setw(2) << static_cast<int>(c) << "] " << c << "\n";
        os << std::setw(2) << static_cast<int>(c);
    }
    return os << std::dec;
}

} // rsp::security


