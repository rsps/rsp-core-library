/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <utils/json/JsonString.h>
#include <utils/json/JsonExceptions.h>
#include <utils/json/JsonObject.h>
#include <utils/json/JsonArray.h>
#include <logging/Logger.h>

using namespace rsp::utils::json;

/**
 * Traverse the string to locate start and end of a substring.
 *
 * \param aToken1
 * \param aToken2
 */
void JsonString::findSubString(const char aToken1, const char aToken2)
{
    auto it = mIt;

    skipWhiteSpace();

    if (it == mEnd) { // String was only whitespace
        THROW_WITH_BACKTRACE(EJsonFormatError);
    }

    if (*it != aToken1) {
        THROW_WITH_BACKTRACE(EJsonFormatError);
    }
    it++;
    mIt = it;

    int indent = 0;

    while (it != mEnd) {
        if (*it == aToken2) {
            if (indent == 0) {
                push();
                mEnd = it;
                it++;
                DLOG("Stacked: " << *mIt << ", " << *mEnd);
                return;
            }
            else {
                it++;
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

    THROW_WITH_BACKTRACE(EJsonFormatError);
}

void JsonString::push()
{
    mStack.emplace_back(mEnd);
}

void JsonString::pop()
{
    mEnd = mStack.back();
    mStack.pop_back();
}

void JsonString::skipWhiteSpace()
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

std::string JsonString::getString()
{
    debug();

    findSubString('"', '"');

    std::string result;

    while (mIt != mEnd) {
        unsigned int c = static_cast<unsigned int>(*mIt);

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
                    THROW_WITH_BACKTRACE(EJsonParseError);
                    break;
            }
        }
        else {
            result += *mIt;
        }
        mIt++;
    }
    debug();

    pop();
    return result;
}

JsonValue* JsonString::getObject()
{
    findSubString('{', '}');

    auto result = new JsonObject();

    while (mIt != mEnd) {
        auto name = getString();
        skipWhiteSpace();
        if (*mIt != ':') {
            THROW_WITH_BACKTRACE(EJsonFormatError);
        }
        mIt++;
        result->Add(name, GetValue());
        skipWhiteSpace();
        if (mIt != mEnd && *mIt == ',') {
            mIt++;
        }
    }

    pop();
    return result;
}

JsonValue* JsonString::getArray()
{
    auto result = new JsonArray();

    findSubString('[', ']');

    while (mIt != mEnd) {
        result->Add(GetValue());
        skipWhiteSpace();
        if (mIt != mEnd && *mIt == ',') {
            mIt++;
        }
    }

    pop();
    return result;
}

JsonValue* JsonString::getNumber()
{
    bool is_float = false;
    bool is_negative = false;
    int i = 0;
    std::string result;

    skipWhiteSpace();

    while (mIt != mEnd) {
        char c = *mIt;
        int skip = 0;
        DLOG("c: " << c << ", i: " << i);
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
                    THROW_WITH_BACKTRACE(EJsonNumberError);
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
                    THROW_WITH_BACKTRACE(EJsonNumberError);
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
                    THROW_WITH_BACKTRACE(EJsonNumberError);
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
                if (c == ',' || mIt == mEnd) {
                    skip = 100;
                }
                else {
                    THROW_WITH_BACKTRACE(EJsonNumberError);
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
        return new JsonValue(std::strtod(result.c_str(), nullptr));
    }
    else if (is_negative) {
        return new JsonValue(static_cast<std::int64_t>(std::strtoll(result.c_str(), nullptr, 10)));
    }
    else {
        return new JsonValue(static_cast<std::int64_t>(std::strtoull(result.c_str(), nullptr, 10)));
    }
}

JsonValue* JsonString::GetValue()
{
    JsonValue* result = nullptr;

    skipWhiteSpace();

    if (mIt != mEnd) {
        switch (*mIt) {
            case '{':
                LOG("Object detected: " << substr(getOffset(), getLength()));
                result = getObject();
                LOG("Result Type: " << *result << ", Count: " << result->AsObject().GetCount());
                break;

            case '[':
                LOG("Array detected: " << substr(getOffset(), getLength()));
                result = getArray();
                LOG("Result Type: " << *result << ", Count: " << result->AsArray().GetCount());
                break;

            case '"':
                LOG("String detected: " << substr(getOffset(), getLength()));
                result = new JsonValue(getString());
                LOG("Result Type: " << *result << ", Length: " << result->AsString().size());
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
                DLOG("Number detected: " << substr(getOffset(), getLength()));
                result = getNumber();
                DLOG("Result Type: " << *result);
                break;

            case 't':
                DLOG("true detected: " << substr(getOffset(), getLength()));
                if (substr(getOffset(), 4) == "true") {
                    mIt += 4;
                    result = new JsonValue(true);
                }
                else {
                    THROW_WITH_BACKTRACE(EJsonParseError);
                }
                break;

            case 'f':
                DLOG("false detected: " << substr(getOffset(), getLength()));
                if (substr(getOffset(), 5) == "false") {
                    mIt += 5;
                    result = new JsonValue(false);
                }
                else {
                    THROW_WITH_BACKTRACE(EJsonParseError);
                }
                break;

            case 'n':
                DLOG("null detected: " << substr(getOffset(), getLength()));
                if (substr(getOffset(), 4) == "null") {
                    mIt += 4;
                    result = new JsonValue();
                }
                else {
                    THROW_WITH_BACKTRACE(EJsonParseError);
                }
                break;

            default:
                DLOG("Illegal start character: " << std::hex << static_cast<int>(*mIt) << ", input: " << substr(getOffset(), getLength()));
                THROW_WITH_BACKTRACE(EJsonParseError);
                break;
        }
        skipWhiteSpace();
    }

    return result;
}

void JsonString::debug()
{
    DLOG("JsonString: (" << *this << "), (" << *mIt << "), (" << *mEnd << ")");
}
