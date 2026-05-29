/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */


#ifndef RSP_CORE_LIB_JSON_JSON_DECODER_H
#define RSP_CORE_LIB_JSON_JSON_DECODER_H

#include <string>
#include <string_view>
#include <vector>
#include <rsp/utils/DynamicData.h>

namespace rsp::json {

/**
 * \class JsonString
 * \brief String derivative specialized in traversing self as a Json formatted string.
 */
class JsonDecoder : public std::string
{
public:
    /**
     * Constructor that takes a json formatted string.
     *
     * \param std::string
     */
    explicit JsonDecoder(std::string_view aJson);

    /**
     * Decode a value object from the content. The result can be a complex hierarchy of value objects.
     * \return JsonValue*
     */
    rsp::utils::DynamicData Decode();

protected:
    std::string::iterator mIt; // Current index iterator, this is always moving forward.
    std::string::iterator mEnd; // Current end iterator, to limit the end of the current extraction.
    std::vector<std::string::iterator> mStack{}; // Used to stack end iterators when parsing sub-strings.

    /**
     * \brief Traverse the string to locate start and end of a substring.
     * \param aToken1
     * \param aToken2
     */
    void findSubString(char aToken1, char aToken2);

    /**
     * \brief Push cursor on stack, when diving into an object or array.
     */
    void push();

    /**
     * \brief Pop cursor from the stack, when exiting an object or array.
     */
    void pop();

    /**
     * \brief Move cursor over all next whitespace characters.
     */
    void skipWhiteSpace();

    /**
     * \brief Parse the JSON content and extract next as a string.
     * \throw EJsonFormatError if contents is not correctly escaped according to json string rules.
     * \return string
     */
    std::string getString();

    /**
     * \brief Parse the JSON content and extract next as an object
     * \throw EJsonParseError if content is not correctly formatted json object elements
     * \return DynamicData with object content
     */
    rsp::utils::DynamicData getObject();

    /**
     * \brief Parse the JSON content and extract next as an array
     * \throw EJsonParseError if content is not correctly formatted json array elements
     * \return DynamicData with array content
     */
    rsp::utils::DynamicData getArray();

    /**
     * \brief Parse the JSON content and extract next as a number.
     * \throw EJsonNumberError if content has illegal number formatting.
     * \return DynamicData with numeric value in one of these native
     *         formats: double, int64_t or uint64_t.
     */
    rsp::utils::DynamicData getNumber();

    unsigned int getOffset() { return static_cast<unsigned int>(mIt - begin()); };
    unsigned int getLength() { return static_cast<unsigned int>(mEnd - mIt); };

    std::string debug(bool aIncludeText = false, bool aIncludeSubstr = false);
};

} /* rsp::json */

#endif // RSP_CORE_LIB_JSON_JSON_DECODER_H
