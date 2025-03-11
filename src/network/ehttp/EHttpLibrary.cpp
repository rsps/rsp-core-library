/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include "EHttpLibrary.h"
#ifdef USE_OPENSSL
    #include <openssl/ssl.h>
#endif

namespace rsp::network {

NetworkLibrary& NetworkLibrary::Get()
{
    return ehttp::EHttpLibrary::Get();
}

namespace ehttp {

EHttpLibrary& EHttpLibrary::Get()
{
    static EHttpLibrary instance;
    return instance;
}

EHttpLibrary::EHttpLibrary()
{
#ifdef USE_OPENSSL
    SSL_library_init();
#endif
}

} // namespace ehttp

} // rsp::network