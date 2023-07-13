/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_UTILS_FUNCTION_H_
#define INCLUDE_UTILS_FUNCTION_H_

#include <atomic>
#include <functional>
#include <list>

namespace rsp::utils {

template<typename Res, typename ... ArgTypes>
class Function;


template<typename Res, typename ... ArgTypes>
class Function<Res(ArgTypes...)> : private std::function<Res(ArgTypes...)>
{
private:
    struct WrapperBase
    {
        std::atomic_int refcount;

        virtual Res Execute(ArgTypes ... args) = 0;

        WrapperBase()
            : refcount(1)
        {
        }
        virtual ~WrapperBase()
        {
        }
    };

    template<class F>
    struct Wrapper: WrapperBase
    {
        F fn;
        virtual Res Execute(ArgTypes ... args)
        {
            return fn(args...);
        }

        Wrapper(F &&aFn)
            : fn(std::move(aFn))
        {
        }
    };

    WrapperBase *ptr = nullptr;

    static void _free(WrapperBase *aPtr)
    {
        if (aPtr && --aPtr->refcount == 0) {
            delete aPtr;
        }
    }

    void _copy(const Function &arOther)
    {
        ptr = arOther.ptr;
        if (ptr) {
            ptr->refcount++;
        }
    }

public:
    Function()
    {
    }

    template<class F> Function(F fn)
    {
        ptr = new Wrapper<F>(std::move(fn));
    }

    ~Function()
    {
        _free(ptr);
    }

    Function(const Function &arOther)
        : std::function<Res(ArgTypes...)>(arOther)
    {
        _copy(arOther);
    }

    Function& operator=(const Function &arOther)
    {
        if (&arOther != this) {
            auto b = ptr;
            _copy(arOther);
            _free(b);
        }
        return *this;
    }

    Function(Function &&arOther)
    {
        if (&arOther != this) {
            ptr = arOther.ptr;
            arOther.ptr = nullptr;
        }
    }

    Function& operator=(Function &&arOther)
    {
        if (&arOther != this) {
            _free(ptr);
            ptr = arOther.ptr;
            arOther.ptr = nullptr;
        }
        return *this;
    }

    Res operator()(ArgTypes ... args) const
    {
        return ptr ? ptr->Execute(args...) : Res();
    }

    operator bool() const
    {
        return (ptr != nullptr);
    }

    void Clear()
    {
        _free(ptr);
        ptr = nullptr;
    }
};

template<class Ptr, class Class, class Res, class ... ArgTypes>
Function<Res(ArgTypes...)> Method(Ptr object, Res (Class::*method)(ArgTypes...))
{
    return [=](ArgTypes ... args)
    {
        return (object->*method)(args...);
    };
}

} // namespace rsp::utils

#endif /* INCLUDE_UTILS_FUNCTION_H_ */
