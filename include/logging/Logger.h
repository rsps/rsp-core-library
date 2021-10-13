/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_LOGGING_LOGGER_H_
#define SRC_LOGGING_LOGGER_H_

#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <sstream>
#include <mutex>
#include <iostream>
#include "LogWriter.h"

namespace rsp::logging {

// Usage: make CXXFLAGS="-DDEBUG_LOG"

//#define DEBUG_LOG 1

#ifdef DEBUG_LOG
#include <iostream>

#define LOG(a) { std::cout << a << std::endl; }
#define DLOG(a) { std::cerr << a << std::endl; }

#else /* DEBUG_LOG */

#define LOG(a)
#define DLOG(a)

#endif /* DEBUG_LOG */


class LogStreamInterface;
class LogStream;

/**
 * \class Abstract LoggerInterface
 *
 * \brief This is the interface for logging functionality.
 *
 * The logging design is a public logger interface with multiple writers attached to it.
 * A writer can be limited in which log level should trigger its output.
 */
class LoggerInterface
{
public:
    virtual ~LoggerInterface() {}

    virtual LogStream Emergency() = 0;
    virtual LogStream Alert() = 0;
    virtual LogStream Critical() = 0;
    virtual LogStream Error() = 0;
    virtual LogStream Warning() = 0;
    virtual LogStream Notice() = 0;
    virtual LogStream Info() = 0;
    virtual LogStream Debug() = 0;

    bool HasWriters() const;

    typedef uintptr_t Handle_t;
    Handle_t AddLogWriter(std::shared_ptr<LogWriterInterface> aWriter);

    void RemoveLogWriter(Handle_t aHandle);

protected:
    std::mutex mMutex;
    std::vector<std::shared_ptr<LogWriterInterface>> mWriters;

    friend class LogStreamInterface;
    virtual void write(const LogStreamInterface *apStream, const std::string &arMsg);
};

/**
 * \class LogStreamInterface
 *
 * \brief This is the base class for log streams.
 *
 * LogStreams are used as temporary objects, holding logged information together until
 * flushed or destroyed.
 * It ensures thread safe write of the log data.
 */
class LogStreamInterface
{
public:
    LogStreamInterface(LoggerInterface *apOwner, LogLevel aLevel);
    LogStreamInterface(const LogStreamInterface & aFrom)
        : mpOwner(aFrom.mpOwner), mLevel(aFrom.mLevel) {}

    LogLevel GetLevel() const { return mLevel; }
    void SetLevel(LogLevel aLevel) { mLevel = aLevel; }

protected:
    LoggerInterface *mpOwner;
    LogLevel mLevel;

    void ownerWrite(const std::string &arMsg);
};

/**
 * \class LogStream
 *
 * \brief A templated LogStream implementation
 */
class LogStream : public LogStreamInterface
{
public:
    LogStream(LoggerInterface *apOwner, LogLevel aLevel);
    LogStream(const LogStream &aFrom) = delete; /* No copy, move is OK */
    LogStream(LogStream &&aFrom);
    ~LogStream();

    template< class type> LogStream& operator<<( type aValue) {
        mBuffer << aValue;
        return *this;
    }

    LogStream& operator<<( std::ostream&(*apFunc)(std::ostream&) )
    {
        mBuffer << apFunc;
        return *this;
    }

    void Flush();

protected:
    std::stringstream mBuffer;
};

/**
 * \class OutStreamBuf
 *
 * \brief An ostreambuf implementation allowing for std::ostream operations.
 */
class OutStreamBuf : public std::streambuf, public LogStreamInterface
{
public:
    OutStreamBuf(LoggerInterface *apOwner, LogLevel aLevel);

    void Lock() { mMutex.lock(); }

protected:
    std::string mBuffer;
    std::mutex mMutex;

    int overflow(int c) override;
    int sync() override;
};

std::ostream& operator<< (std::ostream& os, LogLevel aLevel);


/**
 * \class Logger
 *
 * \brief The default application logging instance type.
 */
class Logger : public LoggerInterface
{
public:
    Logger(bool aCaptureClog = false);
    Logger(const Logger&) = delete;
    ~Logger() override;

    LogStream Emergency() override;
    LogStream Alert() override;
    LogStream Critical() override;
    LogStream Error() override;
    LogStream Warning() override;
    LogStream Notice() override;
    LogStream Info() override;
    LogStream Debug() override;

protected:
    std::streambuf *mpClogBackup;
};

} /* namespace logging */

#endif /* SRC_LOGGING_LOGGER_H_ */
