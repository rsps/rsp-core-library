/*!
 * \copyright   Copyright (c) 2023 RSP Systems A/S. All rights reserved.
 * \author      steffen
 */

#include <messaging/Emitter.h>

namespace rsp::messaging {

SubscriberBase::SubscriberBase(EmitterBase &arEmitter)
{
    arEmitter.Subscribe(this);
    mpEmitter = &arEmitter;
}

SubscriberBase::~SubscriberBase()
{
    if (mpEmitter) {
        mpEmitter->Unsubscribe(this);
    }
}

SubscriberBase::SubscriberBase(const SubscriberBase& arOther)
{
    mpEmitter = arOther.mpEmitter;
    mpEmitter->Subscribe(this);
}

SubscriberBase& SubscriberBase::operator =(const SubscriberBase& arOther)
{
    if (&arOther != this) {
        mpEmitter = arOther.mpEmitter;
        mpEmitter->Subscribe(this);
    }
    return *this;
}

SubscriberBase::SubscriberBase(SubscriberBase&& arOther)
{
    mpEmitter = arOther.mpEmitter;
    arOther.mpEmitter->Unsubscribe(&arOther);
    arOther.mpEmitter = nullptr;
    mpEmitter->Subscribe(this);
}

SubscriberBase& SubscriberBase::operator =(SubscriberBase&& arOther)
{
    if (&arOther != this) {
        mpEmitter = arOther.mpEmitter;
        arOther.mpEmitter->Unsubscribe(&arOther);
        arOther.mpEmitter = nullptr;
        mpEmitter->Subscribe(this);
    }
    return *this;
}

SubscriberBase& SubscriberBase::Attach(EmitterBase &arEmitter)
{
    mpEmitter = &arEmitter;
    mpEmitter->Subscribe(this);
    return *this;
}

SubscriberBase& SubscriberBase::Detach()
{
    if (mpEmitter) {
        mpEmitter->Unsubscribe(this);
        mpEmitter = nullptr;
    }
    return *this;
}



EmitterBase::~EmitterBase()
{
    for(SubscriberBase* sub : mSubscribers) {
        sub->mpEmitter = nullptr;
    }
}

EmitterBase& EmitterBase::Subscribe(SubscriberBase *apSubscriber)
{
    if (!apSubscriber) {
        return *this;
    }

    for(SubscriberBase* sub : mSubscribers) {
        if (sub == apSubscriber) {
            return *this; // Already subscribed, do nothing
        }
    }

    mSubscribers.push_back(apSubscriber);

    return *this;
}

EmitterBase& EmitterBase::Unsubscribe(SubscriberBase *apSubscriber)
{
    if (!apSubscriber) {
        return *this;
    }

    for (auto it = mSubscribers.cbegin(); it != mSubscribers.cend() ; ++it) {
        if (*it == apSubscriber) {
            mSubscribers.erase(it);
            break;
        }
    }

    return *this;
}

} /* namespace rsp::messaging */
