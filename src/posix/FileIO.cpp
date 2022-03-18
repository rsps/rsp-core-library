/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <iostream>

#include <fcntl.h>
#include <poll.h>
#include <posix/FileIO.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <utils/ExceptionHelper.h>

namespace rsp::posix
{

// const std::ios_base::openmode FileIO::cNonBlock = (std::ios_base::openmode)((int)std::_Ios_Openmode::_S_ios_openmode_end + 1);

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
    } else if (aMode & std::ios_base::in) {
        flags |= O_RDONLY;
    } else if (aMode & std::ios_base::out) {
        flags |= O_WRONLY;
        if (aPermissions) {
            flags |= O_CREAT;
        }
    }
    if (aMode & std::ios_base::trunc) {
        flags |= O_TRUNC;
    }
    //    if (aMode & cNonBlock) {
    //        flags |= O_NONBLOCK;
    //    }

    mFileName = arFileName;

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

bool FileIO::IsEOF()
{
    char buf[1];
    int ret = read(mHandle, buf, 1);
    if (ret != 0) {
        lseek(mHandle, lseek(mHandle, 0, SEEK_CUR) - 1, SEEK_SET);
        return false;
    }
    return true;
}

std::size_t FileIO::Seek(std::size_t aOffset, std::ios_base::seekdir aSeekdir)
{
    int base = 0;
    switch (aSeekdir) {
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
    int ret = lseek(mHandle, static_cast<off_t>(aOffset), base);
    if (ret < 0) {
        THROW_SYSTEM("Error moving cursor in file " + mFileName);
    }

    return static_cast<std::size_t>(ret);
}

std::size_t FileIO::Read(void *apBuffer, std::size_t aNumberOfBytesToRead)
{
    int ret = read(mHandle, apBuffer, aNumberOfBytesToRead);
    if (ret < 0) {
        THROW_SYSTEM("Error reading from file " + mFileName);
    }

    return static_cast<std::size_t>(ret);
}

std::size_t FileIO::Write(const void *apBuffer, std::size_t aNumberOfBytesToWrite)
{
    int ret = write(mHandle, apBuffer, aNumberOfBytesToWrite);
    if (ret < 0) {
        THROW_SYSTEM("Error writing to file " + mFileName);
    }

    return static_cast<std::size_t>(ret);
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
    std::size_t offset = 0;
    std::size_t remaining = arData.size();
    char *p = const_cast<char *>(arData.data());

    do {
        std::size_t count = Write(p + offset, remaining);
        remaining -= count;
        offset += count;
    } while (remaining > 0);
}

std::size_t FileIO::GetSize()
{
    std::size_t current = Seek(0, std::ios_base::cur); // Get current position
    std::size_t result = Seek(0, std::ios_base::end);  // Get last position
    Seek(current);                                     // Restore position

    return result;
}

void FileIO::SetSize(std::size_t aSize)
{
    int ret = ftruncate(mHandle, static_cast<long int>(aSize));
    if (ret < 0) {
        THROW_SYSTEM("Error resizing file " + mFileName);
    }
}

bool FileIO::WaitForDataReady(int aTimeoutms)
{
    struct pollfd fd;
    int ret;
    bool result = false;

    fd.fd = mHandle;
    fd.events = POLLIN;

    ret = poll(&fd, 1, aTimeoutms);

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
    std::size_t rlen;

    while ((rlen = Read(buf, 1024)) > 0) {
        ss.write(buf, static_cast<long int>(rlen));
    }

    return ss.str();
}

void FileIO::PutContents(const std::string &arData)
{
    Seek(0, std::ios_base::beg);
    PutLine(arData);
}

} // namespace rsp::posix
