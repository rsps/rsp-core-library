/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#ifndef EVENT_H
#define EVENT_H

#include <string_view>
#include <type_traits>
#include <typeinfo>
#include <exceptions/ExceptionHelper.h>
#include <utils/ConstTypeInfo.h>

namespace rsp::messaging
{

class Event
{
  public:
    size_t Type;
    std::string_view Name;

//    Event() : mcType(0), mcName("") {}
//    Event(size_t aType) : mcType(aType) {}
    Event(size_t aType, std::string_view aName) : Type(aType), Name(aName) {}

    template<class T>
    T& GetAs() {
        if (T::ClassType != Type) {
            THROW_WITH_BACKTRACE(std::bad_alloc);
        }
        return *static_cast<T*>(this);
    }
};

template <class T>
class EventBase : public Event
{
  public:
    static constexpr size_t ClassType = rsp::utils::ID<T>();
    static constexpr std::string_view ClassName = rsp::utils::NameOf<T>();

    EventBase() : Event(ClassType, ClassName) {}
    EventBase(size_t aType, std::string_view aName) : Event(aType, aName) {}
};

} // namespace rsp::messaging
#endif // EVENT_H
