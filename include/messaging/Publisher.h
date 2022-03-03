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

#include "messaging/Broker.h"
#include <iostream>

namespace rsp::messaging
{

class Publisher
{
  public:
    // Publisher() : mrBroker(nullptr) {}

    /**
     * \brief Create a publisher for a given broker.
     * \param arBroker Reference to Broker to publish through.
     */
    Publisher(BrokerBase &arBroker) : mrBroker(arBroker) {}
    Publisher(const Publisher &) = delete;
    Publisher &operator=(const Publisher &) = delete;

    virtual ~Publisher() {}

    /**
     * \brief Register a broker with this publisher
     * \param arBroker Reference to Broker
     * \deprecated
     */
    void RegisterBroker(BrokerBase &arBroker);

    /**
     * \brief Publish the given enumerated topic through the registered broker.
     * \tparam T Enum type for topics
     * \param aTopic Topic to publish to
     * \param arNewEvent Event to publish
     */
    template <typename T>
    void PublishToBroker(T aTopic, Event &arNewEvent)
    {
        mrBroker.doPublish(static_cast<int>(aTopic), arNewEvent);
    }

  protected:
    /*
     * TODO:
     * By removing registerBroker and only using new constructor, the following
     * can be changed to:
     * BrokerBase &mrBroker;
     */
    BrokerBase &mrBroker;
};
} // namespace rsp::messaging

#endif // PUBLISHER_H
