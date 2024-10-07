/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_INCLUDE_UTILS_STREAM_STORAGE_H
#define RSP_CORE_LIB_INCLUDE_UTILS_STREAM_STORAGE_H

#include <iostream>
#include <string>

namespace rsp::utils {

/**
 * \brief Class to store and retrieve binary data to/from a stream
 */
class StreamStorage
{
public:
    explicit StreamStorage(std::istream &arIn) : mpIn(&arIn) {}
    explicit StreamStorage(std::ostream &arOut) : mpOut(&arOut) {}

    template< class T>
    StreamStorage& operator<<(const T& arValue) {
        mpOut->write(reinterpret_cast<const char*>(&arValue), sizeof(T));
        return *this;
    }

    StreamStorage& operator<<(const std::string& arValue) {
        auto sz = uint32_t(arValue.size());
        mpOut->write( reinterpret_cast<char*>(&sz), sizeof(sz));
        mpOut->write(arValue.data(), sz);
        return *this;
    }


    template< class T>
    StreamStorage& operator>>(T& arValue) {
        mpIn->read(reinterpret_cast<char*>(&arValue), sizeof(T));
        return *this;
    }

    StreamStorage& operator>>(std::string& arValue) {
        uint32_t sz;
        mpIn->read(reinterpret_cast<char*>(&sz), sizeof(sz));
        arValue.resize(size_t(sz));
        mpIn->read(arValue.data(), sz);
        return *this;
    }

protected:
    union {
        std::istream *mpIn = nullptr;
        std::ostream *mpOut;
    };
};

} // rsp::utils

#endif //RSP_CORE_LIB_INCLUDE_UTILS_STREAM_STORAGE_H
