/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2025 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef RSP_CORE_LIB_MULTIPART_BODY_H
#define RSP_CORE_LIB_MULTIPART_BODY_H

#include "IStreamDataProvider.h"
#include "HttpRequestOptions.h"
#include "MultipartBoundary.h"
#include <posix/FileIO.h>
#include <vector>

namespace rsp::network {

/**
 * \brief Aggregate class that holds a collection of body parts to be formatted as multipart/form-data.
 *
 * Every element has to be of type HttpBody_t, and are added using the overloaded Add methods.
 */
class MultipartBody : public IStreamDataProvider
{
public:
    /**
     * \brief Simply do nothing here. It could throw an exception in the future.
     * \see IStreamDataProvider::Write
     * \param aData
     * \return 0
     */
    [[nodiscard]] size_t Write(std::span<const std::byte> aData) override;
    [[nodiscard]] size_t Read(std::span<std::byte> aBuffer) const override;
    [[nodiscard]] size_t GetStreamSize() const override;
    MultipartBody& Rewind() override;

    /**
     * \brief Add a file body part
     * \param arName
     * \param arFile
     * \param arContentType
     * \return self
     */
    MultipartBody& Add(const std::string &arName, posix::FileIO &arFile, const std::string &arContentType = std::string());

    /**
     * \brief Add any exiting body part to this collection
     * \param arName
     * \param apBody
     * \param arContentType
     * \return self
     */
    MultipartBody& Add(const std::string &arName, HttpBody_t apBody = nullptr, const std::string &arContentType = std::string());

    /**
     * \brief Add a StringBody part
     * \param arName
     * \param arValue
     * \param arContentType
     * \return self
     */
    MultipartBody& Add(const std::string &arName, const std::string &arValue, const std::string &arContentType = std::string());

    /**
     * \brief Get the Boundary object used internally.
     * \return cref MultipartBoundary
     */
    [[nodiscard]] const MultipartBoundary& GetBoundary() const { return mBoundary; }

protected:
    struct MultipartPart_t
    {
        std::string mHeaders{};
        HttpBody_t mpBody{};

        mutable size_t mReadIndex = 0;
    };

    std::vector<MultipartPart_t> mParts{};
    MultipartBoundary mBoundary{};
    mutable size_t mReadPartIndex = 0;
};

} // rsp::network

#endif //RSP_CORE_LIB_MULTIPART_BODY_H
