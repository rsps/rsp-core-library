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

#include <utils/CoreException.h>

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
     * \fn void Create()
     * \brief Generic factory method.
     */
    static void Create() {
        if (mpInstance) {
            THROW_WITH_BACKTRACE(ESingletonViolation);
        }
        mpInstance = new T();
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
    static void Set(T* apObject) {
        if (mpInstance && mOwnsInstance) {
            THROW_WITH_BACKTRACE(ESingletonViolation);
        }
        mpInstance = apObject;
    }

    /**
     * \fn T Get&()
     * \brief Get a reference to a valid instance.
     *
     * \return
     */
    static T& Get() {
        if (!mpInstance) {
            THROW_WITH_BACKTRACE(ENoInstance);
        }
        return *mpInstance;
    }

    /**
     * \fn void Destroy()
     * \brief Call this to destroy a self owned instance. Useful during unit testing.
     */
    static void Destroy() {
        if (mpInstance && mOwnsInstance) {
            delete mpInstance;
            mpInstance = nullptr;
            mOwnsInstance = false;
        }
    }

private:
    static T* mpInstance;
    static bool mOwnsInstance;
};

template<class T>
T* Singleton<T>::mpInstance = nullptr;

template<class T>
bool Singleton<T>::mOwnsInstance = false;

} /* namespace rsp::utils */

#endif /* INCLUDE_UTILS_SINGLETON_H_ */
