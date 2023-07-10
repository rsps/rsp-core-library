/*!
 * \copyright   Copyright (c) 2023 RSP Systems A/S. All rights reserved.
 * \author      steffen
 */

#include <messaging/Notifier.h>

namespace rsp::messaging {

ListenerBase::ListenerBase()
    : mpEmitter(nullptr)
{
}

ListenerBase::ListenerBase(NotifierBase &arEmitter)
    : mpEmitter(&arEmitter)
{
    arEmitter.Subscribe(this);
}

ListenerBase::~ListenerBase()
{
    if (mpEmitter) {
        mpEmitter->Unsubscribe(this);
    }
}

ListenerBase::ListenerBase(const ListenerBase& arOther)
{
    mpEmitter = arOther.mpEmitter;
    mpEmitter->Subscribe(this);
}

ListenerBase& ListenerBase::operator =(const ListenerBase& arOther)
{
    if (&arOther != this) {
        mpEmitter = arOther.mpEmitter;
        mpEmitter->Subscribe(this);
    }
    return *this;
}

ListenerBase::ListenerBase(ListenerBase&& arOther)
{
    mpEmitter = arOther.mpEmitter;
    arOther.mpEmitter->Unsubscribe(&arOther);
    arOther.mpEmitter = nullptr;
    mpEmitter->Subscribe(this);
}

ListenerBase& ListenerBase::operator =(ListenerBase&& arOther)
{
    if (&arOther != this) {
        mpEmitter = arOther.mpEmitter;
        arOther.mpEmitter->Unsubscribe(&arOther);
        arOther.mpEmitter = nullptr;
        mpEmitter->Subscribe(this);
    }
    return *this;
}

ListenerBase& ListenerBase::Attach(NotifierBase &arEmitter)
{
    mpEmitter = &arEmitter;
    mpEmitter->Subscribe(this);
    return *this;
}

ListenerBase& ListenerBase::Detach()
{
    if (mpEmitter) {
        mpEmitter->Unsubscribe(this);
        mpEmitter = nullptr;
    }
    return *this;
}



NotifierBase::~NotifierBase()
{
    for(ListenerBase* sub : mSubscribers) {
        sub->mpEmitter = nullptr;
    }
}

NotifierBase& NotifierBase::Subscribe(ListenerBase *apSubscriber)
{
    if (!apSubscriber) {
        return *this;
    }

    for(ListenerBase* sub : mSubscribers) {
        if (sub == apSubscriber) {
            return *this; // Already subscribed, do nothing
        }
    }

    mSubscribers.push_back(apSubscriber);

    return *this;
}

NotifierBase& NotifierBase::Unsubscribe(ListenerBase *apSubscriber)
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
