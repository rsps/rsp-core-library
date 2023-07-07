/*!
 * \copyright   Copyright (c) 2023 RSP Systems A/S. All rights reserved.
 * \author      steffen
 */

#ifndef INCLUDE_MESSAGING_EMITTER_H_
#define INCLUDE_MESSAGING_EMITTER_H_

#include <functional>
#include <list>

namespace rsp::messaging {

class EmitterBase;

class SubscriberBase
{
public:
    SubscriberBase() {}
    SubscriberBase(EmitterBase &arEmitter);
    virtual ~SubscriberBase();

    SubscriberBase(const SubscriberBase&);
    SubscriberBase& operator=(const SubscriberBase&);

    SubscriberBase(SubscriberBase&&);
    SubscriberBase& operator=(SubscriberBase&&);

    SubscriberBase& Attach(EmitterBase &arEmitter);
    SubscriberBase& Detach();

protected:
    friend class EmitterBase;
    EmitterBase *mpEmitter = nullptr;
};

class EmitterBase
{
public:
    ~EmitterBase();
    EmitterBase& Subscribe(SubscriberBase *apSubscriber);
    EmitterBase& Unsubscribe(SubscriberBase *apSubscriber);

protected:
    std::list<SubscriberBase*> mSubscribers{};
};


template <typename... ArgTypes>
class Subscriber : public SubscriberBase
{
public:
    Subscriber() {}

    Subscriber(std::function<void(ArgTypes...)> aFunction)
        : mFunction(aFunction)
    {
    }

    Subscriber(EmitterBase &arEmitter, std::function<void(ArgTypes...)> aFunction)
        : SubscriberBase(arEmitter),
          mFunction(aFunction)
    {
    }

    Subscriber& Set(std::function<void(ArgTypes...)> aFunction) {
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
class Emitter : public EmitterBase
{
public:
    using Type_t = Subscriber<ArgTypes...>;

    void Emit(ArgTypes... args) {
        for(SubscriberBase* sub : mSubscribers) {
            dynamic_cast<Type_t*>(sub)->Invoke(args...);
        }
    }
};

} /* namespace rsp::messaging */

#endif /* INCLUDE_MESSAGING_EMITTER_H_ */
