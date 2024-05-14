/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_LOGGING_SYS_LOG_WRITER_H
#define RSP_CORE_LIB_LOGGING_SYS_LOG_WRITER_H

#include <logging/LogWriterInterface.h>

namespace rsp::logging {

/**
 * Definitions copied from syslog.h
 */
/* facility codes */
#define	LOG_KERN	(0<<3)	/* kernel messages */
#define	LOG_USER	(1<<3)	/* random user-level messages */
#define	LOG_MAIL	(2<<3)	/* mail system */
#define	LOG_DAEMON	(3<<3)	/* system daemons */
#define	LOG_AUTH	(4<<3)	/* security/authorization messages */
#define	LOG_SYSLOG	(5<<3)	/* messages generated internally by syslogd */
#define	LOG_LPR		(6<<3)	/* line printer subsystem */
#define	LOG_NEWS	(7<<3)	/* network news subsystem */
#define	LOG_UUCP	(8<<3)	/* UUCP subsystem */
#define	LOG_CRON	(9<<3)	/* clock daemon */
#define	LOG_AUTHPRIV	(10<<3)	/* security/authorization messages (private) */
#define	LOG_FTP		(11<<3)	/* ftp daemon */

/* other codes through 15 reserved for system use */
#define	LOG_LOCAL0	(16<<3)	/* reserved for local use */
#define	LOG_LOCAL1	(17<<3)	/* reserved for local use */
#define	LOG_LOCAL2	(18<<3)	/* reserved for local use */
#define	LOG_LOCAL3	(19<<3)	/* reserved for local use */
#define	LOG_LOCAL4	(20<<3)	/* reserved for local use */
#define	LOG_LOCAL5	(21<<3)	/* reserved for local use */
#define	LOG_LOCAL6	(22<<3)	/* reserved for local use */
#define	LOG_LOCAL7	(23<<3)	/* reserved for local use */

/**
 * The syslog program types.
 */
enum class LogFacility
{
    Auth   = LOG_AUTH,  /**< Auth */
    Cron   = LOG_CRON,  /**< Cron */
    Daemon = LOG_DAEMON,/**< Daemon */
    FTP    = LOG_FTP,   /**< FTP */
    Local0 = LOG_LOCAL0,/**< Local0 */
    Local1 = LOG_LOCAL1,/**< Local1 */
    Local2 = LOG_LOCAL2,/**< Local2 */
    Local3 = LOG_LOCAL3,/**< Local3 */
    Local4 = LOG_LOCAL4,/**< Local4 */
    Local5 = LOG_LOCAL5,/**< Local5 */
    Local6 = LOG_LOCAL6,/**< Local6 */
    Local7 = LOG_LOCAL7,/**< Local7 */
    Print  = LOG_LPR,   /**< Print */
    Mail   = LOG_MAIL,  /**< Mail */
    News   = LOG_NEWS,  /**< News */
    User   = LOG_USER,  /**< User */
    Uucp   = LOG_UUCP,  /**< Uucp */
};

/**
 * \class SysLogWriter
 * \brief A log writer that sends output to a syslog server.
 */
class SysLogWriter: public LogWriterInterface {
public:
    SysLogWriter(std::string aIdentifier, const std::string& arAcceptLevel, LogFacility aType = LogFacility::Local0);
    SysLogWriter(std::string aIdentifier, LogLevel aAcceptLevel, LogFacility aType = LogFacility::Local0);
    ~SysLogWriter() override;

    void Write(const std::string &arMsg, LogLevel aCurrentLevel, const std::string &arChannel, const rsp::utils::DynamicData &arContext) override;

protected:
    std::string mIdent;
    LogLevel mAcceptLevel;
};

} /* namespace logging */

#endif // RSP_CORE_LIB_LOGGING_SYS_LOG_WRITER_H
