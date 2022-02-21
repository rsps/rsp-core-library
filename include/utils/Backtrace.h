/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef INCLUDE_UTILS_BACKTRACE_H_
#define INCLUDE_UTILS_BACKTRACE_H_

#include <string>

namespace rsp::utils {

class Backtrace
{
public:
    static void Print();

    static std::string Get(int skip = 1);
};

} /* namespace rsp::utils */

#endif /* INCLUDE_UTILS_BACKTRACE_H_ */
