/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_UTILS_CONST_TYPE_INFO_H
#define RSP_CORE_LIB_UTILS_CONST_TYPE_INFO_H

#include <cstdint>
#include <string_view>
#include "Crc32.h"
#include "Fnv1a.h"
#include <array>
#include <utility>

namespace rsp::utils {


template <std::size_t...Index>
constexpr auto substring_as_array(std::string_view str, std::index_sequence<Index...>)
{
    return std::array{str[Index]...};
}

template <typename T>
constexpr auto type_name_array()
{
#if defined(__clang__)
    constexpr auto prefix   = std::string_view{"[T = "};
    constexpr auto suffix   = std::string_view{"]"};
    constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(__GNUC__)
    constexpr auto prefix   = std::string_view{"with T = "};
    constexpr auto suffix   = std::string_view{"]"};
    constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(_MSC_VER)
    constexpr auto prefix   = std::string_view{"type_name_array<"};
    constexpr auto suffix   = std::string_view{">(void)"};
    constexpr auto function = std::string_view{__FUNCSIG__};
#else
# error Unsupported compiler
#endif

    constexpr auto start = function.find(prefix) + prefix.size();
    constexpr auto end = function.rfind(suffix);

    static_assert(start < end);

    constexpr auto name = function.substr(start, (end - start));
    return substring_as_array(name, std::make_index_sequence<name.size()>{});
}

template <typename T>
struct type_name_holder {
    static inline constexpr auto value = type_name_array<T>();
};

/**
 * \brief Template type to string helper.
 *
 * \see https://rodusek.com/posts/2021/03/09/getting-an-unmangled-type-name-at-compile-time/
 *
 * @tparam T Type to get the name of
 * @return string_view pointing to name
 */
template <typename T>
constexpr auto NameOf() -> std::string_view
{
    constexpr auto& value = type_name_holder<T>::value;
    return std::string_view{value.data(), value.size()};
}


namespace fnv1a {

template<typename T>
constexpr uint32_t HashOf() noexcept
{
    return fnv1a::Hash32Const(NameOf<T>());
}

}

namespace crc32 {

template<typename T>
constexpr uint32_t HashOf() noexcept
{
    return crc32::HashConst(NameOf<T>());
}

constexpr uint32_t HashOf(const char *apName) noexcept
{
    return crc32::HashConst(std::string_view(apName));
}

} /* namespace crc32 */


template <class T>
constexpr uint32_t ID()
{
    return crc32::HashOf<T>();
}


/**
 * \class TypeInfo
 *
 * \brief Represents metadata about a specific runtime object in a system.
 *
 * This class adds simple runtime information to class types, such as their name and ID.
 * The attributes can be changed, with a simple helper for constant compile time initialization.
 */
class TypeInfo
{
public:
    virtual ~TypeInfo() = default;

    /**
     * Get the name of this object.
     * \return string with the name
     */
    [[nodiscard]] const std::string& GetName() const
    {
        return mName;
    }

    /**
     * Set the name of this object
     * \param arName string with name
     */
    void SetName(const std::string &arName) { setName(arName); }
    void SetName(const char *apName) { setName(std::string(apName)); }

    /**
     * Get the ID of this object
     * \return unsigned numeric ID
     */
    [[nodiscard]] uint32_t GetId() const { return mId; }

    /**
     * Set the ID of this object
     * \param aId numeric ID
     */
    void SetId(const uint32_t aId) { setId(aId); }
    void SetId(const int aId) { setId(static_cast<uint32_t>(aId)); }
    void SetId(const char aId) { setId(static_cast<uint32_t>(aId)); }

protected:
    /**
     * Helper method for static initialization of descendants
     * \tparam C Descendant class type, used for constant naming
     */
    template <class C>
    void initTypeInfo()
    {
        mName = std::string(NameOf<C>());
        mId = ID<C>();
    }

    virtual void setId(const uint32_t aId)
    {
        mId = aId;
    }

    virtual void setName(const std::string &arName)
    {
        mName = arName;
    }

private:
    std::string mName{};
    uint32_t mId{0};
};


} /* namespace rsp::utils */

#endif // RSP_CORE_LIB_UTILS_CONST_TYPE_INFO_H
