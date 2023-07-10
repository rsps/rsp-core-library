/*!
 * \copyright   Copyright (c) 2023 RSP Systems A/S. All rights reserved.
 * \author      steffen
 */

#ifndef INCLUDE_MESSAGING_NOTIFIER_H_
#define INCLUDE_MESSAGING_NOTIFIER_H_

#include <functional>
#include <list>

namespace rsp::messaging {

class NotifierBase;

class ListenerBase
{
public:
    ListenerBase();
    ListenerBase(NotifierBase &arEmitter);
    virtual ~ListenerBase();

    ListenerBase(const ListenerBase&);
    ListenerBase& operator=(const ListenerBase&);

    ListenerBase(ListenerBase&&);
    ListenerBase& operator=(ListenerBase&&);

    ListenerBase& Attach(NotifierBase &arEmitter);
    ListenerBase& Detach();

protected:
    friend class NotifierBase;
    NotifierBase *mpEmitter = nullptr;
};

class NotifierBase
{
public:
    ~NotifierBase();
    NotifierBase& Subscribe(ListenerBase *apSubscriber);
    NotifierBase& Unsubscribe(ListenerBase *apSubscriber);

protected:
    std::list<ListenerBase*> mSubscribers{};
};


template <typename... ArgTypes>
class Listener : public ListenerBase
{
public:
    Listener()
        : ListenerBase() {};

    Listener(std::function<void(ArgTypes...)> aFunction)
        : mFunction(aFunction)
    {
    }

    Listener(NotifierBase &arEmitter, std::function<void(ArgTypes...)> aFunction)
        : ListenerBase(arEmitter),
          mFunction(aFunction)
    {
    }

    Listener& Set(std::function<void(ArgTypes...)> aFunction) {
        mFunction = aFunction;
    }

    void Invoke(ArgTypes ... args) {
        if (mFunction) {
            mFunction(args...);
        }
    }

protected:
    std::function<void(ArgTypes...)> mFunction{};
};


template <typename... ArgTypes>
class Notifier : public NotifierBase
{
public:
    using ListenerType = Listener<ArgTypes...>;

    void Emit(ArgTypes... args) {
        for(ListenerBase* sub : mSubscribers) {
            dynamic_cast<ListenerType*>(sub)->Invoke(args...);
        }
    }
};

} /* namespace rsp::messaging */


#endif /* INCLUDE_MESSAGING_NOTIFIER_H_ */
