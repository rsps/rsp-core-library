/*!
 * \copyright    Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license      Mozilla Public License 2.0
 * \author:      Jesper Madsen
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDE_NETWORK_IHTTPSESSION_H
#define INCLUDE_NETWORK_IHTTPSESSION_H

#include <network/IHttpRequest.h>
#include <network/HttpRequestOptions.h>

namespace rsp::network {

class IHttpSession
{
public:
    virtual ~IHttpSession() {}

    virtual HttpRequestOptions& GetDefaultOptions() = 0;
    virtual IHttpRequest& MakeRequest() = 0;
    virtual IHttpRequest& MakeRequest(const HttpRequestOptions &arOptions) = 0;

    virtual void Execute() = 0;
};

} // namespace rsp::network

#endif
