/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <string>
#include <json/JsonStream.h>

namespace rsp::json {

JsonStream::JsonStream(bool aPrettyPrint, unsigned aLevel)
    : mPrettyPrint(aPrettyPrint),
      mRootLevel(aLevel)
{
    if (mPrettyPrint) {
        indentation = std::string( (mRootLevel) * 4, ' ');
        space = " ";
        newLine = "\n";
    }
}

JsonStream& operator <<(JsonStream &o, const Indent &arIndent)
{
    if (o.mPrettyPrint) {
        o.indentation = std::string( (o.mRootLevel + arIndent.mValue) * 4, ' ');
    }
    return o;
}

JsonStream& operator <<(JsonStream &o, const Comma &arComma)
{
    static_cast<std::ostringstream&>(o) << arComma.mValue << o.newLine << o.indentation;
    return o;
}

JsonStream& operator <<(JsonStream &o, const Key &arKey)
{
    static_cast<std::ostringstream&>(o) << "\"" << arKey.mpValue << "\":" << o.space;
    return o;
}

JsonStream& operator <<(JsonStream &o, const OBegin &)
{
    static_cast<std::ostringstream&>(o) << "{" << o.newLine << o.indentation;
    return o;
}

JsonStream& operator <<(JsonStream &o, const OEnd &)
{
    static_cast<std::ostringstream&>(o) << o.newLine << o.indentation << "}";
    return o;
}

JsonStream& operator <<(JsonStream &o, const ABegin &)
{
    static_cast<std::ostringstream&>(o) << "[" << o.newLine << o.indentation;
    return o;
}

JsonStream& operator <<(JsonStream &o, const AEnd &)
{
    static_cast<std::ostringstream&>(o) << o.newLine << o.indentation << "]";
    return o;
}

JsonStream& operator <<(JsonStream &o, const std::string &arStr)
{
    static_cast<std::ostringstream&>(o) << "\"" << arStr << "\"";
    return o;
}

JsonStream& operator<<(JsonStream& o, const char *apStr)
{
    static_cast<std::ostringstream&>(o) << "\"" << apStr << "\"";
    return o;
}

}

 // namespace rsp::json
