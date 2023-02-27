/*!
 * \copyright   Copyright (c) 2023 RSP Systems A/S. All rights reserved.
 * \author      steffen
 */

#include <iomanip>
#include <json/JsonEncoder.h>
#include <logging/Logger.h>

using namespace rsp::utils;

//#define JLOG(a) DLOG(a)
#define JLOG(a)

namespace rsp::json {

JsonEncoder::JsonEncoder(const PrintFormat &arPf)
    : mPf(arPf)
{
}

JsonEncoder::JsonEncoder(bool aPrettyPrint, unsigned int aArrayLineLength, bool aForceToUCS2)
{
    if (aPrettyPrint) {
        mPf.indent = 4;
        mPf.arll = aArrayLineLength;
        mPf.nl = "\n";
        mPf.sp = " ";
        mPf.ForceToUCS2 = aForceToUCS2;
    }
}

std::string JsonEncoder::Encode(const rsp::utils::DynamicData &arData)
{
    toStringStream(arData, 0);
    return mResult.str();
}


void JsonEncoder::stringToStringStream(const DynamicData &arData, unsigned int aLevel)
{
    std::string s = arData.AsString();
    std::size_t i = 0;
    while (i < s.length()) {
        auto c = s[i];
        switch (c) {
            case '\"':
                s.replace(i, 1, "\\\"");
                break;
            case '\\':
                s.replace(i, 1, "\\\\");
                break;
            case '\b':
                s.replace(i, 1, "\\b");
                break;
            case '\f':
                s.replace(i, 1, "\\f");
                break;
            case '\n':
                s.replace(i, 1, "\\n");
                break;
            case '\r':
                s.replace(i, 1, "\\r");
                break;
            case '\t':
                s.replace(i, 1, "\\t");
                break;
            default:
                if (mPf.ForceToUCS2 && static_cast<uint8_t>(c) > 127) {
                    int v = 0;
                    char buf[12];
                    switch (static_cast<uint8_t>(c) & 0xE0) {
                        case 0xE0:
                            v =   ((static_cast<int>(s[i]) & 0x0F) << 12)
                                + ((static_cast<int>(s[i+1]) & 0x3F) << 6)
                                + (static_cast<int>(s[i+2]) & 0x3F);
                            sprintf(buf, "\\u%04x", v);
                            s.replace(i, 3, buf);
                            i += 4;
                            break;

                        case 0xC0:
                            v =   ((static_cast<int>(s[i]) & 0x1F) << 6)
                                + (static_cast<int>(s[i+1]) & 0x3F);
                            sprintf(buf, "\\u%04x", v);
                            s.replace(i, 2, buf);
                            i += 4;
                            break;

                        default:
                            THROW_WITH_BACKTRACE1(EJsonParseError, "DynamicData of type string has illegal JSON character: " + c);
                            break;
                    }
                }
                break;
        }
        i++;
    }
    mResult << "\"" << s << "\"";
}

void JsonEncoder::arrayToStringStream(const DynamicData &arData, unsigned int aLevel)
{
    std::string in(static_cast<std::string::size_type>(mPf.indent) * (aLevel + 1), ' ');
    std::string c = ",";
    int min_witdh = 0;

    mResult << "[" << mPf.nl;

    auto& items = arData.GetItems();
    auto rest = items.size();
    mResult << in;
    unsigned int item_count = 0;

    if (mPf.arll && mPf.indent && rest) {
        switch (items[0].GetType()) {
            case DynamicData::Types::Int:
            case DynamicData::Types::Uint32:
                min_witdh = 10;
                break;

            case DynamicData::Types::Int64:
            case DynamicData::Types::Uint64:
                min_witdh = 20;
                break;

            case DynamicData::Types::Uint16:
                min_witdh = 5;
                break;

            default:
                min_witdh = 7;
                break;
        }
    }

    for (const DynamicData &el : items) {
        if (min_witdh) {
            mResult << std::setw(min_witdh) << std::setfill(' ');
        }
        toStringStream(el, aLevel + 1);
        if (--rest == 0) {
            c = "";
            in.clear();
        }
        mResult << c;
        if ((rest == 0) || (++item_count >= mPf.arll)) {
            mResult << mPf.nl << in;
            item_count = 0;
        }
        else {
            mResult << " ";
        }
    }
    std::string in1(static_cast<std::string::size_type>(mPf.indent) * aLevel, ' ');
    mResult << in1 << "]";
}

void JsonEncoder::objectToStringStream(const DynamicData &arData, unsigned int aLevel)
{
    std::string in(static_cast<std::string::size_type>(mPf.indent) * (aLevel+1), ' ');
    std::string c = ",";

    mResult << "{" << mPf.nl;

    auto& items = arData.GetItems();

    JLOG("Members:");

    auto rest = items.size();
    for (const DynamicData &value : items) {
        JLOG("  " << value.mName << ": " << value.AsString());
        mResult << in << "\"" << value.GetName() << "\":" << mPf.sp;

        toStringStream(value, aLevel+1);
        if (--rest == 0) {
            c = "";
        }
        mResult << c << mPf.nl;
    }
    std::string in1(static_cast<std::string::size_type>(mPf.indent) * aLevel, ' ');
    mResult << in1 << "}";
}

void JsonEncoder::toStringStream(const DynamicData &arData, unsigned int aLevel)
{
    switch(arData.GetType()) {
        case DynamicData::Types::String:
            stringToStringStream(arData, aLevel);
            break;

        case DynamicData::Types::Array:
            arrayToStringStream(arData, aLevel);
            break;

        case DynamicData::Types::Object:
            objectToStringStream(arData, aLevel);
            break;

        default:
            mResult << arData.AsString();
        break;
    }
}

} /* namespace rsp::json */
