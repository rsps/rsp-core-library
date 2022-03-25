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

namespace rsp::utils {

template<typename Res, typename... ArgTypes>
class Function<Res(ArgTypes...)> : std::function<Res(ArgTypes...)>
{
    struct WrapperBase {
        std::atomic_int refcount;

        virtual Res Execute(ArgTypes... args) = 0;

        WrapperBase() { refcount = 1; }
        virtual ~WrapperBase() {}
    };

    template <class F>
    struct Wrapper : WrapperBase {
        F fn;
        virtual Res Execute(ArgTypes... args) { return fn(args...); }

        Wrapper(F&& fn) : fn(pick(fn)) {}
    };

    WrapperBase *ptr = nullptr;

    static void Free(WrapperBase *ptr) {
        if(ptr && --ptr->refcount == 0)
            delete ptr;
    }

    void Copy(const Function& a) {
        ptr = a.ptr;
        if(ptr)
            ptr->refcount++;
    }

    void Pick(Function&& src) {
        ptr = src.ptr;
        src.ptr = nullptr;
    }

public:
    Function()                                 { }

    template <class F> Function(F fn)          { ptr = new Wrapper<F>(std::move(fn)); }

    ~Function()                                { Free(ptr); }

    Function(const Function& src)              { Copy(src); }
    Function& operator=(const Function& src)   { auto b = ptr; Copy(src); Free(b); return *this; }

    Function(Function&& src)                   { Pick(std::move(src)); }
    Function& operator=(Function&& src)        { if(&src != this) { Free(ptr); ptr = src.ptr; src.ptr = nullptr; } return *this; }

    Function Proxy() const                     { return [=] (ArgTypes... args) { return (*this)(args...); }; }

    Res operator()(ArgTypes... args) const     { return ptr ? ptr->Execute(args...) : Res(); }

    operator bool() const                      { return (ptr != nullptr); }
    void Clear()                               { Free(ptr); ptr = nullptr; }


    friend Function Proxy(const Function& a)   { return a.Proxy(); }
    friend void Swap(Function& a, Function& b) { UPP::Swap(a.ptr, b.ptr); }
};

template <typename... ArgTypes>
using Event = Function<void (ArgTypes...)>;

template <typename... ArgTypes>
using Gate = Function<bool (ArgTypes...)>;

template <class Ptr, class Class, class Res, class... ArgTypes>
Function<Res (ArgTypes...)> MemFn(Ptr object, Res (Class::*method)(ArgTypes...))
{
    return [=](ArgTypes... args) { return (object->*method)(args...); };
}

#define THISFN(x)   MemFn(this, &CLASSNAME::x)

} // namespace rsp::utils

#endif /* INCLUDE_UTILS_FUNCTION_H_ */
