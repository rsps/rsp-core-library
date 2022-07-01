/*!
 * \copyright    Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license      Mozilla Public License 2.0
 * \author:      Jesper Madsen
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef I_HTTPSESSION_H
#define I_HTTPSESSION_H

#include <network/IHttpRequest.h>
#include <network/HttpRequestOptions.h>

#include <memory>

namespace rsp::network {

class IHttpSession
{
public:
    virtual IHttpRequest& GetDefaultOptions();
    virtual IHttpRequest& MakeRequest();
    virtual IHttpRequest& MakeRequest(HttpRequestOptions options);
    virtual ~IHttpSession()
    {
    }
    ;
};

} // rsp::network

#endif
