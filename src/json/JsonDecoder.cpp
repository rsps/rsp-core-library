/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <json/JsonDecoder.h>
#include <json/JsonExceptions.h>
#include <sstream>
#include <logging/Logger.h>
#include <utils/StrUtils.h>

using namespace rsp::json;
using namespace rsp::utils;

//#define JLOG(a) DLOG(a)
#define JLOG(a)

JsonDecoder::JsonDecoder(std::string_view aJson)
    : std::string(aJson),
      mIt(begin()),
      mEnd(end())
{
    JLOG("JsonString Created: " << std::distance(begin(), mIt) << ", " << getLength());
}

/**
 * Traverse the string to locate start and end of a substring.
 *
 * \param aToken1
 * \param aToken2
 */
void JsonDecoder::findSubString(const char aToken1, const char aToken2)
{
    JLOG("findSubString(" << aToken1 << ", " << aToken2 << "), " << debug());
    skipWhiteSpace();

    auto it = mIt;

    if (it == mEnd) {
        THROW_WITH_BACKTRACE1(EJsonParseError, "Substring was only whitespace");
    }

    if (*it != aToken1) {
        THROW_WITH_BACKTRACE1(EJsonParseError, "Substring start token was not found. " + debug(false, true));
    }
    it++;
    mIt = it;

    int indent = 0;

    while (it != mEnd) {
        if (*it == aToken2) {
            if (indent == 0) {
                push();
                mEnd = it;
                JLOG("findSubString stacked: " << std::string(mIt, mEnd));
                return;
            }
            else {
                indent--;
            }
        }
        else if (*it == aToken1) {
            indent++;
        }
        else if (*it == '\\') {
            it++;
            if (*it == 'u') {
                it += 4; // Advance u + 4 hex digits
            }
        }
        it++;
    }

    THROW_WITH_BACKTRACE1(EJsonParseError, "End token was not found. " + aToken2);
}

void JsonDecoder::push()
{
    mStack.emplace_back(mEnd);
}

void JsonDecoder::pop()
{
    mEnd = mStack.back();
    mStack.pop_back();
}

void JsonDecoder::skipWhiteSpace()
{
    while (mIt != mEnd) {
        switch (*mIt) {
            case ' ':
            case '\n':
            case '\r':
            case '\t':
                mIt++;
                break;

            default:
                return;
                break;
        }
    }
}

std::string JsonDecoder::getString()
{
    JLOG("getString: " << debug(false, true));
    findSubString('"', '"');

    std::string result;

    while (mIt != mEnd) {
        if (*mIt == '\\') {
            mIt++;
            switch (*mIt) {
                case 'u':
                {
                    /**
                     * Decoding UCS codepoint to UTF-8.
                     * \see https://stackoverflow.com/questions/6240055/manually-converting-unicode-codepoints-into-utf-8-and-utf-16
                     */
                    int u = std::stoi(substr(getOffset() + 1, 4), nullptr, 16);
                    if (u < 0x80) { // one byte binary 0xxxxxxx
                        result += static_cast<char>(u);
                    }
                    else if (u < 0x800) { // two byte binary 110xxxxx 10xxxxxx
                        result += static_cast<char>(0xC0 + ((u >> 6) & 0x1F));
                        result += static_cast<char>(0x80 + (u & 0x3F));
                    }
                    else { // Three byte binary 1110xxxx 10xxxxxx 10xxxxxx
                        result += static_cast<char>(0xE0 + ((u >> 12) & 0x0F));
                        result += static_cast<char>(0x80 + ((u >> 6) & 0x3F));
                        result += static_cast<char>(0x80 + (u & 0x3F));
                    }
                    mIt += 4;
                    break;
                }

                case '"':
                case '\\':
                case '/':
                    result += *mIt;
                    break;
                case 'b':
                    result += '\b';
                    break;
                case 'f':
                    result += '\f';
                    break;
                case 'n':
                    result += '\n';
                    break;
                case 'r':
                    result += '\r';
                    break;
                case 't':
                    result += '\t';
                    break;

                default:
                    THROW_WITH_BACKTRACE1(EJsonFormatError, "String contains illegal escape character. " + debug(false, true));
                    break;
            }
        }
        else {
            result += *mIt;
        }
        mIt++;
    }
    mIt = mEnd + 1;

    pop();
    JLOG("getString exit (" << result << "): " << debug());
    return result;
}

DynamicData JsonDecoder::getObject()
{
    JLOG("getObject: " << debug(false, true));
    findSubString('{', '}');

    bool element_required = false;

    skipWhiteSpace();

    DynamicData result;

    while (mIt != mEnd) {
        auto name = getString();
        skipWhiteSpace();
//        JLOG("Object Name: " << name << ", next: " << *mIt);
        if (*mIt != ':') {
            THROW_WITH_BACKTRACE1(EJsonParseError, "Object key/value delimiter not found." + debug(false, true));
        }
        mIt++;
        result.Add(name, Decode());
        skipWhiteSpace();
        element_required = false;
        if (mIt != mEnd && *mIt == ',') {
            element_required = true;
            mIt++;
        }
    }
    if (element_required) {
        THROW_WITH_BACKTRACE1(EJsonParseError, "Excessive key/value delimiter found after " + result.GetItems().back().GetName() + ":" + result.GetItems().back().AsString());
    }
    JLOG("Skip next char: " << *mIt);
    mIt++;

    pop();

    JLOG("getObject exit (" << result << "): " << debug());
    return result;
}

DynamicData JsonDecoder::getArray()
{
    JLOG("getArray: " << debug(false, true));
    findSubString('[', ']');

    bool element_required = false;

    skipWhiteSpace();

    DynamicData result;

    while (mIt != mEnd) {
        result.Add(Decode());
        skipWhiteSpace();
//        JLOG("getArray: " << debug());
        element_required = false;
        if (mIt != mEnd && *mIt == ',') {
            element_required = true;
            mIt++;
        }
    }
    if (element_required) {
        THROW_WITH_BACKTRACE1(EJsonParseError, std::string("Excessive array delimiter found after ") +  std::string(result.GetCount() ? result[result.GetCount()-1].AsString() : ""));
    }
    mIt++;
    pop();
    JLOG("getArray exit (" << result << "): " << debug());
    return result;
}

/*
 * Parse the JSON content and extract it as a number,
 * stored in one of three supported native types.
 *
 * Exceptions are thrown if content has illegal number formatting.
 */
DynamicData JsonDecoder::getNumber()
{
    JLOG("getNumber: " << debug(false, true));
    bool is_float = false;
    bool is_negative = false;
    int i = 0;
    std::string result;

    skipWhiteSpace();

    while (mIt != mEnd) {
        char c = *mIt;
        int skip = 0;
        JLOG("c: " << c << ", i: " << i);
        switch (i) {
            case 0:
                if (c == '-') {
                    is_negative = true;
                    i = 1;
                }
                else {
                    skip = 1;
                }
                break;

            case 1:
                if (c == '0') {
                    i = 3;
                }
                else if (c >= '0' && c <= '9') {
                    i = 2;
                }
                else {
                    THROW_WITH_BACKTRACE1(EJsonNumberError, "First character is not a sign or numeric.");
                }
                break;

            case 2:
                if (!(c >= '0' && c <= '9')) {
                    skip = 3;
                }
                break;

            case 3:
                if (c == '.') {
                    is_float = true;
                    i = 4;
                }
                else {
                    skip = 6;
                }
                break;

            case 4:
                if (!(c >= '0' && c <= '9')) {
                    THROW_WITH_BACKTRACE1(EJsonNumberError, "Floating point decimal digit is not numeric.");
                }
                else {
                    i = 5;
                }
                break;

            case 5:
                if (!(c >= '0' && c <= '9')) {
                    skip = 6;
                }
                break;

            case 6:
                if (c == 'e' || c == 'E') {
                    is_float = true;
                    i = 7;
                }
                else {
                    skip = 10;
                }
                break;

            case 7:
                if (c == '+' || c == '-') {
                    i = 8;
                }
                else {
                    skip = 8;
                }
                break;

            case 8:
                if (!(c >= '0' && c <= '9')) {
                    THROW_WITH_BACKTRACE1(EJsonNumberError, "Floating point exponent is not numeric.");
                }
                else {
                    i = 9;
                }
                break;

            case 9:
                if (!(c >= '0' && c <= '9')) {
                    skip = 10;
                }
                break;

            case 10: // delimiter
                skipWhiteSpace();
                if (c == ',' || mIt == mEnd) {
                    skip = 100;
                }
                else {
                    THROW_WITH_BACKTRACE1(EJsonNumberError, std::string("Numeric value has non numeric ending: '") + c + "'");
                }
                break;

            default:
                break;
        }
        if (i < 100) {
            if (skip > 0) {
                i = skip;
            }
            else {
                mIt++;
                result += c;
            }
        }
        else {
            break;
        }
    }

    if (is_float) {
        return DynamicData(rsp::utils::StrUtils::ToDouble(result));
    }
    else if (is_negative) {
        return DynamicData(static_cast<std::int64_t>(std::strtoll(result.c_str(), nullptr, 10)));
    }
    else {
        return DynamicData(static_cast<std::uint64_t>(std::strtoull(result.c_str(), nullptr, 10)));
    }
}

DynamicData JsonDecoder::Decode()
{
    JLOG("Decode: " << debug(false, true));
    DynamicData result;

    skipWhiteSpace();

    if (mIt != mEnd) {
        switch (*mIt) {
            case '{':
                JLOG("Object detected: " << getOffset() << ", " << getLength() << "; " << substr(getOffset(), getLength()));
                result = getObject();
                JLOG("Result Object: " << result << ", Count: " << result.GetCount());
                break;

            case '[':
                JLOG("Array detected: " << substr(getOffset(), getLength()));
                result = getArray();
                JLOG("Result Array: " << result << ", Count: " << result.GetCount());
                break;

            case '"':
                JLOG("String detected: " << substr(getOffset(), getLength()));
                result = getString();
                JLOG("Result String: " << result << ", Length: " << result.AsString().size());
                break;

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '-':
                JLOG("Number detected: " << substr(getOffset(), getLength()));
                result = getNumber();
                JLOG("Result Number: " << result);
                break;

            case 't':
                JLOG("true detected: " << substr(getOffset(), getLength()));
                if (substr(getOffset(), 4) == "true") {
                    mIt += 4;
                    result = true;
                }
                else {
                    THROW_WITH_BACKTRACE1(EJsonParseError, "True is not formatted correctly. " + debug(false, true));
                }
                break;

            case 'f':
                JLOG("false detected: " << substr(getOffset(), getLength()));
                if (substr(getOffset(), 5) == "false") {
                    mIt += 5;
                    result = false;
                }
                else {
                    THROW_WITH_BACKTRACE1(EJsonParseError, "False is not formatted correctly. " + debug(false, true));
                }
                break;

            case 'n':
                JLOG("null detected: " << substr(getOffset(), getLength()));
                if (substr(getOffset(), 4) == "null") {
                    mIt += 4;
//                    result;
                }
                else {
                    THROW_WITH_BACKTRACE1(EJsonParseError, "Null is not formatted correctly. " + debug(false, true));
                }
                break;

            default:
                THROW_WITH_BACKTRACE1(EJsonParseError, "Illegal start character: " + debug(false, true));
                break;
        }
        skipWhiteSpace();
    }

    JLOG("Decode exit (" << result << "): " << debug());
    return result;
}

std::string JsonDecoder::debug(bool aIncludeText, bool aIncludeSubstr)
{
    std::stringstream ss;
    if (aIncludeText) {
        ss << *this << "; ";
    }
    ss << "Iterators: " << getOffset() << "(" << *mIt << "), " << getLength() << "(" << *mEnd << ");\n";
    if (aIncludeSubstr) {
        ss << " Substr: " << substr(getOffset(), getLength()) << ";";
    }
    return ss.str();
}
