/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_UTILS_FILESYSTEM_H_
#define SRC_UTILS_FILESYSTEM_H_

#include <string>
#include <thread>

namespace rsp::FileSystem {

bool DirectoryExists(const std::string aPath);

bool FileExists(const std::string aPath);

void DeleteFile(const std::string &arFileName);

void RecursiveDeleteDir(const std::string aDir);

/* *
 * Recursively create directories.
 *
 * \param aDir
 */
void MakeDirectory(const std::string aDir);

void MakeSymlink(const std::string aExisting, const std::string aSymlink);

void Sleep(unsigned int aMilliSeconds);

std::string GetCurrentWorkingDirectory();

void SetPermissions(const std::string aPath, int aPermissions);

/**
 * Create a UDP connection without sending anything.
 * Then read the local IP from connection.
 *
 * \return String with ip address
 */
std::string GetCurrentIpAddress();

/**
 * Execute a system command and return the exit code.
 * It can collect stdout and stderr if output variables are set.
 *
 * \param arCommand String with command to execute.
 * \param apStdOut Pointer to string to collect stdout into.
 * \param apStdErr Pointer to string to collect error output
 * \return integer exit code
 */
int ExecuteCommand(const std::string &arCommand, std::string *apStdOut = nullptr, std::string *apStdErrOut = nullptr);

/**
 * Function to retrieve a unique id of the lastest resume
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
};

#endif /* SRC_UTILS_FILESYSTEM_H_ */
