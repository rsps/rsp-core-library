/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_NETWORK_HTTP_SESSION_H
#define RSP_CORE_LIB_NETWORK_HTTP_SESSION_H

#include <network/IHttpSession.h>

namespace rsp::network {

class HttpSession: public IHttpSession
{
public:
    explicit HttpSession(size_t aSize);
    void ProcessRequests() override;

    IHttpSession& SetDefaultOptions(const HttpRequestOptions &arOptions) override;
    HttpRequestOptions& GetDefaultOptions() override;
    [[nodiscard]] const HttpRequestOptions& GetDefaultOptions() const override;

    IHttpRequest& Request(HttpRequestType aType, std::string_view aUri, ResponseCallback_t aCallback) override;

protected:
    std::unique_ptr<IHttpSession> mPimpl;
    static IHttpSession* MakePimpl(size_t aSize);
};

} /* namespace rsp::network */

#endif // RSP_CORE_LIB_NETWORK_HTTP_SESSION_H
