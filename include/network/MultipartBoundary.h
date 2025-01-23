/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_SRC_NETWORK_MULTIPART_BOUNDARY_H
#define RSP_CORE_LIB_SRC_NETWORK_MULTIPART_BOUNDARY_H

#include <ostream>
#include <string>
#include <string_view>

namespace rsp::network {

constexpr const char* CRLF = "\r\n";

/**
 * \brief Helper class to generate multipart boundaries for http multipart/form-data protocol formatting
 * \see https://datatracker.ietf.org/doc/html/rfc7578
 */
class MultipartBoundary
{
public:
    /**
     * \brief Construct with a random boundary
     */
    MultipartBoundary();

    /**
     * \brief Construct with given boundary
     * \param aBoundary
     */
    explicit MultipartBoundary(std::string_view aBoundary);

    /**
     * \brief Get the http content type header value
     * \return string
     */
    [[nodiscard]] std::string GetContentTypeHeader() const;

    /**
     * \brief Make a string containing a multiline content-disposition block with the given attributes
     * \param arName Field name for content-disposition
     * \param arFileName Filename to add to content-disposition
     * \param arContentType Content-Type header to add to content-disposition
     * \return string with CRLF line delimiters
     */
    [[nodiscard]] std::string MakeContentDisposition(const std::string &arName, const std::string &arFileName = std::string(), const std::string &arContentType = std::string()) const;
    /**
     * \brief Stream contents for a multiline content-disposition block with the given attributes
     * \param o The output stream
     * \param arName Field name for content-disposition
     * \param arFileName Filename to add to content-disposition
     * \param arContentType Content-Type header to add to content-disposition
     * \return output stream
     */
    [[nodiscard]] std::ostream& StreamContentDisposition(std::ostream& o, const std::string &arName, const std::string &arFileName = std::string(), const std::string &arContentType = std::string()) const;

    /**
     * \brief Get a string formatted as the end boundary
     * \return string
     */
    [[nodiscard]] std::string GetEndBoundary() const;
    /**
     * \brief Stream an end boundary string
     * \return output stream
     */
    [[nodiscard]] std::ostream& StreamEndBoundary(std::ostream &) const;

    /**
     * \brief Get the contained boundary
     * \return const reference to contained string
     */
    [[nodiscard]] const std::string& GetBoundary() const;

protected:
    std::string mBoundary{};
};

} // rsp::network

#endif //RSP_CORE_LIB_SRC_NETWORK_MULTIPART_BOUNDARY_H
