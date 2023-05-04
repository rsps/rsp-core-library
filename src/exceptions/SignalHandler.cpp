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

namespace rsp::exceptions {

SignalHandler::SignalCallback_t SignalHandler::mHandlers[_NSIG]{};


void SignalHandler::signalHandler(int aSignalCode) noexcept
{
    if (mHandlers[aSignalCode]) {
        BackTrace bt(0);
        mHandlers[aSignalCode](bt);
    }
}


void SignalHandler::Register(Signals aSignal, SignalCallback_t aHandler)
{
    int s = static_cast<int>(aSignal);
    mHandlers[s] = aHandler;
    signal(s, signalHandler);
}

void SignalHandler::Unregister(Signals aSignal)
{
    int s = static_cast<int>(aSignal);
    mHandlers[s].Clear();
    signal(s, SIG_DFL);
}


} /* namespace rsp::exceptions */
