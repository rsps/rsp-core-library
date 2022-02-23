/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef INCLUDE_UTILS_JSON_JSONARRAY_H_
#define INCLUDE_UTILS_JSON_JSONARRAY_H_

#include <vector>
#include "JsonValue.h"

namespace rsp::utils::json {

/**
 * \class JsonArray
 * \brief Specialized derivative of JsonValue with array implementation
 */
class JsonArray : public JsonValue
{
public:
    /**
     * \fn JsonArray
     * \brief Construct empty array object
     */
    JsonArray();
    JsonArray(const JsonArray &arOther);
    JsonArray(JsonArray &&arOther);
    ~JsonArray() override;

    JsonArray& operator=(const JsonArray &arOther);
    JsonArray& operator=(JsonArray &&arOther);

    /**
     * \fn std::size_t GetCount() const
     * \brief Get number of elements in array
     * \return Integer count of elements
     */
    std::size_t GetCount() const;

    /**
     * \fn JsonValue& operator[](unsigned int aIndex)
     * \brief Operator to access specific elements
     * \param aIndex Integer index of element in array
     * \return Reference to array element
     */
    JsonValue& operator[](unsigned int aIndex);

    /**
     * \fn JsonArray& Add(JsonValue* apValue)
     * \brief Add a new element to the array
     * \param apValue Pointer to JsonValue object to add
     * \return Reference to this
     */
    JsonArray& Add(JsonValue* apValue);

    /**
     * \fn JsonArray& Remove(int aIndex)
     * \brief Destroy and remove a specified element object from the array.
     * \param aIndex Integer index of element to remove
     * \return Reference to this
     */
    JsonArray& Remove(int aIndex);

    /**
     * \fn void Clear()
     * \brief Clears the entire array and destroys all elements.
     */
    void Clear() override;

    /**
     * \fn bool IsArray()
     * \brief Interface override to return true for this object
     * \return True
     */
    bool IsArray() const override { return true; }

protected:
    std::vector<JsonValue*> mData{};

    void toStringStream(std::stringstream &arResult, PrintFormat &arPf, unsigned int aLevel, bool aForceToUCS2) override;
    JsonValue* clone() const override;
};


} /* namespace rsp::utils::json */

#endif /* INCLUDE_UTILS_JSON_JSONARRAY_H_ */
