/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <exceptions/SignalHandler.h>
#include <magic_enum.hpp>
#include <logging/LogChannel.h>

namespace rsp::exceptions {

SignalHandler::SignalCallback_t SignalHandler::mHandlers[_NSIG]{};

SignalHandler::SignalHandler()
{
    Signals signals[] = {
        Signals::HangUp,
        Signals::InteractiveAttention,
        Signals::Quit,
        Signals::IllegalInstruction,
        Signals::BreakpointTrap,
        Signals::AbnormalTermination,
        Signals::FloatingPointErr,
        Signals::Killed,
        Signals::InvalidAccess,
        Signals::BrokenPipe,
        Signals::AlarmClock,
        Signals::Terminate
    };

    for (Signals s : signals) {
        signal(static_cast<int>(s), signalHandler);
    }
}

SignalHandler::~SignalHandler()
{
    Signals signals[] = {
        Signals::HangUp,
        Signals::InteractiveAttention,
        Signals::Quit,
        Signals::IllegalInstruction,
        Signals::BreakpointTrap,
        Signals::AbnormalTermination,
        Signals::FloatingPointErr,
        Signals::Killed,
        Signals::InvalidAccess,
        Signals::BrokenPipe,
        Signals::AlarmClock,
        Signals::Terminate
    };

    for (Signals s : signals) {
        signal(static_cast<int>(s), SIG_DFL);
    }
}

void SignalHandler::signalHandler(int aSignalCode) noexcept
{
    if (mHandlers[aSignalCode]) {
        mHandlers[aSignalCode]();
        return;
    }

    BackTrace bt(1);
    logging::LogChannel log("SignalHandler");
    log.Alert() << "Caught signal (" << aSignalCode << ") " << magic_enum::enum_name(static_cast<Signals>(aSignalCode)) << "\n" << bt;
    exit(200 + aSignalCode);
}


void SignalHandler::Register(Signals aSignal, SignalCallback_t aHandler)
{
    mHandlers[static_cast<int>(aSignal)] = std::move(aHandler);
}

void SignalHandler::Unregister(Signals aSignal)
{
    mHandlers[static_cast<int>(aSignal)] = nullptr;
}


} /* namespace rsp::exceptions */
