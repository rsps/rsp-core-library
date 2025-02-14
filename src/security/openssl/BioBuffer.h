/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_SRC_SECURITY_OPENSSL_BIOBUFFER_H
#define RSP_CORE_LIB_SRC_SECURITY_OPENSSL_BIOBUFFER_H

#include "exceptions.h"
#include <memory>
#include <string_view>

namespace rsp::security {

class BioBuffer
{
public:
    explicit BioBuffer(std::string_view aPem)
            : mpBio(BioPtr(BIO_new_mem_buf(aPem.data(), int(aPem.size()))))
    {
        if (!mpBio) {
            THROW_WITH_BACKTRACE1(EOpenSSL, "BIO Allocation Error");
        }
    }

    BIO* Get() { return mpBio.get(); }

protected:
    struct BioDeleter {
        void operator()(BIO* x) { ::BIO_free(x); }
    };
    using BioPtr = std::unique_ptr<BIO, BioDeleter>;

    BioPtr mpBio;
};

} // namespace rsp::security

#endif //RSP_CORE_LIB_SRC_SECURITY_OPENSSL_BIOBUFFER_H
