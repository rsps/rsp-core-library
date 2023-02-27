/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_LOGGING_LOGWRITER_H_
#define SRC_LOGGING_LOGWRITER_H_

#include <string>
#include <utils/DynamicData.h>
#include "LogTypes.h"

namespace rsp::logging {


/**
 * \class LogWriterInterface
 * \brief Interface definition for a log writer
 */
class LogWriterInterface {
public:
    virtual ~LogWriterInterface() {}

    /**
     * Write a string to the destination in a thread safe manner.
     *
     * \param arMsg
     * \param aCurrentLevel
     */
    virtual void Write(const std::string &arMsg, LogLevel aCurrentLevel, const std::string &arChannel, const rsp::utils::DynamicData &arContext) = 0;

    /**
     * Set the log acceptance level of this writer.
     *
     * \param aLevel
     */
    void SetAcceptLogLevel(LogLevel aLevel) { mAcceptLevel = aLevel; }

protected:
    LogLevel mAcceptLevel = cDefautLogLevel;
};

} /* namespace logging */

#endif /* SRC_LOGGING_LOGWRITER_H_ */
