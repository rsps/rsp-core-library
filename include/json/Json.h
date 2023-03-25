/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_JSON_JSON_H_
#define INCLUDE_JSON_JSON_H_

#include <json/JsonExceptions.h>
#include <json/JsonDecoder.h>
#include <json/JsonEncoder.h>
#include <json/JsonStream.h>

namespace rsp::json {

class Json : public rsp::utils::DynamicData
{
public:
    enum class Types : unsigned int { Null, Bool, Number, String, Object, Array };

    Json() : DynamicData() {}
    Json(std::string_view aJson);
    Json(const std::string &arJson) : Json(std::string_view(arJson)) {}
    Json(const rsp::utils::DynamicData &arData);
    Json(rsp::utils::DynamicData&& arData);

    /**
     * \brief Encode a DynamicData object to a JSON formatted string
     * \param aPrettyPrint Set to make the string human readable
     * \param aForceToUCS2 Set to use UCS2 codepoints for all characters above ASCII.
     * \return JSON formatted string
     */
    static std::string Encode(const rsp::utils::DynamicData &arData, bool aPrettyPrint = false, bool aForceToUCS2 = false, unsigned int aArrayLineLength = 0);
    std::string Encode(bool aPrettyPrint = false, bool aForceToUCS2 = false, unsigned int aArrayLineLength = 0) const;

    /**
     * \brief Decode a string into a DynamicData object
     * \param aJson JSON formatted string
     * \return JsonValue object
     */
    static rsp::utils::DynamicData Decode(std::string_view aJson);

    /**
     * \brief Get the type of the value content
     * \return JsonTypes
     */
    static Types GetJsonType(const rsp::utils::DynamicData &arData);
    Types GetJsonType();

    /**
     * \brief Get the type as a string. Useful for error logging.
     * \return string
     */
    static std::string GetJsonTypeAsString(Types aType);
    std::string GetJsonTypeAsString();
};

std::ostream& operator<<(std::ostream& os, Json::Types aType);
std::ostream& operator<<(std::ostream& os, const Json &arJson);


} /* namespace rsp::json */

#endif /* INCLUDE_JSON_JSON_H_ */
