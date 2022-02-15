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

class JsonString : public std::string
{
public:
    JsonString(const std::string &arJson): std::string(arJson) { mStack.emplace_back(begin(), end()); }

    void Push(const char aToken1, const char aToken2);
    void Pop();
    void SkipWhiteSpace();
    std::string GetString();
    JsonValue* GetValue();

protected:
    std::string::iterator mIt; // Current index iterator
    std::string::iterator mEnd; // Current end iterator
    std::vector<std::pair<std::string::iterator, std::string::iterator>> mStack{}; // begin, end iterators
};

} /* rsp::utils::json */

#endif /* INCLUDE_UTILS_JSON_JSONSTRING_H_ */
