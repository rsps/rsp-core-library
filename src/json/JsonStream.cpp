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

const int cIndentSize = 4;

JsonStream::JsonStream(bool aPrettyPrint, unsigned aLevel)
    : mPrettyPrint(aPrettyPrint),
      mRootLevel(aLevel)
{
    if (mPrettyPrint) {
        indentation = std::string( (mRootLevel) * cIndentSize, ' ');
        space = " ";
        newLine = "\n";
    }
}

size_t JsonStream::Getsize() const
{
    return size_t(view().size());;
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
    if (o.mPrettyPrint) {
        o.indentation += std::string( cIndentSize, ' ');
    }
    static_cast<std::ostringstream&>(o) << "{" << o.newLine << o.indentation;
    return o;
}

JsonStream& operator <<(JsonStream &o, const OEnd &)
{
    if (o.mPrettyPrint) {
        o.indentation.resize(o.indentation.size() - cIndentSize);
    }
    static_cast<std::ostringstream&>(o) << o.newLine << o.indentation << "}";
    return o;
}

JsonStream& operator <<(JsonStream &o, const ABegin &)
{
    if (o.mPrettyPrint) {
        o.indentation += std::string( cIndentSize, ' ');
    }
    static_cast<std::ostringstream&>(o) << "[" << o.newLine << o.indentation;
    return o;
}

JsonStream& operator <<(JsonStream &o, const AEnd &)
{
    if (o.mPrettyPrint) {
        o.indentation.resize(o.indentation.size() - cIndentSize);
    }
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
