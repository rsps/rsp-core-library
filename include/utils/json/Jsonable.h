/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_UTILS_JSON_JSONABLE_H_
#define INCLUDE_UTILS_JSON_JSONABLE_H_

#include <string>
#include <sstream>

namespace rsp::utils {

enum class JsonableStreamAttributes { in0, in1, in2, in3, in4, in5, in6, in7, in8, in9, cnl, nl, sp };

std::ostream& operator<< (std::ostream& os, const JsonableStreamAttributes aAttrib);

/**
 * \class Jsonable
 * \brief Interface with helpers for building json formatted stringstream
 */
class Jsonable
{
public:
    virtual ~Jsonable() {}

    /**
     * \fn abstract, std::string ToJson(bool aPrettyPrint = true, int aLevel = 0) const
     * \brief Interface to get object content as a JSON formatted string
     * \param aPrettyPrint Set for human readable format
     * \param aLevel Starting indentation level, only relevant if aPrettyPrint is set.
     * \return JSON formatted string
     */
    virtual std::string ToJson(bool aPrettyPrint = true, int aLevel = 0) const = 0;

    /**
     * \fn void FromJson(const std::string &arJson)
     * \brief Interface to populate object content with content from JSON formatted string
     * \param arJson JSON formatted string
     */
    virtual void FromJson(const std::string &arJson);

    /**
     * \class JsonStream
     * \brief Stringstream derivative to help with json formatting streams in human readable format
     */
    class JsonStream : public std::stringstream {
    public:
        /**
         * \fn JsonStream(bool aPrettyPrint, int aLevel)
         * \brief Contructs a stream object
         *
         * \param aPrettyPrint Set to format output in human readable format
         * \param aLevel Starting indentation level.
         */
        JsonStream(bool aPrettyPrint, int aLevel);

    protected:
        friend std::ostream& operator<< (std::ostream& os, const JsonableStreamAttributes aAttrib);
        int mRootLevel;
        std::string mCommaNewLine{};
        std::string mNewLine{};
        std::string mSpace{};
    };
};


} /* namespace rsp::utils */

#endif /* INCLUDE_UTILS_JSON_JSONABLE_H_ */
