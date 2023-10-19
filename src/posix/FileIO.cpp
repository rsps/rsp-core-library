/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <exceptions/ExceptionHelper.h>
#include <iostream>

#include <fcntl.h>
#include <poll.h>
#include <posix/FileIO.h>
#include <unistd.h>


namespace rsp::posix
{

FileIO::FileIO()
    : mHandle(-1)
{
}

FileIO::FileIO(const std::string &arFileName, std::ios_base::openmode aMode, int aPermissions)
    : mHandle(-1)
{
    Open(arFileName, aMode, aPermissions);
}

FileIO::~FileIO()
{
    Close();
}

void FileIO::Open(const std::string &arFileName, std::ios_base::openmode aMode, int aPermissions)
{
    int flags = 0;
    if (aMode & std::ios_base::ate) {
        flags |= O_APPEND;
    }
    if ((aMode & std::ios_base::in) && (aMode & std::ios_base::out)) {
        flags |= O_RDWR;
        if (aPermissions) {
            flags |= O_CREAT;
        }
    }
    else if (aMode & std::ios_base::in) {
        flags |= O_RDONLY;
    }
    else if (aMode & std::ios_base::out) {
        flags |= O_WRONLY;
        if (aPermissions) {
            flags |= O_CREAT;
        }
    }
    if (aMode & std::ios_base::trunc) {
        flags |= O_TRUNC;
    }
    if (aMode & cNonBlock) {
        flags |= O_NONBLOCK;
    }

    mFileName = arFileName;

    Close();
    int ret = open(mFileName.c_str(), flags, aPermissions);
    if (ret < 0) {
        THROW_SYSTEM("Error opening file " + mFileName);
    }

    mHandle = ret;
}

void FileIO::Close()
{
    if (mHandle >= 0) {
        close(mHandle);
        mHandle = -1;
    }
}

size_t FileIO::Seek(size_t aOffset, std::ios_base::seekdir aSeekDir)
{
    int base;
    switch (aSeekDir) {
        default:
        case std::ios_base::beg:
            base = SEEK_SET;
            break;
        case std::ios_base::cur:
            base = SEEK_CUR;
            break;
        case std::ios_base::end:
            base = SEEK_END;
            break;
    }
    off_t ret = lseek(mHandle, static_cast<off_t>(aOffset), base);
    if (ret < 0) {
        THROW_SYSTEM("Error moving cursor in file " + mFileName);
    }

    return static_cast<size_t>(ret);
}

size_t FileIO::Read(void *apBuffer, size_t aNumberOfBytesToRead)
{
    off_t ret = read(mHandle, apBuffer, aNumberOfBytesToRead);
    if (ret < 0) {
        THROW_SYSTEM("Error reading from file " + mFileName);
    }

    return static_cast<size_t>(ret);
}

void FileIO::ExactRead(void *apBuffer, size_t aNumberOfBytesToRead)
{
    size_t len = 0;
    int retries = 3;

    while (len != aNumberOfBytesToRead) {
        if (retries-- == 0) {
            errno = ENODATA;
            THROW_SYSTEM("Error reading " + std::to_string(aNumberOfBytesToRead) + " bytes from file " + mFileName);
        }

        len += Read(&static_cast<uint8_t*>(apBuffer)[len], aNumberOfBytesToRead - len);
    }
}

size_t FileIO::Write(const void *apBuffer, size_t aNumberOfBytesToWrite)
{
    ssize_t ret = write(mHandle, apBuffer, aNumberOfBytesToWrite);
    if (ret < 0) {
        THROW_SYSTEM("Error writing to file " + mFileName);
    }

    return static_cast<size_t>(ret);
}

void FileIO::ExactWrite(const void *apBuffer, size_t aNumberOfBytesToWrite)
{
    size_t len = 0;
    int retries = 3;

    while (len != aNumberOfBytesToWrite) {
        if (retries-- == 0) {
            errno = ENOSPC;
            THROW_SYSTEM("Error reading from file " + mFileName);
        }

        len += Write(&static_cast<const uint8_t*>(apBuffer)[len], aNumberOfBytesToWrite - len);
    }
}

std::string FileIO::GetLine()
{
    std::stringstream ss;
    char c;

    while (Read(&c, 1) == 1) {
        if (c == '\n') {
            break;
        }
        ss << c;
    }

    return ss.str();
}

void FileIO::PutLine(const std::string &arData)
{
    size_t offset = 0;
    size_t remaining = arData.size();
    char *p = const_cast<char *>(arData.data());

    do {
        size_t count = Write(p + offset, remaining);
        remaining -= count;
        offset += count;
    } while (remaining > 0);
}

size_t FileIO::GetSize()
{
    size_t current = Seek(0, std::ios_base::cur); // Get current position
    size_t result = Seek(0, std::ios_base::end);  // Get last position
    Seek(current);                                     // Restore position

    return result;
}

void FileIO::SetSize(size_t aSize)
{
    int ret = ftruncate(mHandle, static_cast<long int>(aSize));
    if (ret < 0) {
        THROW_SYSTEM("Error resizing file " + mFileName);
    }
}

bool FileIO::WaitForDataReady(int aTimeoutMs) const
{
    struct pollfd fd{};
    int ret;
    bool result = false;

    fd.fd = mHandle;
    fd.events = POLLIN;

    ret = poll(&fd, 1, aTimeoutMs);

    if (ret > 0) {
        if (fd.revents & POLLIN) {
            result = true;
        }
    }

    return result;
}

std::string FileIO::GetContents()
{
    Seek(0, std::ios_base::beg);

    std::stringstream ss;
    char buf[1024];
    size_t read_len;

    while ((read_len = Read(buf, 1024)) > 0) {
        ss.write(buf, static_cast<std::streamsize>(read_len));
    }

    return ss.str();
}

void FileIO::PutContents(const std::string &arData)
{
    Seek(0, std::ios_base::beg);
    PutLine(arData);
}

} // namespace rsp::posix
