/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_LOGGING_SET_LEVEL_H
#define RSP_CORE_LIB_LOGGING_SET_LEVEL_H

#include <ostream>
#include "LogTypes.h"

namespace rsp::logging {

/**
 * \class SetLevel
 * \brief Stream manipulator to set the log accept level of log streams
 *
 */
class SetLevel
{
public:
    /**
     * \brief Construct object with LogLevel value
     *
     * \param aLevel
     */
    explicit SetLevel(LogLevel aLevel) : mValue(aLevel) {}

    /**
     * \brief Callable operator used by streaming operators
     *
     * \param o
     * \return
     */
    std::ostream& operator()(std::ostream& o) const;

    LogLevel mValue;
};

/**
 * \brief Streaming operator for SetLevel object
 *
 * \param o
 * \param aLevel
 * \return
 */
std::ostream& operator<<(std::ostream &o, SetLevel aLevel);

} /* namespace rsp::logging */

#endif // RSP_CORE_LIB_LOGGING_SET_LEVEL_H
