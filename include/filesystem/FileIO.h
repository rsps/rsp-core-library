/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_POSIX_FILEIO_H_
#define SRC_POSIX_FILEIO_H_

#include <chrono>
#include <string>
#include <sstream>
#include <fstream>

namespace rsp::filesystem
{

/**
 * \class FileIO
 * \brief Class that implements common file operations as
 *        defined in the POSIX specification.
 *
 */
class FileIO
{
public:
//    const std::ios_base::openmode cNonBlock

    FileIO();
    FileIO(const std::string &aFileName, std::ios_base::openmode aMode, int aPermissions = 0);
    virtual ~FileIO();

    void Open(const std::string &aFileName, std::ios_base::openmode aMode, int aPermissions = 0);
    void Close();
    bool IsOpen() { return (mHandle >= 0); }

    std::size_t Seek(std::size_t aOffset, std::ios_base::seekdir aSeekDir = std::ios_base::beg);

    std::size_t Read(void *aBuffer, std::size_t aNumberOfBytesToRead);
    std::size_t Write(const void *aBuffer, std::size_t aNumberOfBytesToWrite);

    std::string GetLine(); // Read contents until newline or eof into string
    void PutLine(const std::string &aData); // Write a string to the file

    std::string GetContents(); // Read all data until eof into string
    void PutContents(const std::string &aData); // Write the string to file

    int GetHandle() { return mHandle; }
    std::size_t GetSize();

    bool WaitForDataReady(int aTimeoutms);

protected:
    std::string mFileName;
    int mHandle;
};

} /* namespace posix */

#endif /* SRC_POSIX_FILEIO_H_ */
