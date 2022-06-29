/**
 * \copyright    Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license      Mozilla Public License 2.0
 * \author:      Jesper Madsen
 * Created Date:  Tuesday, May 17th 2022, 8:49:44 am
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * HISTORY:
 * Date      	By	Comments
 * ----------	---	----------------------------------------------------------
 */

#ifndef NETWORKEXCEPTION_H
#define NETWORKEXCEPTION_H

#include <utils/CoreException.h>
#include <string>

namespace rsp::network::exceptions
{
/**
 * \class NetworkException
 *
 * \brief Base exception for handling network related issues in the rsp::network namespace
 *
 */
class NetworkException: public rsp::utils::CoreException
{
public:
    explicit NetworkException(const std::string &aMsg)
        : rsp::utils::CoreException(aMsg)
    {
    }
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
    explicit ERequestOptions(const std::string &aMsg)
        : NetworkException(aMsg)
    {
    }
};

}// namespace rsp::network::exceptions
#endif //NETWORKEXCEPTION_H
