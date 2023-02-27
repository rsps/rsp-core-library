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

/**
 * \class JsonEncoder
 * \brief DynamicData to JSON encoder
 */
class JsonEncoder : public rsp::utils::DynamicData::Encoder
{
public:
    class PrintFormat {
    public:
        unsigned int indent = 0;
        unsigned int arll = 0;
        std::string nl{};
        std::string sp{};
        bool ForceToUCS2 = false;
    };

    /**
     * \brief Constructor that takes a PrintFormat preset
     * \param arPf
     */
    JsonEncoder(const PrintFormat &arPf);

    /**
     * \brief Constructor with most used formatting arguments
     * \param aPrettyPrint Human readable output format
     * \param aArrayLineLength Allows for multiple elements per line if PrettyPrint is enabled.
     * \param aForceToUCS2 Force extended UTF8 characters into u+0000 JSON format.
     */
    JsonEncoder(bool aPrettyPrint = false, unsigned int aArrayLineLength = 0, bool aForceToUCS2 = false);

    /**
     * \brief Encodes all data in the DynamicData object into valid JSON.
     *
     * \param arData DynamicData object
     * \return string
     */
    std::string Encode(const rsp::utils::DynamicData &arData) override;

protected:
    PrintFormat mPf{};
    std::stringstream mResult{};

    void stringToStringStream(const rsp::utils::DynamicData &arData, unsigned int aLevel);
    void arrayToStringStream(const rsp::utils::DynamicData &arData, unsigned int aLevel);
    void objectToStringStream(const rsp::utils::DynamicData &arData, unsigned int aLevel);
    void toStringStream(const rsp::utils::DynamicData &arData, unsigned int aLevel);
};

} /* namespace rsp::json */

#endif /* INCLUDE_JSON_JSONENCODER_H_ */
