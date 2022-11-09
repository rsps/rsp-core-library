/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/limits.h>
#include <fts.h>
#include <glob.h>
#include <sys/sysmacros.h>
#include <posix/FileSystem.h>
#include <pthread.h>
#include <sys/resource.h>
#include <system_error>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <regex>
#include <utils/StrUtils.h>

#include <utils/ExceptionHelper.h>

using namespace rsp::utils;

namespace rsp::posix::FileSystem
{

bool DirectoryExists(const std::string aPath)
{
    struct stat info;

    int statRC = stat(aPath.c_str(), &info);
    if (statRC != 0) {
        if (errno == ENOENT) {
            return false;
        } // something along the path does not exist
        if (errno == ENOTDIR) {
            return false;
        } // something in path prefix is not a dir
        return false;
    }

    return (info.st_mode & S_IFDIR) ? true : false;
}

bool FileExists(const std::string aPath)
{
    struct stat info;

    int statRC = stat(aPath.c_str(), &info);
    if (statRC != 0) {
        if (errno == ENOENT) {
            return false;
        } // something along the path does not exist
        if (errno == ENOTDIR) {
            return false;
        } // something in path prefix is not a dir
        return false;
    }

    return (info.st_mode & (S_IFCHR | S_IFBLK | S_IFREG)) ? true : false;
}

void DeleteFile(const std::string &arFileName)
{
    if (!FileExists(arFileName)) {
        return;
    }
    if (std::remove(arFileName.c_str()) != 0) {
        THROW_SYSTEM("Could not delete " + arFileName);
    }
}

void RecursiveDeleteDir(const std::string aDir)
{
    FTS *ftsp = nullptr;
    FTSENT *curr;

    // Cast needed (in C) because fts_open() takes a "char * const *", instead
    // of a "const char * const *", which is only allowed in C++. fts_open()
    // does not modify the argument.
    char *files[] = { const_cast<char*>(aDir.c_str()), nullptr };

    // FTS_NOCHDIR  - Avoid changing cwd, which could cause unexpected behavior
    //                in multithreaded programs
    // FTS_PHYSICAL - Don't follow symlinks. Prevents deletion of files outside
    //                of the specified directory
    // FTS_XDEV     - Don't cross filesystem boundaries
    ftsp = fts_open(files, FTS_NOCHDIR | FTS_PHYSICAL | FTS_XDEV, nullptr);
    if (!ftsp) {
        THROW_SYSTEM("FileSystem - fts_open failed: " + aDir);
    }

    while ((curr = fts_read(ftsp))) {
        switch (curr->fts_info) {
            case FTS_NS:
            case FTS_DNR:
            case FTS_ERR:
                if (curr->fts_errno == ENOENT) {
                    break; // Ignore "No such file or directory", dir does not exist, so nothing to delete.
                }
                THROW_SYSTEM("FileSystem - fts_read error: " + std::string(curr->fts_accpath));
                break;

            case FTS_DC:
            case FTS_DOT:
            case FTS_NSOK:
                // Not reached unless FTS_LOGICAL, FTS_SEEDOT, or FTS_NOSTAT were
                // passed to fts_open()
                break;

            case FTS_D:
                // Do nothing. Need depth-first search, so directories are deleted
                // in FTS_DP
                break;

            case FTS_DP:
            case FTS_F:
            case FTS_SL:
            case FTS_SLNONE:
            case FTS_DEFAULT:
                if (remove(curr->fts_accpath) < 0) {
                    THROW_SYSTEM("FileSystem - remove error: " + std::string(curr->fts_accpath));
                }
                break;
            default:
                break;
        }
    }

    if (ftsp) {
        fts_close(ftsp);
    }
}

void MakeDirectory(const std::string aDir)
{
    std::string work = aDir;

    char *tmp = &work[0];
    char *p = nullptr;
    size_t len = work.length();

    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;
    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            if ((mkdir(tmp, 0755) != 0) && (errno != EEXIST)) {
                THROW_SYSTEM("DeviceScanner - Could not create directory: " + aDir);
            }
            *p = '/';
        }
    }

    if ((mkdir(tmp, 0755) != 0) && (errno != EEXIST)) {
        THROW_SYSTEM("FileSystem - Could not create directory: " + aDir);
    }
}

void MakeSymlink(const std::string aExisting, const std::string aSymlink)
{
    if ((symlink(aExisting.c_str(), aSymlink.c_str()) != 0) && (errno != EEXIST)) {
        THROW_SYSTEM("FileSystem - Could not create symlink: " + aSymlink);
    }
}

void Sleep(unsigned int aMilliSeconds)
{
    usleep(aMilliSeconds * 1000);
}

std::string GetCurrentWorkingDirectory()
{
    char buf[PATH_MAX];

    if (getcwd(buf, PATH_MAX) == nullptr) {
        THROW_SYSTEM("FileSystem - Could not get current working directory");
    }

    std::string result(buf);

    return result + "/";
}

void SetPermissions(const std::string aPath, int aPermissions)
{
    if (chmod(aPath.c_str(), static_cast<mode_t>(aPermissions)) != 0) {
        std::stringstream ss;
        ss << "FileSystem - Could not set file permissions on " << aPath << " to " << std::oct << aPermissions;
        THROW_SYSTEM(ss.str());
    }
}

std::string GetCurrentIpAddress()
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        THROW_SYSTEM("FileSystem - Could not create socket");
    }

    const char *kExternalIp = "1.1.1.1";
    uint16_t kDnsPort = 53;
    struct sockaddr_in serv;

    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(kExternalIp);
    serv.sin_port = htons(kDnsPort);

    int err = connect(sock, reinterpret_cast<const sockaddr*>(&serv), sizeof(serv));
    if (sock == -1) {
        THROW_SYSTEM("FileSystem - Could connect socket");
    }

    sockaddr_in name;
    socklen_t namelen = sizeof(name);
    err = getsockname(sock, reinterpret_cast<sockaddr*>(&name), &namelen);
    if (err == -1) {
        THROW_SYSTEM("FileSystem - Could not get socket name");
    }

    char buffer[17];
    const char *p = inet_ntop(AF_INET, &name.sin_addr, buffer, sizeof(buffer) - 1);
    buffer[16] = 0;

    close(sock);

    std::string result(p);
    return result;
}

int ExecuteCommand(const std::string &arCommand, std::string *apStdOut, std::string *apStdErrOut)
{
    static int cnt = 0;
    std::stringstream cmd;

    std::string tmp_name("/tmp/ExecuteCommand");
    tmp_name += std::to_string(cnt++);

    cmd << arCommand;

    if (apStdOut) {
        cmd << " > " << tmp_name << ".txt";
    }
    if (apStdErrOut) {
        cmd << " 2> " << tmp_name << ".err";
    }

    int result = std::system(cmd.str().c_str());

    if (apStdOut) {
        std::ifstream out(tmp_name + ".txt");
        std::string ret { std::istreambuf_iterator<char>(out), std::istreambuf_iterator<char>() };
        out.close(); // close the file so it can be deleted
        *apStdOut = ret;
    }
    DeleteFile(tmp_name + ".txt");

    if (apStdErrOut) {
        std::ifstream err(tmp_name + ".err");
        std::string ret { std::istreambuf_iterator<char>(err), std::istreambuf_iterator<char>() };
        err.close(); // close the file so it can be deleted
        *apStdErrOut = ret;
    }
    DeleteFile(tmp_name + ".err");

    return result;
}

std::string GetLastResumeId()
{
    std::string out;
    std::string err;

    int result = ExecuteCommand("journalctl -t systemd-sleep |tail -n1", &out, &err);

    if (result != 0) {
        THROW_WITH_BACKTRACE1(std::runtime_error, err);
    }

    return out;
}

void SetThreadPriority(std::thread &arThread, unsigned int aPriority)
{
    sched_param sch;
    int policy;
    struct rlimit limit;

    if (getrlimit(RLIMIT_RTPRIO, &limit)) {
        THROW_SYSTEM("FileSystem - Failed to get realtime limits");
    }
    if (limit.rlim_max <= aPriority) {
        // Quietly fail if we do not have capabilities to set the priority.
        // This is to avoid real time dependencies on test systems.
        return;
    }

    if (limit.rlim_cur < aPriority) {
        limit.rlim_cur = aPriority;
        limit.rlim_max = aPriority;
        if (setrlimit(RLIMIT_RTPRIO, &limit)) {
            THROW_SYSTEM("FileSystem - Failed to set realtime limits");
        }
    }

    if (pthread_getschedparam(arThread.native_handle(), &policy, &sch)) {
        THROW_SYSTEM("FileSystem - Failed to get thread priority");
    }
    sch.sched_priority = static_cast<int>(aPriority);
    if (pthread_setschedparam(arThread.native_handle(), SCHED_FIFO, &sch)) {
        THROW_SYSTEM("FileSystem - Failed to set thread priority");
    }
    if (pthread_getschedparam(arThread.native_handle(), &policy, &sch)) {
        THROW_SYSTEM("FileSystem - Failed to get thread priority");
    }

    if (sch.sched_priority != static_cast<int>(aPriority)) {
        std::stringstream ss;
        ss << "FileSystem - Could not set thread priority. Expected: " << aPriority << ", Current: " << sch.sched_priority;
        THROW_WITH_BACKTRACE1(std::runtime_error, ss.str());
    }
}

std::vector<std::filesystem::path> Glob(const std::filesystem::path &arPath, bool aRecursive, bool aDirOnly)
{
    std::vector<std::filesystem::path> result;

    std::filesystem::path path = arPath.parent_path();

    std::string filter = std::string("^") + arPath.string() + std::string("$");
    StrUtils::ReplaceAll(filter, "*", ".*");
    StrUtils::ReplaceAll(filter, "?", ".");

    std::regex self_regex(filter, std::regex_constants::basic);

    if (aRecursive) {
        for(auto const& dir_entry: std::filesystem::recursive_directory_iterator{path}) {
            if (std::regex_search(dir_entry.path().string(), self_regex)) {
                if (!aDirOnly || dir_entry.is_directory()) {
                    result.push_back(dir_entry.path());
                }
            }
        }
    }
    else {
        for(auto const& dir_entry: std::filesystem::directory_iterator{path}) {
            if (std::regex_search(dir_entry.path().string(), self_regex)) {
                if (!aDirOnly || dir_entry.is_directory()) {
                    result.push_back(dir_entry.path());
                }
            }
        }
    }

    return result;
}

std::filesystem::path GetCharacterDeviceByDriverName(const std::string &arDriverName, const std::filesystem::path &arPath)
{
    struct stat stat_buf;

    auto list = Glob(arPath, false, false);

    for (auto dir : list) {
        if (stat(dir.string().c_str(), &stat_buf) == -1) {
            THROW_SYSTEM("stat ERROR");
        }

        unsigned int major = major(stat_buf.st_rdev);
        unsigned int minor = minor(stat_buf.st_rdev);

        std::filesystem::path sys_path(StrUtils::Format("/sys/dev/char/%d:%d/device/driver", major, minor).c_str());

        if (!std::filesystem::directory_entry(sys_path).exists()) {
            continue;
        }

        std::filesystem::path real(std::filesystem::read_symlink(sys_path));

        if (arDriverName == real.stem()) {
            return dir;
        }
    }

    return std::string();
//        struct stat
//          {
//            __dev_t st_dev;     /* Device.  */
//            __ino_t st_ino;     /* File serial number.  */
//            __nlink_t st_nlink;     /* Link count.  */
//            __mode_t st_mode;       /* File mode.  */
//            __uid_t st_uid;     /* User ID of the file's owner. */
//            __gid_t st_gid;     /* Group ID of the file's group.*/
//            __dev_t st_rdev;        /* Device number, if device.  */
//            __off_t st_size;            /* Size of file, in bytes.  */
//            __blksize_t st_blksize; /* Optimal block size for I/O.  */
//            __blkcnt_t st_blocks;       /* Number 512-byte blocks allocated. */
//            __time_t st_atime;          /* Time of last access.  */
//            __syscall_ulong_t st_atimensec; /* Nscecs of last access.  */
//            __time_t st_mtime;          /* Time of last modification.  */
//            __syscall_ulong_t st_mtimensec; /* Nsecs of last modification.  */
//            __time_t st_ctime;          /* Time of last status change.  */
//            __syscall_ulong_t st_ctimensec; /* Nsecs of last status change.  */
//          };
//        Path = fullpath;
//        Size = stat_buf.st_size;
//        Uid = stat_buf.st_uid;
//        Gid = stat_buf.st_gid;
//        Mode = stat_buf.st_mode;
//        Major = major(stat_buf.st_rdev);
//        Minor = minor(stat_buf.st_rdev);
}

std::chrono::system_clock::time_point GetFileModifiedTime(const std::filesystem::path &arFileName)
{
    return std::chrono::file_clock::to_sys(std::filesystem::last_write_time(arFileName));
}

void SetFileModifiedTime(const std::filesystem::path &arFileName, std::chrono::system_clock:: time_point aTime)
{
    std::filesystem::last_write_time(arFileName, std::chrono::file_clock::from_sys(aTime));
}

} // namespace FileSystem

