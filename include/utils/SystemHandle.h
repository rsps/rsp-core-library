/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2025 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_INCLUDE_UTILS_UNIQUE_HANDLE_H
#define RSP_CORE_LIB_INCLUDE_UTILS_UNIQUE_HANDLE_H

#include <concepts>
#include <functional>
#include <type_traits>

namespace rsp::utils {

template<typename T> concept IsTrivial = std::is_trivial_v<T>;

/**
 * \brief Class to encapsulate integral type system handles.
 * \tparam T System handle (like a Linux file handle)
 * \tparam D Invokable, called when this object is destroyed so handle can be released correctly.
 * \tparam invalid_handle Initializer value for empty/destroyed/invalidated handle
 */
template<IsTrivial T, typename D, T invalid_handle = T{}> requires std::invocable<D&, T>
class SystemHandle
{
public:
    constexpr SystemHandle() = default;

    constexpr ~SystemHandle()
    {
        Close();
    }

    constexpr explicit SystemHandle(T const aHandle) noexcept
        : mHandle{aHandle}
    {
    }

    constexpr SystemHandle(SystemHandle&& arOther) noexcept
        : mHandle{arOther.mHandle}
    {
        arOther.mHandle = invalid_handle;
    }

    constexpr SystemHandle& operator=(SystemHandle&& arOther) noexcept
    {
        mHandle = arOther.mHandle;
        arOther.mHandle = invalid_handle;
        return *this;
    }

    constexpr SystemHandle(SystemHandle const&) = delete;
    constexpr SystemHandle& operator=(SystemHandle const&) = delete;

    [[nodiscard]] constexpr explicit operator T() const noexcept
    {
        return mHandle;
    }

    [[nodiscard]] constexpr T Get() const noexcept
    {
        return mHandle;
    }

    [[nodiscard]] constexpr T& Get() noexcept
    {
        return mHandle;
    }

    [[nodiscard]] constexpr const T* operator->() const noexcept
    {
        return &mHandle;
    }

    [[nodiscard]] constexpr T* operator->() noexcept
    {
        return &mHandle;
    }

    [[nodiscard]] constexpr explicit operator bool() const noexcept
    {
        return mHandle != invalid_handle;
    }

    [[nodiscard]] constexpr bool operator!() const noexcept
    {
        return mHandle == invalid_handle;
    }

    [[nodiscard]] constexpr bool operator==(SystemHandle const&) const noexcept requires std::equality_comparable<T> = default;

    constexpr SystemHandle& operator=(T const handle)
    {
        Close();
        mHandle = handle;
        return *this;
    }

    constexpr void Close()
    {
        if (mHandle != invalid_handle) {
            std::invoke(D{}, mHandle);
            mHandle = invalid_handle;
        }
    }

private:
    T mHandle{invalid_handle};
};

} // rsp::utils

#endif //RSP_CORE_LIB_INCLUDE_UTILS_UNIQUE_HANDLE_H
