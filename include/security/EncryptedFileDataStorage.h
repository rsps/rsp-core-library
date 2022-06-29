/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_SECURITY_ENCRYPTEDFILEDATASTORAGE_H_
#define INCLUDE_SECURITY_ENCRYPTEDFILEDATASTORAGE_H_

#include <utils/DataContainer.h>
#include "SecureBuffer.h"

namespace rsp::security {

/**
 * \brief Storage implementation for encrypted files
 */
class EncryptedFileDataStorage: public utils::FileDataStorage
{
public:
    /**
     * \brief Initialize the encrypt storage object.
     * \param aFileName Path to file
     * \param arInitializationVector AES initialization vector
     * \param arKey AES symmetric key
     */
    void Init(std::string_view aFileName, const SecureBuffer& arInitializationVector, const SecureBuffer& arKey);

    void Write(const rsp::utils::IDataContent &arContent) override;
    void Read(rsp::utils::IDataContent &arContent) override;

protected:
    SecureBuffer mIv{};
    SecureBuffer mKey{};
};

} /* namespace rsp::security */

#endif /* INCLUDE_SECURITY_ENCRYPTEDFILEDATASTORAGE_H_ */
