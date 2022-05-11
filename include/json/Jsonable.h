/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_JSON_JSONABLE_H_
#define INCLUDE_JSON_JSONABLE_H_

#include <memory>
#include <json/JsonValue.h>

namespace rsp::json {

/**
 * \class Jsonable
 * \brief Interface with helpers for building json formatted stringstream
 */
class Jsonable
{
public:
    virtual ~Jsonable() {}

    /**
     * \brief Interface to get object content as a JSON item.
     * \return JSON formatted string
     */
    virtual std::unique_ptr<JsonValue> ToJson() const = 0;

    /**
     * \brief Interface to populate object content with content from a JSON item
     * \param arJson Reference to JsonValue object
     */
    virtual void FromJson(const JsonValue &arJson);
};


} /* namespace rsp::utils */

#endif /* INCLUDE_JSON_JSONABLE_H_ */
