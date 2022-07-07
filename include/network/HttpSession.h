/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_NETWORK_HTTPSESSION_H_
#define INCLUDE_NETWORK_HTTPSESSION_H_

#include <memory>
#include <network/IHttpSession.h>

namespace rsp::network {

class HttpSession: public IHttpSession
{
public:
    HttpSession();
    void Execute() override;
    HttpRequestOptions& GetDefaultOptions() override;
    IHttpRequest& MakeRequest() override;
    IHttpRequest& MakeRequest(const rsp::network::HttpRequestOptions &arOptions) override;

protected:
    std::unique_ptr<IHttpSession> mPimpl;
    static IHttpSession* MakePimpl();
};

} /* namespace rsp::network */

#endif /* INCLUDE_NETWORK_HTTPSESSION_H_ */
