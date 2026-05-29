/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_SECURITY_SECURE_BUFFER_H
#define RSP_CORE_LIB_SECURITY_SECURE_BUFFER_H

#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include "SecureAllocator.h"

namespace rsp::security {

class SecureBuffer;
class SecureString;

/**
 * \brief Output streaming operator for SecureBuffer
 * \param os Destination ostream
 * \param arBuffer SecureBuffer to stream to destination
 * \return The destination ostream
 */
std::ostream& operator<<(std::ostream& os, const SecureBuffer &arBuffer);


/**
 * \brief A secure buffer implementation that clears its memory after use.
 */
class SecureBuffer : public std::vector<uint8_t, SecureAllocator<uint8_t>>
{
public:
    using std::vector<uint8_t, SecureAllocator<uint8_t>>::vector;

    SecureBuffer(const char* apData) // NOLINT, conversion constructor
    {
        auto b = reinterpret_cast<const uint8_t*>(apData);
        auto e = reinterpret_cast<const uint8_t*>(apData + std::strlen(apData));
        assign(b, e);
    }

    /**
     * \brief Constructs from pointer to char array
     * \param apData Pointer to char data
     * \param aSize Size of data to include
     */
    SecureBuffer(const char* apData, std::size_t aSize)
    {
        auto b = reinterpret_cast<const uint8_t*>(apData);
        auto e = reinterpret_cast<const uint8_t*>(apData + aSize);
        assign(b, e);
    }

    /**
     * \brief Constructs from pointer to unsigned char array
     * \param apData Pointer to unsigned char data
     * \param aSize Size of data to include
     */
    SecureBuffer(const uint8_t* apData, std::size_t aSize)
    {
        auto b = apData;
        auto e = (apData + aSize);
        assign(b, e);
    }

    /**
     * \brief Comparator overload, compares with std::string of ASCII hex
     * \param arOther std::string with ASCII hex to compare content with.
     * \return bool True if equal
     */
    bool operator==(const std::string& arOther) const
    {
        std::stringstream ss;
        ss << *this;
        return ss.str() == arOther;
    }

    /**
     * \brief Comparator overload, compares with C style string of ASCII hex
     * \param apOther Pointer to C style string of ASCII hex to compare content with.
     * \return bool True if equal
     */
    bool operator==(const char* apOther) const
    {
        std::stringstream ss;
        ss << *this;
        return ss.str() == apOther;
    }

    /**
     * \brief Format this content to ASCII hex.
     * \return String with ASCII hex.
     */
    [[nodiscard]] SecureString GetHex() const;
};

} // namespace rsp::security

#endif // RSP_CORE_LIB_SECURITY_SECURE_BUFFER_H
