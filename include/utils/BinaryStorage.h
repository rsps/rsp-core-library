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
#include <type_traits>

namespace rsp::utils {

struct BinaryStorage
{
    explicit BinaryStorage(std::istream& i) : mpIn(&i) {}
    explicit BinaryStorage(std::ostream& o) : mpOut(&o) {}
    explicit BinaryStorage(std::iostream& io) : mpIn(&dynamic_cast<std::istream&>(io)), mpOut(&dynamic_cast<std::ostream&>(io)) {}

    std::istream *mpIn = nullptr;
    std::ostream *mpOut = nullptr;
};

/**
 * \brief Streaming operators for store/retrieve binary data to/from streams
 */
template< class T> requires(std::is_arithmetic_v<T> || std::is_enum_v<T>)
BinaryStorage& operator>>(BinaryStorage &i, T& arValue) {
    i.mpIn->read(reinterpret_cast<char*>(&arValue), sizeof(T));
    return i;
}

// For string like types
template <template <typename> class ContainerT,
        typename ValueT>
BinaryStorage& operator>>(BinaryStorage &i, ContainerT<ValueT>& arContainer)
{
    uint32_t sz;
    i.mpIn->read(reinterpret_cast<char *>(&sz), sizeof(sz));
    arContainer.resize(size_t(sz));
    i.mpIn->read(reinterpret_cast<char *>(arContainer.data()), sz * sizeof(ValueT));
    return i;
}

// For vector like types
template <template <typename, typename> class ContainerT,
        typename ValueT,
        typename AllocatorT>
BinaryStorage& operator>>(BinaryStorage &i, ContainerT<ValueT, AllocatorT>& arContainer)
{
    uint32_t sz;
    i.mpIn->read(reinterpret_cast<char *>(&sz), sizeof(sz));
    arContainer.resize(size_t(sz));
    i.mpIn->read(reinterpret_cast<char *>(arContainer.data()), sz * sizeof(ValueT));
    return i;
}

// For basic_string like types
template <template <typename, typename, typename> class ContainerT,
        typename ValueT,
        typename TraitsT,
        typename AllocatorT>
BinaryStorage& operator>>(BinaryStorage &i, ContainerT<ValueT, TraitsT, AllocatorT>& arContainer)
{
    uint32_t sz;
    i.mpIn->read(reinterpret_cast<char *>(&sz), sizeof(sz));
    arContainer.resize(size_t(sz));
    i.mpIn->read(reinterpret_cast<char *>(arContainer.data()), sz * sizeof(ValueT));
    return i;
}



template<class T> requires(std::is_arithmetic_v<T> || std::is_enum_v<T>)
BinaryStorage& operator<<(BinaryStorage &o, const T& arValue)
{
    o.mpOut->write(reinterpret_cast<const char *>(&arValue), sizeof(T));
    return o;
}

// For vector like types
template <template <typename, typename> class ContainerT,
          typename ValueT,
          typename AllocatorT>
BinaryStorage& operator<<(BinaryStorage &o, const ContainerT<ValueT, AllocatorT>& arContainer)
{
    auto sz = uint32_t(arContainer.size());
    o.mpOut->write(reinterpret_cast<const char *>(&sz), sizeof(sz));
    o.mpOut->write(reinterpret_cast<const char *>(arContainer.data()), sz * sizeof(ValueT));
    return o;
}

// For string like types
template <template <typename, typename, typename> class ContainerT,
        typename ValueT,
        typename TraitsT,
        typename AllocatorT>
BinaryStorage& operator<<(BinaryStorage &o, const ContainerT<ValueT, TraitsT, AllocatorT>& arContainer)
{
    auto sz = uint32_t(arContainer.size());
    o.mpOut->write(reinterpret_cast<const char *>(&sz), sizeof(sz));
    o.mpOut->write(reinterpret_cast<const char *>(arContainer.data()), sz * sizeof(ValueT));
    return o;
}

} // rsp::utils

#endif //RSP_CORE_LIB_INCLUDE_UTILS_STREAM_STORAGE_H
