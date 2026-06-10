/*!
 * \copyright    Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license      Mozilla Public License 2.0
 * \author:      Jesper Madsen
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef RSP_CORE_LIB_NETWORK_NETWORK_EXCEPTION_H
#define RSP_CORE_LIB_NETWORK_NETWORK_EXCEPTION_H

#include <rsp/exceptions/CoreException.h>
#include <string>

namespace rsp::network
{

/**
 * \class NetworkException
 *
 * \brief Base exception for handling network related issues in the rsp::network namespace
 *
 */
class NetworkException: public exceptions::CoreException
{
public:
    using exceptions::CoreException::CoreException;
};

/**
 * \brief Exception needed by TLS connections, to inform when connection must be reconnected to perform the desired operation.
 */
class ENetReconnect: public NetworkException
{
public:
    using NetworkException::NetworkException;
};

/**
 * \class ERequestOptions
 *
 * \brief Exception regarding Request options
 *
 */
class ERequestOptions: public NetworkException
{
public:
    using NetworkException::NetworkException;
};



}// namespace rsp::network

#endif // RSP_CORE_LIB_NETWORK_NETWORK_EXCEPTION_H
