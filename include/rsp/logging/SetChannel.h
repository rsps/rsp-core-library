/**
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 *\copyright   Copyright (c) 2023 RSP Systems A/S. All rights reserved.
 *\license     Mozilla Public License 2.0
 *\author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_LOGGING_SET_CHANNEL_H
#define RSP_CORE_LIB_LOGGING_SET_CHANNEL_H

#include <ostream>
#include <string>

namespace rsp::logging {

class SetChannel
{
public:
    /**
     * \brief Construct object with LogLevel value
     *
     * \param aLevel
     */
    explicit SetChannel(std::string aValue) : mValue(std::move(aValue)) {}

    /**
     * \brief Callable operator used by streaming operators
     *
     * \param o
     * \return o
     */
    std::ostream& operator()(std::ostream& o) const;

    std::string mValue;
};

/**
 * \brief Streaming operator for SetChannel object
 *
 * \param o
 * \param aLevel
 * \return o
 */
std::ostream& operator<<(std::ostream &o, const SetChannel &arChannel);

} // rsp::logging

#endif // RSP_CORE_LIB_LOGGING_SET_CHANNEL_H
