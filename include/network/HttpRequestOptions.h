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

#ifndef HTTPREQUESTOPTIONS_H
#define HTTPREQUESTOPTIONS_H

#include <network/ConnectionOptions.h>
#include <network/HttpRequestType.h>
#include <memory>
#include <string>
#include <map>

namespace rsp::network::http {  
    class HttpRequestOptions : public ConnectionOptions {
        public:
            std::map<std::string, std::string> Headers = {};
            std::string Uri = "";
            std::string Body = "";
            HttpRequestType RequestType = HttpRequestType::NONE;
            std::string BasicAuthUsername = "";
            std::string BasicAuthPassword = "";
    };
}

#endif