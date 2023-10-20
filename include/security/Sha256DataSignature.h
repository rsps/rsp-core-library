/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_SECURITY_SHA256_DATA_SIGNATURE_H
#define RSP_CORE_LIB_SECURITY_SHA256_DATA_SIGNATURE_H

#include "SecureBuffer.h"
#include <utils/DataContainer.h>

namespace rsp::security {

/**
 * \brief Signature implementation using HMAC-SHA256
 */
class Sha256DataSignature: public utils::IDataSignature
{
public:
    /**
     * \brief Initialize the signature object with the secret used in HMAC calculation.
     * \param arSecret Buffer with secret to use in HMAC
     */
    void Init(const SecureBuffer& arSecret);

    [[nodiscard]] std::size_t GetSize() const override { return 32; }
    std::uint8_t* GetData() override { return mSignature.data(); }
    [[nodiscard]] const std::uint8_t* GetData() const override { return mSignature.data(); }
    void Calc(const rsp::utils::IDataContent &arContent) override;
    void Verify(const rsp::utils::IDataContent &arContent) override;

protected:
    SecureBuffer mSignature{};
    SecureBuffer mSecret{};
};

} /* namespace rsp::security */

#endif // RSP_CORE_LIB_SECURITY_SHA256_DATA_SIGNATURE_H
