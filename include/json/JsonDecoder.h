/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */


#ifndef INCLUDE_JSON_JSONDECODER_H_
#define INCLUDE_JSON_JSONDECODER_H_

#include <string>
#include <string_view>
#include <vector>
#include <utils/DynamicData.h>

namespace rsp::json {

/**
 * \class JsonString
 * \brief String derivative specialized in traversing a Json formatted string.
 */
class JsonDecoder : public std::string
{
public:
    /**
     * Constructor that takes a json formatted string.
     *
     * \param std::string
     */
    JsonDecoder(std::string_view aJson);

    /**
     * Decode a value object from the content. The result can be a complex hierarchy of value objects.
     * \return JsonValue*
     */
    rsp::utils::DynamicData Decode();

protected:
    std::string::iterator mIt; // Current index iterator, this is always moving forward.
    std::string::iterator mEnd; // Current end iterator, to limit the end of the current extraction.
    std::vector<std::string::iterator> mStack{}; // Used to stack end iterators when parsing sub-strings.

    void findSubString(const char aToken1, const char aToken2);
    void push();
    void pop();
    void skipWhiteSpace();
    std::string getString();
    rsp::utils::DynamicData getObject();
    rsp::utils::DynamicData getArray();
    rsp::utils::DynamicData getNumber();

    unsigned int getOffset() { return static_cast<unsigned int>(mIt - begin()); };
    unsigned int getLength() { return static_cast<unsigned int>(mEnd - mIt); };

    std::string debug(bool aIncludeText = false, bool aIncludeSubstr = false);
};

} /* rsp::json */

#endif /* INCLUDE_JSON_JSONDECODER_H_ */
