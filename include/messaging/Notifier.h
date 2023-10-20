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

template< class ... Args >
class Notifier final
{
public:
    // Tokens held by listeners
    using Listener_t = std::shared_ptr<void>;

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


class Endpoints
{
public:
    Endpoints& operator<<(const std::shared_ptr<void>& arEndpoint)
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
    std::vector<std::shared_ptr<void>> mListeners{};
};


} /* namespace rsp::messaging */


#endif // RSP_CORE_LIB_MESSAGING_NOTIFIER_H
