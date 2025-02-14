/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_SRC_NETWORK_EHTTP_E_HTTP_LIBRARY_H
#define RSP_CORE_LIB_SRC_NETWORK_EHTTP_E_HTTP_LIBRARY_H

#include <network/NetworkLibrary.h>

namespace rsp::network::ehttp {

class EHttpLibrary : public rsp::network::NetworkLibrary
{
public:
    EHttpLibrary(const EHttpLibrary&) = delete;
    EHttpLibrary& operator=(const EHttpLibrary&) = delete;

    static EHttpLibrary& Get();

    [[nodiscard]] std::string_view GetLibraryName() const override { return "eHttp"; }
    [[nodiscard]] std::string_view GetVersion() const override { return "0.1.0"; }
    [[nodiscard]] std::string_view GetSslVersion() const override { return "mBedTLS"; }

private:
    EHttpLibrary();
};

} // rsp::network::ehttp

#endif //RSP_CORE_LIB_SRC_NETWORK_EHTTP_E_HTTP_LIBRARY_H
