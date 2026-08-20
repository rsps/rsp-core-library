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

#include <sstream>
#include <string>
#include <string_view>
#include <rsp/utils/DateTime.h>
#include <rsp/utils/EnumReflection.h>
#include <rsp/utils/Variant.h>
#include <rsp/utils/StructElement.h>

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

struct Raw
{
    std::string_view mJson;
    explicit Raw(const std::string &arJson) : mJson(arJson) {}
    explicit Raw(std::string_view aJson) : mJson(aJson) {}
    explicit Raw(const char* apJson) : mJson(std::string_view(apJson)) {}
};

template <class T>
struct Value
{
    T& mrValue;
    explicit Value(T& arValue) : mrValue(arValue) {}
};

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

    /**
     * \brief Set the internal work buffer to use.
     * \param apBuffer
     * \param aSize
     * \return self
     */
    JsonStream& SetBuffer(char *apBuffer, size_t aSize);

    /**
     * \brief Get the current size, in bytes, of the stream
     * \return size_t
     */
    [[nodiscard]] size_t GetSize();

    /**
     * \brief Get the maximum size, in bytes, of the stream.
     * \return size_t
     */
    [[nodiscard]] size_t GetMaxSize() const;

    /**
     * \brief Get available space, in bytes, in the stream
     * \return
     */
    [[nodiscard]] size_t GetAvailable();

    static void StringToStream(const std::string &arString, std::ostream &o, bool aForceToUCS2 = false);
    static void StringToStream(std::string_view aString, std::ostream &o, bool aForceToUCS2 = false);
    static void StringToStream(const char *apString, size_t aSize, std::ostream &o, bool aForceToUCS2 = false);

    bool mPrettyPrint;
    unsigned mRootLevel;
    std::string indentation{};
    std::string space{};
    std::string newLine{};

protected:
    size_t mMaxSize = size_t(std::numeric_limits<std::streamsize>::max());
};

JsonStream& operator<<(JsonStream& o, const Comma &arComma);
JsonStream& operator<<(JsonStream& o, const Key &arKey);
JsonStream& operator<<(JsonStream& o, const OBegin &arObjectBegin);
JsonStream& operator<<(JsonStream& o, const OEnd &arObjectEnd);
JsonStream& operator<<(JsonStream& o, const ABegin &arArrayBegin);
JsonStream& operator<<(JsonStream& o, const AEnd &arArrayEnd);
JsonStream& operator<<(JsonStream& o, const std::string &arStr);
JsonStream& operator<<(JsonStream& o, const std::string_view &arStr);
JsonStream& operator<<(JsonStream& o, const char *apStr);
JsonStream& operator<<(JsonStream& o, const Null &arNull);
JsonStream& operator<<(JsonStream& o, const bool &arBool);
JsonStream& operator<<(JsonStream& o, const rsp::utils::Variant &arValue);
JsonStream& operator<<(JsonStream& o, const Raw &arJson);
JsonStream& operator<<(JsonStream& o, const rsp::utils::DateTime &arDt);

template <class T>
JsonStream& operator<<(JsonStream& o, const Value<T>& v) {
    if constexpr (requires { v.mrValue.empty(); }) {
        if (v.mrValue.empty()) {
            o << Null();
        }
        else {
            o << v.mrValue;
        }
    }
    else if constexpr (requires { v.mrValue.has_value(); }) {
        if (v.mrValue.has_value()) {
            o << v.mrValue.value();
        }
        else {
            o << Null();
        }
    }
    else {
        static_assert(false, "Value type not supported");
    }

    return o;
}

// Default enum streaming
template <class E> requires std::is_enum_v<E>
JsonStream& operator<< (JsonStream &o, E value) {
    return o << utils::EnumName(value);
}

template <class T> requires (!std::is_enum_v<T>)
JsonStream& operator<<(JsonStream& o, const T& v) {
    static_cast<std::ostringstream&>(o) << v;
    return o;
}

template <class T>
JsonStream& operator<< (JsonStream &o, utils::StructElement<T> const &t) {
    if (t.IsNull()) {
        o << Null();
    }
    else {
        if constexpr(std::is_floating_point<T>::value) {
            std::streamsize digits = t.GetPrecision();
            if (digits == -1) {
                digits = std::numeric_limits<double>::max_digits10;
            }
            o.imbue(std::locale::classic());
            if (digits >= 0) {
                o.precision(digits);
                o << std::fixed;
            }
        }
        o << t.Get();
    }
    return o;
}

} // namespace rsp::json

#endif // RSP_CORE_LIB_JSON_JSON_STREAM_H
