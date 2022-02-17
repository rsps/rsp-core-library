/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */


#ifndef INCLUDE_UTILS_JSON_JSONSTRING_H_
#define INCLUDE_UTILS_JSON_JSONSTRING_H_

#include <string>
#include <vector>
#include "JsonValue.h"

namespace rsp::utils::json {

class JsonValue;

/**
 * \class JsonString
 * \brief String derivative specialized in traversing a Json formatted string.
 */
class JsonString : public std::string
{
public:
    /**
     * Constructor that takes a json formatted string.
     *
     * \param std::string
     */
    JsonString(const std::string &arJson);
    JsonString(const JsonString &arJson);

    /**
     * Decode a value object from the content. The result can be a complex hierarchy of value objects.
     * \return JsonValue*
     */
    JsonValue* GetValue();

protected:
    std::string::iterator mIt; // Current index iterator, this is always moving forward.
    std::string::iterator mEnd; // Current end iterator, to limit the end of the current extraction.
    std::vector<std::string::iterator> mStack{}; // Used to stack end iterators when parsing sub-strings.

    void findSubString(const char aToken1, const char aToken2);
    void push();
    void pop();
    void skipWhiteSpace();
    std::string getString();
    JsonValue* getObject();
    JsonValue* getArray();
    JsonValue* getNumber();

    unsigned int getOffset() { return static_cast<unsigned int>(mIt - begin()); };
    unsigned int getLength() { return static_cast<unsigned int>(mEnd - mIt); };

    std::string debug(bool aIncludeText = false, bool aIncludeSubstr = false);
};

} /* rsp::utils::json */

#endif /* INCLUDE_UTILS_JSON_JSONSTRING_H_ */
