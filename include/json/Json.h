/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_UTILS_JSON_JSON_H_
#define INCLUDE_UTILS_JSON_JSON_H_

#include "JsonExceptions.h"
#include "JsonValue.h"
#include "JsonArray.h"
#include "JsonObject.h"

namespace rsp::utils::json {

/**
 * \class Json
 * \brief Gateway class to all JSON operations.
 *
 * The class holds a pointer to the root
 * JsonValue of the Json object model.
 */
class Json
{
public:
    /**
     * Constructs an empty object
     */
    Json() : mpValue{nullptr} {}
    /**
     * Copy constructor
     * \param arOther Source to copy from
     */
    Json(const Json &arOther);
    /**
     * Move constructor
     * \param arOther Source to move from
     */
    Json(Json &&arOther);
    /**
     * Virtual destructor
     */
    virtual ~Json();

    /**
     * Copy assignment operator
     *
     * \param arOther Source to copy from
     * \return Reference to this
     */
    Json& operator=(const Json &arOther);

    /**
     * Move assignment operator
     *
     * \param arOther Source to move from
     * \return Reference to this
     */
    Json& operator=(Json &&arOther);

    /**
     * Create content of type JsonObject
     *
     * \return Reference to new JsonObject
     */
    JsonObject& MakeObject();
    /**
     * Create content of type JsonArray
     *
     * \return Reference to new JsonArray
     */
    JsonArray& MakeArray();

    /**
     * Decode a JSON formatted string and populate
     * the content with the result.
     *
     * \param arJson JSON formatted string
     */
    void Decode(const std::string &arJson);
    /**
     * Encode the content into a JSON formatted string
     *
     * \param aPrettyPrint If true the result is human readable formatted
     * \return JSON formatted string
     */
    std::string Encode(bool aPrettyPrint = false) const;

    /**
     * Access the internal JsonValue content
     *
     * \return
     */
    JsonValue& Get() const;
    JsonValue& operator*();
    JsonValue* operator->();

    /**
     * Erase the internal JsonValue content.
     *
     * \return Reference to this
     */
    Json& Clear();

    /**
     * Check if content is set
     *
     * \return True if content exist
     */
    bool Empty() const { return (!mpValue); }
protected:
    JsonValue *mpValue;
};

} /* namespace rsp::utils::json */

#endif /* INCLUDE_UTILS_JSON_JSON_H_ */
