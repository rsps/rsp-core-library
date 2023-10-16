/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_EXCEPTIONS_SIGNALHANDLER_H_
#define INCLUDE_EXCEPTIONS_SIGNALHANDLER_H_

#include <exceptions/BackTrace.h>
#include <functional>
#include <csignal>

namespace rsp::exceptions {

/**
 * \enum Signals
 * \brief POSIX/Linux signals
 *
 */
enum class Signals {
    HangUp = SIGHUP,              /**< HangUp */
    InteractiveAttention = SIGINT,/**< InteractiveAttention */
    Quit = SIGQUIT,               /**< Quit */
    IllegalInstruction = SIGILL,  /**< IllegalInstruction */
    BreakpointTrap = SIGTRAP,     /**< BreakpointTrap */
    AbnormalTermination = SIGABRT,/**< AbnormalTermination */
    FloatingPointErr = SIGFPE,    /**< FloatingPointErr */
    Killed = SIGKILL,             /**< Killed */
    InvalidAccess = SIGSEGV,      /**< InvalidAccess */
    BrokenPipe = SIGPIPE,         /**< BrokenPipe */
    AlarmClock = SIGALRM,         /**< AlarmClock */
    Terminate = SIGTERM           /**< Terminate */
};

/**
 * \class SignalHandler
 * \brief Signal handler class that wraps posix signals
 */
class SignalHandler
{
public:
    using SignalCallback_t = std::function<void(void)>;

    SignalHandler();
    ~SignalHandler();

    static void Register(Signals aSignal, SignalCallback_t aHandler);
    static void Unregister(Signals aSignal);

protected:
    static SignalCallback_t mHandlers[_NSIG];

    static void signalHandler(int aSignalCode) noexcept;
};

} /* namespace rsp::exceptions */

#endif /* INCLUDE_EXCEPTIONS_SIGNALHANDLER_H_ */
