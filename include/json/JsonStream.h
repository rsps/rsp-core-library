/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_JSON_JSON_STREAM_H
#define RSP_CORE_LIB_JSON_JSON_STREAM_H

#include <string>
#include <sstream>

namespace rsp::json {

#define QUOTED(a) "\"" #a "\""

struct Comma {
    char mValue = ',';
    explicit Comma(char aValue = ',') : mValue(aValue) {}
};
struct Key
{
    const char *mpValue;
    explicit Key(const std::string &arValue) : mpValue(arValue.c_str()) {}
    explicit Key(const char *apValue) : mpValue(apValue) {}
};
struct OBegin {};
struct OEnd {};
struct ABegin {};
struct AEnd {};
struct Null {};

/**
 * \class JsonStream
 * \brief std::ostringstream derivative to help with formatting data in human readable JSON format
 */
class JsonStream : public std::ostringstream {
public:
    /**
     * \fn JsonStream(bool aPrettyPrint, int aLevel)
     * \brief Constructs a stream object
     *
     * \param aPrettyPrint Set to format output in human readable format
     * \param aLevel Starting indentation level.
     */
    explicit JsonStream(bool aPrettyPrint = false, unsigned aLevel = 0);

    [[nodiscard]] size_t Getsize() const;

    bool mPrettyPrint;
    unsigned mRootLevel;
    std::string indentation{};
    std::string space{};
    std::string newLine{};
};

JsonStream& operator<<(JsonStream& o, const Comma &arComma);
JsonStream& operator<<(JsonStream& o, const Key &arKey);
JsonStream& operator<<(JsonStream& o, const OBegin &arObjectBegin);
JsonStream& operator<<(JsonStream& o, const OEnd &arObjectEnd);
JsonStream& operator<<(JsonStream& o, const ABegin &arArrayBegin);
JsonStream& operator<<(JsonStream& o, const AEnd &arArrayEnd);
JsonStream& operator<<(JsonStream& o, const std::string &arStr);
JsonStream& operator<<(JsonStream& o, const char *apStr);
JsonStream& operator<<(JsonStream& o, const Null &arNull);

template <class T>
JsonStream& operator<<(JsonStream& o, T v) {
    static_cast<std::ostringstream&>(o) << v;
    return o;
}

} // namespace rsp::json

#endif // RSP_CORE_LIB_JSON_JSON_STREAM_H
