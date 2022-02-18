#ifndef EVENT_H
#define EVENT_H

#include <type_traits>
#include <typeinfo>

namespace rsp::graphics
{

class Event
{
  public:
    Event(std::size_t aHash) : typeHash(aHash) {}
    Event() : typeHash(typeid(Event).hash_code()) {}
    ~Event() {}

    template <class T>
    T &GetAs()
    {
        if (typeid(T).hash_code() != typeHash) {
            std::cout << "- Is Not Same Type" << std::endl;
            // throw std::bad_alloc();
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

} // namespace rsp::graphics
#endif // EVENT_H
