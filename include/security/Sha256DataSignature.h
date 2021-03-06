/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_SECURITY_SHA256DATASIGNATURE_H_
#define INCLUDE_SECURITY_SHA256DATASIGNATURE_H_

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

    std::size_t GetSize() const override { return 32; }
    std::uint8_t* GetData() override { return mSignature.data(); }
    const std::uint8_t* GetData() const override { return mSignature.data(); }
    void Calc(const rsp::utils::IDataContent &arContent) override;
    void Verify(const rsp::utils::IDataContent &arContent) override;

protected:
    SecureBuffer mSignature{};
    SecureBuffer mSecret{};
};

} /* namespace rsp::security */

#endif /* INCLUDE_SECURITY_SHA256DATASIGNATURE_H_ */
