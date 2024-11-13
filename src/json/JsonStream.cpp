/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <cstring>
#include <string>
#include <json/JsonExceptions.h>
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

size_t JsonStream::Getsize()
{
    return size_t(tellp());
}

void JsonStream::StringToStream(const std::string& arString, std::ostream& o, bool aForceToUCS2)
{
    StringToStream(arString.data(), arString.size(), o, aForceToUCS2);
}

void JsonStream::StringToStream(std::string_view aString, std::ostream& o, bool aForceToUCS2)
{
    StringToStream(aString.data(), aString.size(), o, aForceToUCS2);
}

void JsonStream::StringToStream(const char *apString, size_t aSize, std::ostream& o, bool aForceToUCS2)
{
    auto s = apString;
    std::size_t i = 0;
    o << "\"";
    while (i < aSize) {
        auto c = *apString++;
        switch (c) {
            case '\"':
                o << "\\\"";
                break;
            case '\\':
                o << "\\\\";
                break;
            case '\b':
                o << "\\b";
                break;
            case '\f':
                o << "\\f";
                break;
            case '\n':
                o << "\\n";
                break;
            case '\r':
                o << "\\r";
                break;
            case '\t':
                o << "\\t";
                break;
            default:
                if (aForceToUCS2 && static_cast<uint8_t>(c) > 127) {
                    int v;
                    char buf[12];
                    switch (static_cast<uint8_t>(c) & 0xE0) {
                        case 0xE0:
                            v =   ((static_cast<int>(s[i]) & 0x0F) << 12)
                                  + ((static_cast<int>(s[i+1]) & 0x3F) << 6)
                                  + (static_cast<int>(s[i+2]) & 0x3F);
                            sprintf(buf, "\\u%04x", v);
                            o << buf;
                            i += 4;
                            break;

                        case 0xC0:
                            v =   ((static_cast<int>(s[i]) & 0x1F) << 6)
                                  + (static_cast<int>(s[i+1]) & 0x3F);
                            sprintf(buf, "\\u%04x", v);
                            o << buf;
                            i += 4;
                            break;

                        default:
                            THROW_WITH_BACKTRACE1(EJsonParseError, std::string("String has illegal JSON character: ") + c + " (" + std::to_string(int(c)) + ")");
                    }
                }
                else {
                    o << c;
                }
                break;
        }
        i++;
    }
    o << "\"";
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
    JsonStream::StringToStream(arStr, o);
    return o;
}

JsonStream& operator<<(JsonStream& o, const std::string_view& arStr)
{
    JsonStream::StringToStream(arStr, o);
    return o;
}

JsonStream& operator<<(JsonStream& o, const char *apStr)
{
    JsonStream::StringToStream(apStr, std::strlen(apStr), o);
    return o;
}

JsonStream& operator<<(JsonStream &o, const Null&)
{
    static_cast<std::ostringstream&>(o) << "null";
    return o;
}

JsonStream& operator<<(JsonStream& o, const bool& arBool)
{
    static_cast<std::ostringstream&>(o) << (arBool ? "true" : "false");
    return o;
}

JsonStream& operator<<(JsonStream& o, const Raw& arJson)
{
    static_cast<std::ostringstream&>(o) << arJson.mJson;
    return o;
}

JsonStream& operator<<(JsonStream& o, const utils::Variant& arValue)
{
    using namespace rsp::utils;
    switch (arValue.GetType()) {
        case Variant::Types::Null:
            o << Null();
            break;
        case Variant::Types::Bool:
            o << arValue.AsBool();
            break;
        case Variant::Types::Int:
        case Variant::Types::Int64:
            o << arValue.AsInt();
            break;
        case Variant::Types::Uint64:
        case Variant::Types::Uint32:
        case Variant::Types::Uint16:
            o << uint64_t(arValue);
            break;
        case Variant::Types::Float:
            o << arValue.AsFloat();
            break;
        case Variant::Types::Double:
            o << arValue.AsDouble();
            break;
        case Variant::Types::Pointer:
            o << static_cast<void*>(arValue);
            break;
        case Variant::Types::String:
        case Variant::Types::Object:
        case Variant::Types::Array:
            o << arValue.AsString();
            break;
    }
    return o;
}

} // namespace rsp::json
