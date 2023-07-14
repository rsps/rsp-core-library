/*!
 * \copyright   Copyright (c) 2023 RSP Systems A/S. All rights reserved.
 * \author      steffen
 */

#ifndef INCLUDE_MESSAGING_NOTIFIER_H_
#define INCLUDE_MESSAGING_NOTIFIER_H_

#include <algorithm>
#include <functional>
#include <list>
#include <memory>
#include <vector>

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
    Endpoints& operator<<(std::shared_ptr<void> aEndpoint)
    {
        mListeners.push_back(aEndpoint);
        return *this;
    }

    Endpoints& Clear()
    {
        mListeners.clear();
        return *this;
    }

protected:
    std::vector<Notifier::Listener_t> mListeners{};
};


} /* namespace rsp::messaging */


#endif /* INCLUDE_MESSAGING_NOTIFIER_H_ */
