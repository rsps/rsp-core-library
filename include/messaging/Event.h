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

#include <type_traits>
#include <typeinfo>

namespace rsp::messaging
{

class Event
{
  public:
    Event(std::size_t aHash) : typeHash(aHash) {}
    Event() : typeHash(typeid(Event).hash_code()) {}
    virtual ~Event() {}

    /**
     * \brief Casts the event to the templated type
     * \return A reference to the object after casting
     */
    template <class T>
    T &GetAs()
    {
        if (typeid(T).hash_code() != typeHash) {
            throw std::bad_alloc();
        }
        return *reinterpret_cast<T *>(this);
    }

    std::size_t typeHash;
};

template <class T>
class EventType : public Event
{
  public:
    EventType() : Event(typeid(T).hash_code()) {}
};

} // namespace rsp::messaging
#endif // EVENT_H