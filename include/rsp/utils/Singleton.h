/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_UTILS_SINGLETON_H
#define RSP_CORE_LIB_UTILS_SINGLETON_H

#include <rsp/exceptions/CoreException.h>
#include "ConstTypeInfo.h"

namespace rsp::utils {

/**
 * \class Singleton
 * \brief Testable singleton pattern implemented as template class.
 *
 * \warning This class is not thread safe during creation and destruction of the instance.
 *
 * Usage:
 *   class MyClass : public Singleton<MyClass> {}
 *
 * \tparam T
 */
template <class T>
class Singleton
{
public:
    /**
     * \brief Construct an empty and non-owning Singleton
     */
    Singleton() = default;

    virtual ~Singleton() = default;

    // Prohibit copy/move operations
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    /**
     * \brief Check if this singleton is instantiated
     * \return bool
     */
    static bool HasInstance()
    {
        return (mpInstance != nullptr);
    }

    /**
     * \brief Check is this singleton instance is self owned
     * \return bool
     */
    static bool OwnsInstance()
    {
        return mOwnsInstance;
    }

    /**
     * \fn void Create(...)
     * \brief Generic factory method.
     */
    template <typename... Args>
    static void CreateInstance(Args&&... args)
    {
        if (mpInstance) {
            THROW_WITH_BACKTRACE1(exceptions::ESingletonViolation, NameOf<T>());
        }
        mpInstance = new T(std::forward<Args>(args)...);
        mOwnsInstance = true;
    }

    /**
     * \fn void Set(T*)
     * \brief Set the instance if created outside this class. E.g. local or member variable.
     * \throws ESingletonViolation if instance already exists and is self owned.
     *
     * If assigned from outside, it is allowed to unassign by setting to nullptr.
     *
     * \param aObject
     */
    static void SetInstance(T* apObject)
    {
        if (mpInstance && mOwnsInstance) {
            THROW_WITH_BACKTRACE1(exceptions::ESingletonViolation, NameOf<T>());
        }
        mpInstance = apObject;
    }

    /**
     * \fn T Get&()
     * \brief Get a reference to a valid instance.
     *
     * \return
     */
    static T& GetInstance()
    {
        if (!mpInstance) {
            THROW_WITH_BACKTRACE1(exceptions::ENoInstance, NameOf<T>());
        }
        return *mpInstance;
    }

    /**
     * \fn void Destroy()
     * \brief Call this to destroy a self owned instance. Useful during unit testing.
     */
    static void DestroyInstance()
    {
        if (mpInstance && mOwnsInstance) {
            mOwnsInstance = false;
            delete mpInstance;
            mpInstance = nullptr;
        }
    }

private:
    static inline T* mpInstance = nullptr;
    static inline bool mOwnsInstance = false;
};

} /* namespace rsp::utils */

#endif // RSP_CORE_LIB_UTILS_SINGLETON_H
