/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_POSIX_FILESYSTEM_H_
#define SRC_POSIX_FILESYSTEM_H_

#include <string>
#include <vector>
#include <thread>
#include <filesystem>
#include <chrono>

/**
 * Namespace with common directory and OS operations.
 *
 * Most are using POSIX compliant system calls.
 */
namespace rsp::posix::FileSystem
{

/**
 * Check if a directory exists.
 *
 * \param aPath
 * \return bool
 */
bool DirectoryExists(const std::string aPath);

/**
 * Chekc if a file exists.
 *
 * \param aPath
 * \return bool
 */
bool FileExists(const std::string aPath);

/**
 * Delete a file
 *
 * \param arFileName
 */
void DeleteFile(const std::string &arFileName);

/**
 * Recursively delete a directory and all of its content.
 *
 * \param aDir
 */
void RecursiveDeleteDir(const std::string aDir);

/**
 * Recursively create directories.
 *
 * \param aDir
 */
void MakeDirectory(const std::string aDir);

/**
 * Make a symlink to an existing path.
 *
 * \param aExisting
 * \param aSymlink
 */
void MakeSymlink(const std::string aExisting, const std::string aSymlink);

/**
 * Sleep for the given number of milliseconds.
 *
 * \param aMilliSeconds
 */
void Sleep(unsigned int aMilliSeconds);

/**
 * Get the current directory.
 *
 * \return string
 */
std::string GetCurrentWorkingDirectory();

void SetPermissions(const std::string aPath, int aPermissions);

/**
 * Get the current IP address of the default outgoing interface.
 *
 * Create a UDP connection without sending anything.
 * Then read the local IP from connection.
 *
 * \return String with ip address
 */
std::string GetCurrentIpAddress();

/**
 * Execute a system command and return the exit code.
 *
 * It can collect stdout and stderr if output variables are set.
 *
 * \param arCommand String with command to execute.
 * \param apStdOut Pointer to string to collect stdout into.
 * \param apStdErr Pointer to string to collect error output
 * \return integer exit code
 */
int ExecuteCommand(const std::string &arCommand, std::string *apStdOut = nullptr, std::string *apStdErr = nullptr);

/**
 * Function to retrieve a unique id of the latest resume
 *
 * This simply returns the last resume line from the systemd journal.
 *
 * Non POSIX compliant.
 *
 * \return String
 */
std::string GetLastResumeId();

/**
 * Set the priority of a thread.
 *
 * Uses the underlying OS thread priority system.
 *
 * Note: Might be limited by OS.
 * \see https://stackoverflow.com/questions/10704983/operation-not-permitted-while-setting-new-priority-for-thread
 *
 * \param arThread Reference to std::thread instance
 * \param aPriority Thread priority 1-99, Higher priority preempts lower
 */
void SetThreadPriority(std::thread &arThread, unsigned int aPriority);

/**
 * \fn std::vector<std::filesystem::path> Glob(const std::filesystem::path&, bool=false, bool=false)
 * \brief Generates a list of directory items, as specified in the path argument.
 *        The common shell wildcards * and ? are supported in the path.
 *
 * \param arPath
 * \param aRecursive
 * \param aDirOnly
 * \return List with real paths of every item matching.
 */
std::vector<std::filesystem::path> Glob(const std::filesystem::path &arPath, bool aRecursive = false, bool aDirOnly = false);

/**
 * \fn std::string GetCharacterDeviceByDriverName(const std::string&, const std::filesystem::path&)
 * \brief Extension for Linux systems to locate a specific enumerated character device by the name of the driver behind it.
 *
 * \param arDriverName
 * \param arPath
 * \return Path to character device
 */
std::filesystem::path GetCharacterDeviceByDriverName(const std::string &arDriverName, const std::filesystem::path &arPath);

/**
 * \fn std::chrono::system_clock::time_point GetFileModifiedTime(const std::string&)
 * \brief The the time when a file was last modified.
 *
 * \param arFileName
 * \return time_point
 */
std::chrono::system_clock::time_point GetFileModifiedTime(const std::filesystem::path &arFileName);

/**
 * \fn void SetFileModifiedTime(const std::string&, std::chrono::system_clock::time_point)
 * \brief Set the time when a file was last modified.
 *
 * \param arFileName
 * \param time_point
 */
void SetFileModifiedTime(const std::filesystem::path &arFileName, std::chrono::system_clock::time_point aTime);

}

#endif /* SRC_POSIX_FILESYSTEM_H_ */
