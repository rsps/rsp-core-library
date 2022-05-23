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

#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <network/IHttpRequest.h>
#include <network/HttpResponse.h>

namespace rsp::network::http
{
    
    class HttpRequest : public IHttpRequest
    {
    public:
        HttpRequest();
        HttpRequestOptions &GetOptions() override
        {
            return mPimpl->GetOptions();
        }
        IHttpRequest &SetOptions(const HttpRequestOptions opt) override
        {
            mPimpl->SetOptions(opt);
            return *this;
        }
        IHttpRequest &SetHeaders(std::map<std::string, std::string> &headers) override
        {
            mPimpl->SetHeaders(headers);
            return *this;
        }
        IHttpRequest &SetBody(std::string const &body) override
        {
            mPimpl->SetBody(body);
            return *this;
        }
        IHttpResponse &Execute() override
        {
            return mPimpl->Execute();
        };
        void Execute(std::function<void(IHttpResponse &)> callback) override
        {
            mPimpl->Execute(callback);
        };
        
    protected:
        std::unique_ptr<IHttpRequest> mPimpl;
        static IHttpRequest *MakeRequest();
    };
}

#endif
