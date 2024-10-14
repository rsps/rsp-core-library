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

#include <fstream>
#include <sstream>
#include <type_traits>
#include <bit>
#include <cstdint>

namespace rsp::utils {

struct BinaryStream
{
    explicit BinaryStream(std::streambuf *apStreamBuf) : mpStreamBuf(apStreamBuf) {}
    explicit BinaryStream(std::iostream &arStream) : mpStreamBuf(arStream.rdbuf()) {}
    explicit BinaryStream(std::ostream &arStream) : mpStreamBuf(arStream.rdbuf()) {}
    explicit BinaryStream(std::istream &arStream) : mpStreamBuf(arStream.rdbuf()) {}

    void Reset() const
    {
        mpStreamBuf->pubseekpos(0);
    }

    std::streambuf *mpStreamBuf = nullptr;

    template <class T>
    void WriteSize(T sz)
    {
        uint8_t bytes[10];
        std::streamsize index = 0;
        if (sz) {
            while (sz) {
                bytes[index++] = (sz & 0x7F) | (sz > 0x7F ? 0x80 : 0);
                sz >>= 7;
            }
        }
        else {
            bytes[0] = 0;
            index = 1;
        }
        mpStreamBuf->sputn(reinterpret_cast<const char *>(bytes), index);
    }

    template <class T>
    T ReadSize()
    {
        T sz = 0;
        uint8_t byte;
        size_t index = 0;
        size_t bits = 0;
        do {
            mpStreamBuf->sgetn(reinterpret_cast<char *>(&byte), 1);
            sz += static_cast<T>((byte & 0x7F) << bits);
            bits += 7;
            index++;
        }
        while((byte & 0x80) && (index < sizeof(sz)));
        return sz;
    }

protected:
    BinaryStream() = default;
};

class BinaryStreamable
{
public:
    virtual ~BinaryStreamable() = default;
    virtual BinaryStream& SaveTo(BinaryStream& o) const = 0;
    virtual BinaryStream& LoadFrom(BinaryStream& i) = 0;
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

template<typename T>
concept IsBinaryStreamable = std::is_base_of_v<BinaryStreamable, T>;

template<typename T>
concept HasMemberFunction = std::is_member_function_pointer_v<decltype(&T::LoadFrom)>;


/**
 * \brief Streaming operators for store/retrieve binary data to/from streams
 */
template< class T> requires (!HasResize<T>) // && !std::is_integral_v<T>) // For values without resize() function
BinaryStream& operator>>(BinaryStream &i, T& arValue)
{
    if constexpr (IsBinaryStreamable<T>) {
        arValue.LoadFrom(i);
    }
    else {
        i.mpStreamBuf->sgetn(reinterpret_cast<char *>(&arValue), sizeof(T));
    }
    return i;
}

template<>
inline BinaryStream& operator>><size_t>(BinaryStream &i, size_t& arValue)
{
    arValue = i.ReadSize<size_t>();
    return i;
}

template <template <typename> class ContainerT,
        typename ValueT> requires (HasResize<ContainerT<ValueT> >) // For containers with resize() function
BinaryStream& operator>>(BinaryStream &i, ContainerT<ValueT>& arContainer)
{
    using size_type = decltype(arContainer.size());
    auto sz = i.ReadSize<size_type>();
    arContainer.resize(size_t(sz));
    i.mpStreamBuf->sgetn(reinterpret_cast<char *>(arContainer.data()), std::streamsize(sz * sizeof(ValueT)));
    return i;
}

template<class T> requires (!HasSize<T>) // && !std::is_integral_v<T>) // For values without size() function
BinaryStream& operator<<(BinaryStream &o, const T& arValue)
{
    if constexpr (IsBinaryStreamable<T>) {
        arValue.SaveTo(o);
    }
    else {
        o.mpStreamBuf->sputn(reinterpret_cast<const char *>(&arValue), sizeof(T));
    }
    return o;
}

template<>
inline BinaryStream& operator<<<size_t>(BinaryStream &o, const size_t& arValue)
{
    o.WriteSize(arValue);
    return o;
}

template <template <typename> class ContainerT,
      typename ValueT> requires (HasSize<ContainerT<ValueT> >) // For containers with size() function
BinaryStream& operator<<(BinaryStream &o, const ContainerT<ValueT>& arContainer)
{
    o.WriteSize(arContainer.size());
    o.mpStreamBuf->sputn(reinterpret_cast<const char *>(arContainer.data()), std::streamsize(arContainer.size() * sizeof(ValueT)));
    return o;
}

class BinaryFileStream : public BinaryStream
{
public:
    explicit BinaryFileStream(const std::string &arFilename, std::ios::openmode aMode)
    {
        mFile.exceptions(std::fstream::failbit | std::fstream::badbit | std::fstream::eofbit);
        mFile.open(arFilename, std::ios::binary | aMode);
        mpStreamBuf = mFile.rdbuf();
    }

    std::fstream& Get()
    {
        return mFile;
    }

protected:
    std::fstream mFile{};
};

class BinaryStringStream : public BinaryStream
{
public:
    BinaryStringStream()
    {
        mpStreamBuf = mStringStream.rdbuf();
    }

    std::stringstream& Get()
    {
        return mStringStream;
    }

protected:
    std::stringstream mStringStream{};
};

} // rsp::utils

#endif //RSP_CORE_LIB_INCLUDE_UTILS_STREAM_STORAGE_H
