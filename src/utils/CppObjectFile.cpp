/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <algorithm>
#include <utils/CppObjectFile.h>

namespace rsp::utils {

CppObjectFile::CppObjectFile(const std::filesystem::path &arFileName)
    : mFile(arFileName, std::ios_base::out | std::ios_base::trunc),
      mVariableName(arFileName.stem())
{
    mVariableName.erase(std::remove(mVariableName.begin(), mVariableName.end(), '\"' ), mVariableName.end());
}

CppObjectFile& CppObjectFile::Hex(const std::uint8_t *apData, size_t aSize, size_t aIndent)
{
    std::string delim = ", ";

    for (std::size_t i = 0 ; i < aSize ; i++) {
        if ((i % 16) == 0) {
            mFile << std::string(aIndent, ' ');
        }
        if (i == (aSize - 1)) {
            delim = "";
        }

        mFile << "0x" << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(apData[i]) << delim;

        if ((i % 16) == 15) {
            mFile << "\n";
        }
    }
    if ((aSize % 16) != 15) {
        mFile << "\n";
    }
    mFile << std::dec;

    return *this;
}

} /* namespace rsp::utils */
