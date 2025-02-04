/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_SRC_NETWORK_EHTTP_E_HTTP_SESSION_H
#define RSP_CORE_LIB_SRC_NETWORK_EHTTP_E_HTTP_SESSION_H

#include <network/IHttpSession.h>
#include <utils/ObjectPool.h>
#include "EHttpRequest.h"

namespace rsp::network::ehttp {

class EHttpSession : public IHttpSession
{
public:
    explicit EHttpSession(size_t aRequestPoolSize);

    void ProcessRequests() override;
    IHttpSession& SetDefaultOptions(const HttpRequestOptions& arOptions) override;
    HttpRequestOptions& GetDefaultOptions() override;
    [[nodiscard]] const HttpRequestOptions& GetDefaultOptions() const override;
    IHttpRequest& Request(HttpRequestType aType, std::string_view aUri, ResponseCallback_t aCallback) override;

protected:
    rsp::utils::ObjectPool<EHttpRequest> mPool;
    std::vector<EHttpRequest*> mPending{};
    HttpRequestOptions mDefaultOptions{};
};

} // rsp::network::ehttp

#endif //RSP_CORE_LIB_SRC_NETWORK_EHTTP_E_HTTP_SESSION_H
