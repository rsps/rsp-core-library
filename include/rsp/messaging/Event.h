/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#ifndef RSP_CORE_LIB_MESSAGING_EVENT_H
#define RSP_CORE_LIB_MESSAGING_EVENT_H

#include <memory>
#include <string_view>
#include <rsp/exceptions/CoreException.h>
#include <rsp/utils/ConstTypeInfo.h>

namespace rsp::messaging
{

class Event
{
  public:
    size_t Type;
    std::string_view Name;

    explicit Event(const size_t aType = 0, const std::string_view aName = "") : Type(aType), Name(aName) {}
    virtual ~Event() = default;

    Event(const Event& arOther) = default;

    Event& operator=(const Event& arOther) {
        if (&arOther != this) {
            Type = arOther.Type;
            Name = arOther.Name;
        }
        return *this;
    }

    template<class T>
    [[nodiscard]] bool IsType() const {
        return (T::ClassType == Type);
    }

    template<class T>
    T& CastTo() {
        if (!IsType<T>()) {
            THROW_WITH_BACKTRACE2(rsp::exceptions::EBadCast, Name, T::ClassName);
        }
        return dynamic_cast<T&>(*this);
    }

    template<class T>
    const T& CastTo() const {
        if (!IsType<T>()) {
            THROW_WITH_BACKTRACE2(rsp::exceptions::EBadCast, Name, T::ClassName);
        }
        return dynamic_cast<const T&>(*this);
    }

    virtual void ToStream(std::ostream &os) const
    {
        os << "Event (" << Type << ") " << Name;
    }

    friend std::ostream& operator<<(std::ostream &os, const Event &arEvent)
    {
        arEvent.ToStream(os);
        return os;
    }

};

using EventPtr_t = std::shared_ptr<Event>;

template <class T>
class EventBase : public Event
{
  public:
    static constexpr size_t ClassType = utils::ID<T>();
    static constexpr std::string_view ClassName = utils::NameOf<T>();

    EventBase() : Event(ClassType, ClassName) {}
    EventBase(const size_t aType, const std::string_view aName) : Event(aType, aName) {}
};


} // namespace rsp::messaging
#endif // RSP_CORE_LIB_MESSAGING_EVENT_H
