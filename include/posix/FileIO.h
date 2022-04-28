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
#include <fstream>
#include <sstream>
#include <string>

namespace rsp::posix
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
    static constexpr std::ios_base::openmode cNonBlock = static_cast<std::ios_base::openmode>(static_cast<int>(std::_Ios_Openmode::_S_ios_openmode_end + 1));

    /**
     * Construct a FileIO object, open/create file if given as argument.
     */
    FileIO();
    FileIO(const std::string &arFileName, std::ios_base::openmode aMode, int aPermissions = 0);
    virtual ~FileIO();

    /**
     * Opens the given file or create it if aPersmissions are given.
     * Open modes can be in, out and inout.
     *
     * \param aFileName
     * \param aMode
     * \param aPermissions
     */
    void Open(const std::string &arFileName, std::ios_base::openmode aMode, int aPermissions = 0);

    /**
     * Close the current file if it is open.
     */
    void Close();

    /**
     * Check if file is open.
     *
     * \return True if file is open.
     */
    bool IsOpen()
    {
        return (mHandle >= 0);
    }

    /**
     * Move the cursor to the given position.
     *
     * \param aOffset
     * \param aSeekDir
     * \return
     */
    std::size_t Seek(std::size_t aOffset, std::ios_base::seekdir aSeekDir = std::ios_base::beg);

    /**
     * Read an amount of bytes into the buffer.
     *
     * \param aBuffer
     * \param aNumberOfBytesToRead
     * \return Number of bytes read. Can be zero if no more bytes are currently available
     */
    std::size_t Read(void *apBuffer, std::size_t aNumberOfBytesToRead);

    /**
     * \brief Read exact amount of bytes into buffer
     * \param apBuffer
     * \param aNumberOfBytesToRead
     */
    void ExactRead(void *apBuffer, std::size_t aNumberOfBytesToRead);

    /**
     * Write an amount of bytes from the buffer to the file.
     *
     * \param aBuffer
     * \param aNumberOfBytesToWrite
     * \return Number of bytes written.
     */
    std::size_t Write(const void *apBuffer, std::size_t aNumberOfBytesToWrite);

    /**
     * \brief Write an exact amount of bytes from the buffer to the file.
     * \param apBuffer
     * \param aNumberOfBytesToWrite
     */
    void ExactWrite(const void *apBuffer, std::size_t aNumberOfBytesToWrite);

    /**
     * Get the rest of the current line.
     *
     * \return std::string with line contents
     */
    std::string GetLine(); // Read contents until newline or eof into string
    /**
     * Write given string to the file from the current cursor position.
     *
     * \param aData
     */
    void PutLine(const std::string &arData); // Write a string to the file

    /**
     * Read all file contents into a string.
     *
     * \return std::string with file contents.
     */
    std::string GetContents(); // Read all data until eof into string

    /**
     * Write given string to file, overwriting existing contents.
     * Remark: A larger file is not truncated to the string length.
     *
     * \param aData
     */
    void PutContents(const std::string &arData); // Write the string to file

    /**
     * Return the OS file handle.
     *
     * \return
     */
    int GetHandle()
    {
        return mHandle;
    }

    /**
     * Get the size of the current file.
     *
     * \return uint size of file.
     */
    std::size_t GetSize();

    /**
     * Set size of the file.
     * If longer file is truncated.
     * If shorter file is padded with binary zeros.
     *
     * \param aSize
     */
    void SetSize(std::size_t aSize);

    /**
     * Wait for data to become ready in the open file or timeout.
     * This is useful when waiting for at character device file to become ready.
     *
     * \param aTimeoutms
     * \return True if data in the file awaits reading.
     */
    bool WaitForDataReady(int aTimeoutms);

  protected:
    std::string mFileName{};
    int mHandle = -1;
};

} // namespace rsp::posix

#endif /* SRC_POSIX_FILEIO_H_ */
