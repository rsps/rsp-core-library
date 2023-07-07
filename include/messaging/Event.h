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

#include <any>
#include <string_view>
#include <type_traits>
#include <typeinfo>
#include <exceptions/CoreException.h>
#include <utils/ConstTypeInfo.h>

namespace rsp::messaging
{

class Event
{
  public:
    size_t Type;
    std::string_view Name;

    Event(size_t aType = 0, std::string_view aName = "") : Type(aType), Name(aName) {}
    virtual ~Event() {}

    Event(const Event& arOther) : Type(arOther.Type), Name(arOther.Name) {}

    Event& operator=(const Event& arOther) {
        if (&arOther != this) {
            Type = arOther.Type;
            Name = arOther.Name;
        }
        return *this;
    }

    template<class T>
    T& CastTo() {
        if (T::ClassType != Type) {
            THROW_WITH_BACKTRACE2(rsp::exceptions::EBadCast, Name, T::ClassName);
        }
        return dynamic_cast<T&>(*this);
    }

    template<class T>
    const T& CastTo() const {
        if (T::ClassType != Type) {
            THROW_WITH_BACKTRACE2(rsp::exceptions::EBadCast, Name, T::ClassName);
        }
        return dynamic_cast<const T&>(*this);
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
