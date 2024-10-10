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
#include <cassert>

namespace rsp::utils {

struct BinaryStorage
{
    explicit BinaryStorage(std::istream& i) : mpIn(&i) {}
    explicit BinaryStorage(std::ostream& o) : mpOut(&o) {}
    explicit BinaryStorage(std::iostream& io) : mpIn(&dynamic_cast<std::istream&>(io)), mpOut(&dynamic_cast<std::ostream&>(io)) {}

    std::istream *mpIn = nullptr;
    std::ostream *mpOut = nullptr;
};

template <typename T>
concept HasSize =
requires(T t) {
    { t.size() } -> std::same_as<size_t>;
};

template<typename T>
concept HasResize = requires {
    typename T::size_type;
    { std::declval<T>().resize(std::declval<typename T::size_type>()) } -> std::same_as<void>;
};


/**
 * \brief Streaming operators for store/retrieve binary data to/from streams
 */
template< class T> requires (!HasResize<T>) // For integrals without resize() function
BinaryStorage& operator>>(BinaryStorage &i, T& arValue) {
    if (i.mpIn) {
        i.mpIn->read(reinterpret_cast<char *>(&arValue), sizeof(T));
    }
    return i;
}

template <template <typename> class ContainerT,
        typename ValueT> requires (HasResize<ContainerT<ValueT> >) // For containers with resize() function
BinaryStorage& operator>>(BinaryStorage &i, ContainerT<ValueT>& arContainer)
{
    if (i.mpIn) {
        uint32_t sz;
        i.mpIn->read(reinterpret_cast<char *>(&sz), sizeof(sz));
        arContainer.resize(size_t(sz));
        i.mpIn->read(reinterpret_cast<char *>(arContainer.data()), sz * sizeof(ValueT));
    }
    return i;
}


template<class T> requires (!HasSize<T>) // For integrals without size() function
BinaryStorage& operator<<(BinaryStorage &o, const T& arValue)
{
    if (o.mpOut) {
        o.mpOut->write(reinterpret_cast<const char *>(&arValue), sizeof(T));
    }
    return o;
}

template <template <typename> class ContainerT,
      typename ValueT> requires (HasSize<ContainerT<ValueT> >) // For containers with size() function
BinaryStorage& operator<<(BinaryStorage &o, const ContainerT<ValueT>& arContainer)
{
    if (o.mpOut) {
        auto sz = uint32_t(arContainer.size());
        o.mpOut->write(reinterpret_cast<const char *>(&sz), sizeof(sz));
        o.mpOut->write(reinterpret_cast<const char *>(arContainer.data()), sz * sizeof(ValueT));
    }
    return o;
}

} // rsp::utils

#endif //RSP_CORE_LIB_INCLUDE_UTILS_STREAM_STORAGE_H
