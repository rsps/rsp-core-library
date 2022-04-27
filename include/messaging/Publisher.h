/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#ifndef PUBLISHER_H
#define PUBLISHER_H

#include "Broker.h"
#include <iostream>

namespace rsp::messaging
{

template< typename T >
class Publisher
{
public:
    /**
     * \brief Create a publisher for a given broker.
     * \param arBroker Reference to Broker to publish through.
     */
    Publisher(Broker<T> &arBroker)
        : mrBroker(arBroker)
    {
    }
    Publisher(const Publisher&) = delete;
    Publisher& operator=(const Publisher&) = delete;

    virtual ~Publisher() {}

    /**
     * \brief Register a broker with this publisher
     * \param arBroker Reference to Broker
     * \deprecated
     *
     * This does not work as one would expect. The reference is not set, the arBroker content
     * is copied into original broker given in constructor.
     * A reference in C++ is an alias for the original object, not a pointer.
     */
//    void RegisterBroker(BrokerBase &arBroker);

    /**
     * \brief Publish the given enumerated topic through the registered broker.
     * \tparam T Enum type for topics
     * \param aTopic Topic to publish to
     * \param arNewEvent Event to publish
     */
    void PublishToBroker(T aTopic, Event &arNewEvent)
    {
        mrBroker.Publish(aTopic, arNewEvent);
    }

protected:
    Broker<T> &mrBroker;
};

} // namespace rsp::messaging

#endif // PUBLISHER_H
