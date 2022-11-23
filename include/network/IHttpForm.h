/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */


#ifndef INCLUDE_NETWORK_IHTTPFORM_H_
#define INCLUDE_NETWORK_IHTTPFORM_H_

#include <string>
#include <posix/FileIO.h>
#include "IHttpRequest.h"

namespace rsp::network {

class IHttpForm
{
public:
    virtual ~IHttpForm() {}

    /**
     * \fn IHttpForm AddField&(std::string_view, std::string_view)
     * \brief Add a form field as key/value pair
     *
     * \param aFieldName Name of the field
     * \param aValue Value of the field
     * \return self
     */
    virtual IHttpForm& AddField(const std::string &arFieldName, const std::string &arValue) = 0;

    /**
     * \fn IHttpForm AddFile&(std::string_view, rsp::posix::FileIO&)
     * \brief Add a file field to the form
     *
     * \param aFieldName Name of the field
     * \param arFile File value of the field
     * \return self
     */
    virtual IHttpForm& AddFile(const std::string &arFieldName, rsp::posix::FileIO &arFile) = 0;

};

} // namespace rsp::network

#endif /* INCLUDE_NETWORK_IHTTPFORM_H_ */
