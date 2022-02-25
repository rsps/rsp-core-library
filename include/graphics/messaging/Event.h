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

namespace rsp::graphics
{
enum class Topic {
    Base,
    SceneChange
};

class Event
{
  public:
    Event(std::size_t aHash) : mTypeHash(aHash) {}
    Event() : mTypeHash(typeid(Event).hash_code()) {}
    ~Event() {}

    template <class T>
    T &GetAs()
    {
        if (typeid(T).hash_code() != mTypeHash) {
            std::cout << "- Is Not Same Type" << std::endl;
            // throw std::bad_alloc();
        }
        return *reinterpret_cast<T *>(this);
    }

    std::size_t mTypeHash;
};

template <class T>
class EventType : public Event
{
  public:
    EventType() : Event(typeid(T).hash_code()) {}
};

} // namespace rsp::graphics
#endif // EVENT_H
