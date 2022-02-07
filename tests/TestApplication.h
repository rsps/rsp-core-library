/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <logging/Logger.h>

#ifndef TESTS_TESTAPPLICATION_H_
#define TESTS_TESTAPPLICATION_H_

#include <application/CliApplication.h>

class TestApplication: public rsp::application::CliApplication
{
public:
    TestApplication(rsp::application::CommandLine &aCmd);
    virtual ~TestApplication();

protected:
    void execute() override;
    void handleOptions() override;
    void showHelp() override;

private:
    rsp::logging::LoggerInterface::Handle_t mLogHandle = 0;
};

#endif /* TESTS_TESTAPPLICATION_H_ */
