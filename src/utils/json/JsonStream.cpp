/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#include <json/JsonStream.h>

namespace rsp::json {

JsonStream::JsonStream(bool aPrettyPrint, int aLevel)
    : mRootLevel(-1)
{
    if (aPrettyPrint) {
        mRootLevel = aLevel;
        mNewLine = "\n";
        mCommaNewLine = ",\n";
        mSpace = " ";
    }
    else {
        mCommaNewLine = ",";
    }
}

std::ostream& operator<< (std::ostream& os, const JsonStream::Attributes aAttrib) {

    JsonStream* js = static_cast<JsonStream*>(&os);

    switch (aAttrib) {
        case JsonStream::Attributes::cnl:
            os << js->mCommaNewLine;
            break;
        case JsonStream::Attributes::nl:
            os << js->mNewLine;
            break;
        case JsonStream::Attributes::sp:
            os << js->mSpace;
            break;

        default:
            if (js->mRootLevel >= 0) {
                os << std::string( (static_cast<unsigned int>(js->mRootLevel) + static_cast<unsigned int>(aAttrib)) * 4, ' ');
            }
            break;
    }
    return os;
}


} // namespace rsp::json


