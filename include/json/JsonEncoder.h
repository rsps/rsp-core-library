/*!
 * \copyright   Copyright (c) 2023 RSP Systems A/S. All rights reserved.
 * \author      steffen
 */

#ifndef INCLUDE_JSON_JSONENCODER_H_
#define INCLUDE_JSON_JSONENCODER_H_

#include <string>
#include <sstream>
#include <utils/DynamicData.h>
#include "JsonExceptions.h"

namespace rsp::json {

class JsonEncoder
{
public:
    static std::string Encode(const rsp::utils::DynamicData &arData, bool aPrettyPrint = false, bool aForceToUCS2 = false, unsigned int aArrayLineLength = 0);

protected:
    class PrintFormat {
    public:
        unsigned int indent = 0;
        unsigned int arll = 0;
        std::string nl{};
        std::string sp{};
    };

    const PrintFormat& mrPf;
    std::stringstream mResult{};
    bool mForceToUCS2 = false;

    JsonEncoder(const PrintFormat &arPf, bool aForceToUCS2);

    std::string getResult() const { return mResult.str(); }

    void stringToStringStream(const rsp::utils::DynamicData &arData, unsigned int aLevel);
    void arrayToStringStream(const rsp::utils::DynamicData &arData, unsigned int aLevel);
    void objectToStringStream(const rsp::utils::DynamicData &arData, unsigned int aLevel);
    void toStringStream(const rsp::utils::DynamicData &arData, unsigned int aLevel);
};

} /* namespace rsp::json */

#endif /* INCLUDE_JSON_JSONENCODER_H_ */
