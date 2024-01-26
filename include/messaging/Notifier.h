/*!
 * \copyright   Copyright (c) 2023 RSP Systems A/S. All rights reserved.
 * \author      steffen
 */

#ifndef RSP_CORE_LIB_MESSAGING_NOTIFIER_H
#define RSP_CORE_LIB_MESSAGING_NOTIFIER_H

#include <algorithm>
#include <functional>
#include <list>
#include <memory>
#include <vector>
#include <utils/Function.h>

namespace rsp::messaging {

// http://nercury.github.io/c++/interesting/2016/02/22/weak_ptr-and-event-cleanup.html
// https://stackoverflow.com/a/39920584

/**
 * \brief Type for Listener tokens, must be held by listeners as long as they wish to listen.
 */
using Listener = std::shared_ptr<void>;

/**
 * \brief Notification template class. Declares callback methods with any amount of arguments.
 * \tparam Args Argument list to declare for callback.
 */
template< class ... Args >
class Notifier final
{
public:
    using Listener_t = Listener;

    void operator()(Args ...args)
    {
        std::erase_if(mCallbacks, [](auto ptr) { return ptr.expired(); });
        auto tmp = mCallbacks;
        for (auto wp : tmp) {
            auto pf = wp.lock();
            if (pf && *pf) {
                (*pf)(args...);
            }
        }
    }

    Listener_t Listen(std::shared_ptr<std::function<void(Args...)> > f)
    {
        mCallbacks.push_back(f);
        return f;
    }

    Listener_t Listen(std::function<void(Args...)> f)
    {
        auto ptr = std::make_shared<std::function<void(Args...)> >(std::move(f));
        return Listen(ptr);
    }

protected:
    std::vector<std::weak_ptr<std::function<void(Args...)> > > mCallbacks{};
};

/**
 * \brief Container to store Listener tokens returned when listening to Notifier's
 */
class Endpoints
{
public:
    Endpoints& operator<<(const Listener& arEndpoint)
    {
        mListeners.push_back(arEndpoint);
        return *this;
    }

    Endpoints& Clear()
    {
        mListeners.clear();
        return *this;
    }

protected:
    std::vector<Listener> mListeners{};
};


} /* namespace rsp::messaging */


#endif // RSP_CORE_LIB_MESSAGING_NOTIFIER_H
