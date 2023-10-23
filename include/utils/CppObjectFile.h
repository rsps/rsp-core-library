/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_UTILS_CPP_OBJECT_FILE_H
#define RSP_CORE_LIB_UTILS_CPP_OBJECT_FILE_H

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <string>

namespace rsp::utils {

class CppObjectFile
{
public:
    explicit CppObjectFile(const std::filesystem::path &arFileName);

    std::string Name() { return mVariableName; }

    template<typename T>
    CppObjectFile& operator<<(const T& arValue)
    {
        mFile << arValue;
        return *this;
    }

    CppObjectFile& operator<<( std::ostream&(*apFunc)(std::ostream&) )
    {
        mFile << apFunc;
        return *this;
    }

    CppObjectFile& Hex(const std::uint8_t *apData, size_t aSize, size_t aIndent = 4);
protected:
    std::fstream mFile;
    std::string mVariableName;
};

} /* namespace rsp::utils */

#endif // RSP_CORE_LIB_UTILS_CPP_OBJECT_FILE_H
