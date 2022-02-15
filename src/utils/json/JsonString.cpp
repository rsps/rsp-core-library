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
#include <logging/Logger.h>

using namespace rsp::utils::json;

void JsonString::Push(const char aToken1, const char aToken2)
{
    mIt, mEnd = mStack.back();

    SkipWhiteSpace();

    if (mIt == mEnd) {
        return;
    }

    if (*mIt != aToken1) {
        THROW_WITH_BACKTRACE(EJsonFormatError);
    }
    mIt++;

    mStack.emplace_back(mIt, mIt);
    int indent = 0;

    while (mIt != mEnd) {
        if (*mIt == aToken2) {
            mIt++;
            if (indent == 0) {
                mStack.back().second = mIt;
                DLOG("Stacked: " << *mStack.back().first << ", " << *mStack.back().second);
                return;
            }
            else {
                indent--;
            }
        }
        else if (*mIt == aToken1) {
            indent++;
        }
        else if (*mIt == '\\') {
            mIt++;
            if (*mIt == 'u') {
                mIt += 4; // Advance u + 4 hex digits
            }
        }
        mIt++;
    }

    THROW_WITH_BACKTRACE(EJsonFormatError);
}

void JsonString::Pop()
{
    mStack.pop_back();
}

void JsonString::SkipWhiteSpace()
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
                break;
        }
    }
}

std::string JsonString::GetString()
{
    Push('"', '"');

    mIt, mEnd = mStack.back();
    std::string result;

    unsigned int c = s.length();
    unsigned int i = 0;
    while (i < c) {
        if (s[i] == '\\') {
            i++;
            switch (s[i]) {
                case 'u':
                {
                    int u = std::stoi(s.substr(i+1, 4), nullptr, 16);
                    if (u > 255) {
                        throw EJsonUnicodeError();
                    }
                    result += static_cast<char>(u);
                    i += 4;
                    break;
                }

                case '"':
                case '\\':
                case '/':
                    result += s[i];
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
            result += s[i];
        }
        i++;
    }


    return result;
}

JsonValue* JsonString::GetValue()
{
    JsonValue* result;

    return result;
}
