/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_UTILS_JSON_JSONSTREAM_H_
#define INCLUDE_UTILS_JSON_JSONSTREAM_H_

#include <string>
#include <sstream>

namespace rsp::utils::json {

/**
 * \class JsonStream
 * \brief std::stringstream derivative to help with formatting data in human readable JSON format
 */
class JsonStream : public std::stringstream {
public:
    enum class Attributes { in0, in1, in2, in3, in4, in5, in6, in7, in8, in9, cnl, nl, sp };
    /**
     * \fn JsonStream(bool aPrettyPrint, int aLevel)
     * \brief Contructs a stream object
     *
     * \param aPrettyPrint Set to format output in human readable format
     * \param aLevel Starting indentation level.
     */
    JsonStream(bool aPrettyPrint, int aLevel);

protected:
    friend std::ostream& operator<< (std::ostream& os, const Attributes aAttrib);
    int mRootLevel;
    std::string mCommaNewLine{};
    std::string mNewLine{};
    std::string mSpace{};
};

std::ostream& operator<< (std::ostream& os, const JsonStream::Attributes aAttrib);


} // namespace rsp::utils::json

#endif /* INCLUDE_UTILS_JSON_JSONSTREAM_H_ */
