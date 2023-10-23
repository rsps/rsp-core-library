/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_UTILS_FUNCTION_H
#define RSP_CORE_LIB_UTILS_FUNCTION_H

#include <functional>

namespace rsp::utils {

//template<typename Res, typename ... ArgTypes>
//class Function;

/*
template<typename Res, typename ... ArgTypes>
class Function<Res(ArgTypes...)> : public std::function<Res(ArgTypes...)>
{
public:
    using std::function<Res(ArgTypes...)>::function;

    Res operator()(ArgTypes ... args) const
    {
        return (*this) ? std::function<Res(ArgTypes...)>::operator()(args...) : Res();
    }

};
*/
template<class Ptr, class Class, class Res, class ... ArgTypes>
std::function<Res(ArgTypes...)> Method(Ptr object, Res (Class::*method)(ArgTypes...))
{
    return [=](ArgTypes ... args)
    {
        return (object->*method)(args...);
    };
}

} // namespace rsp::utils

#endif // RSP_CORE_LIB_UTILS_FUNCTION_H
