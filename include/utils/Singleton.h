/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */


#ifndef INCLUDE_UTILS_SINGLETON_H_
#define INCLUDE_UTILS_SINGLETON_H_

#ifdef MT
#include <mutex>
#endif
#include <exceptions/CoreException.h>
#include "ConstTypeInfo.h"

namespace rsp::utils {


/**
 * \class Singleton
 * \brief Testable singleton pattern implemented as template class.
 *
 * Usage:
 *   class MyClass : public Singleton<MyClass> {}
 *
 * \tparam T
 */
template<class T>
class Singleton
{
public:
    /**
     * \brief Construct a Singleton
     */
    Singleton() {}

    /**
     * \brief Prohibit copy of Singleton
     */
    Singleton(Singleton<T> &other) = delete;

    virtual ~Singleton() {}

    /**
     * \brief Prohibit singleton assignment.
     */
    void operator=(const Singleton &) = delete;

    /**
     * \brief Check if this singleton is instantiated
     * \return bool
     */
    static bool HasInstance() {
        return (mpInstance) ? true : false;
    }

    /**
     * \brief Check is this singleton instance is self owned
     * \return bool
     */
    static bool OwnsInstance() {
        return mOwnsInstance;
    }

    /**
     * \fn void Create(...)
     * \brief Generic factory method.
     */
    template<typename... Args>
    static void CreateInstance(Args &&... args) {
#ifdef MT
        std::lock_guard<std::mutex> lock(mMutex);
#endif
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
    static void SetInstance(T* apObject) {
#ifdef MT
        std::lock_guard<std::mutex> lock(mMutex);
#endif
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
    static T& GetInstance() {
        if (!mpInstance) {
            THROW_WITH_BACKTRACE1(exceptions::ENoInstance, NameOf<T>());
        }
        return *mpInstance;
    }

    /**
     * \fn void Destroy()
     * \brief Call this to destroy a self owned instance. Useful during unit testing.
     */
    static void DestroyInstance() {
#ifdef MT
        std::lock_guard<std::mutex> lock(mMutex);
#endif
        if (mpInstance && mOwnsInstance) {
            delete mpInstance;
            mpInstance = nullptr;
            mOwnsInstance = false;
        }
    }

private:
#ifdef MT
    static std::mutex mMutex;
#endif
    static T* mpInstance;
    static bool mOwnsInstance;
};

template<class T>
T* Singleton<T>::mpInstance = nullptr;

template<class T>
bool Singleton<T>::mOwnsInstance = false;

} /* namespace rsp::utils */

#endif /* INCLUDE_UTILS_SINGLETON_H_ */
