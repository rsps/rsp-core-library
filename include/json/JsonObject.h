/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef INCLUDE_JSON_JSONOBJECT_H_
#define INCLUDE_JSON_JSONOBJECT_H_

#include <json/JsonString.h>
#include <json/JsonValue.h>
#include <utils/InsertOrderedMap.h>

namespace rsp::json {

/**
 * \class JsonObject
 * \brief Specialized derivative of JsonValue with object implementation
 */
class JsonObject : public JsonValue
{
public:
    /**
     * \fn JsonObject()
     * \brief Construct an empty object
     */
    JsonObject();

    JsonObject(const JsonObject &arOther);
    JsonObject(JsonObject &&arOther);
    ~JsonObject() override;

    JsonObject& operator=(const JsonObject &arOther);
    JsonObject& operator=(JsonObject &&arOther);

    /**
     * \fn std::size_t GetCount() const
     * \brief Get the number of members in the object
     * \return size_t Number of members
     */
    std::size_t GetCount() const;

    /**
     * \fn bool MemberExists(const std::string &arName) const
     * \brief Check if a member exists
     * \param arName Name of member to find
     * \return True if the member exists.
     */
    bool MemberExists(const std::string &arName) const;

    /**
     * \fn JsonValue& operator[](const char *apName);
     * \brief Get the value assigned to the given member
     * \param apName Name of member to ask for
     * \return Reference to JsonValue
     */
    JsonValue& operator[](const char *apName);
    JsonValue& operator[](const std::string &arName);

    /**
     * \fn JsonObject& Add(const std::string &arName, JsonValue* apValue)
     * \brief Add a newly created value to this object. The object takes ownership.
     * \param arName Name for the new member
     * \param apValue Value to add to this object
     * \return Reference to this
     */
    JsonObject& Add(const std::string &arName, JsonValue* apValue);

    /**
     * \fn JsonObject& Remove(const std::string &arName)
     * \brief Remove the named member from the object
     * \param arName Name of the member to remove
     * \return Reference to this
     */
    JsonObject& Remove(const std::string &arName);

    /**
     * \fn void Clear()
     * \brief Clear the entire object and destroy its members.
     */
    void Clear() override;

    /**
     * \fn bool IsObject() const
     * \brief Overriden test function to tell this is a JsonObject.
     * \return True
     */
    bool IsObject() const override { return true; }

protected:
    friend JsonString;
    rsp::utils::InsertOrderedMap<std::string, JsonValue*> mData{};

    void toStringStream(std::stringstream &arResult, PrintFormat &arPf, unsigned int aLevel, bool aForceToUCS2) override;
    JsonValue* clone() const override;
};


} /* namespace rsp::json */

#endif /* INCLUDE_JSON_JSONOBJECT_H_ */
