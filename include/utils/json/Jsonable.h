/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_UTILS_JSON_JSONABLE_H_
#define INCLUDE_UTILS_JSON_JSONABLE_H_

#include <string>
#include <sstream>

namespace rsp::utils {

enum class JsonableStreamAttributes { in0, in1, in2, in3, in4, in5, in6, in7, in8, in9, cnl, nl, sp };

std::ostream& operator<< (std::ostream& os, const JsonableStreamAttributes aAttrib);

class Jsonable
{
public:
    virtual ~Jsonable() {}

    virtual std::string ToJson(bool aPrettyPrint = true, int aLevel = 0) const = 0;

    class JsonStream : public std::stringstream {
    public:
        JsonStream(bool aPrettyPrint, int aLevel);

    protected:
        friend std::ostream& operator<< (std::ostream& os, const JsonableStreamAttributes aAttrib);
        int mRootLevel;
        std::string mCommaNewLine{};
        std::string mNewLine{};
        std::string mSpace{};
    };
};


} /* namespace rsp::utils */

#endif /* INCLUDE_UTILS_JSON_JSONABLE_H_ */
