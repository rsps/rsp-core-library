/*
 * \copyright    Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license      Mozilla Public License 2.0
 * \author:      Jesper Madsen
 * Created Date:  Tuesday, May 17th 2022, 8:49:44 am
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * HISTORY:
 * Date      	By	Comments
 * ----------	---	----------------------------------------------------------
 */

#ifndef I_HTTPSESSION_H
#define I_HTTPSESSION_H

#include <network/IHttpRequest.h>
#include <network/HttpRequestOptions.h>

#include <memory>

namespace rsp::network::http {  
class IHttpSession {
    public:
        virtual IHttpRequest& GetDefaultOptions();
        virtual IHttpRequest& MakeRequest();
        virtual IHttpRequest& MakeRequest(HttpRequestOptions options);
        virtual ~IHttpSession(){};
};
}

#endif