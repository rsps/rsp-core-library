/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_JSON_JSONSTREAM_H_
#define INCLUDE_JSON_JSONSTREAM_H_

#include <string>
#include <sstream>

namespace rsp::json {

#define QUOTED(a) "\"" #a "\""

struct Indent
{
    unsigned mValue;
    Indent(unsigned aValue) : mValue(aValue) {}
};
struct Comma {};
struct Key
{
    const char *mpValue;
    Key(const std::string &arValue) : mpValue(arValue.c_str()) {}
    Key(const char *apValue) : mpValue(apValue) {}
};
struct OBegin {};
struct OEnd {};
struct ABegin {};
struct AEnd {};

/**
 * \class JsonStream
 * \brief std::ostringstream derivative to help with formatting data in human readable JSON format
 */
class JsonStream : public std::ostringstream {
public:
    /**
     * \fn JsonStream(bool aPrettyPrint, int aLevel)
     * \brief Contructs a stream object
     *
     * \param aPrettyPrint Set to format output in human readable format
     * \param aLevel Starting indentation level.
     */
    JsonStream(bool aPrettyPrint = false, unsigned aLevel = 0);

    bool mPrettyPrint;
    unsigned mRootLevel;
    std::string indentation{};
    std::string space{};
    std::string newLine{};
};

JsonStream& operator<<(JsonStream& o, const Indent &arIndent);
JsonStream& operator<<(JsonStream& o, const Comma &arComma);
JsonStream& operator<<(JsonStream& o, const Key &arKey);
JsonStream& operator<<(JsonStream& o, const OBegin &arObjectBegin);
JsonStream& operator<<(JsonStream& o, const OEnd &arObjectEnd);
JsonStream& operator<<(JsonStream& o, const ABegin &arArrayBegin);
JsonStream& operator<<(JsonStream& o, const AEnd &arArrayEnd);
JsonStream& operator<<(JsonStream& o, const std::string &arStr);
JsonStream& operator<<(JsonStream& o, const char *apStr);

template <class T>
JsonStream& operator<<(JsonStream& o, T v) {
    static_cast<std::ostringstream&>(o) << v;
    return o;
}

} // namespace rsp::json

#endif /* INCLUDE_JSON_JSONSTREAM_H_ */
