/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <network/HttpSession.h>
#include "CurlSession.h"

namespace rsp::network {

IHttpSession* HttpSession::MakePimpl()
{
    return new curl::CurlSession();
}

} /* namespace rsp::network */

namespace rsp::network::curl {

void CurlSession::Execute()
{
    mMulti.Execute();
}

rsp::network::IHttpSession& CurlSession::operator <<=(rsp::network::IHttpRequest &arRequest)
{
    if (!arRequest.IsAsync()) {
        THROW_WITH_BACKTRACE1(EAsyncRequest, "Requests handled by session, must have an async callback.");
    }

    mMulti.Add(arRequest);
    return *this;
}

} /* namespace rsp::network::curl */
