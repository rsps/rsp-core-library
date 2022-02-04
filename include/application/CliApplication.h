/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_APPLICATION_CLIAPPLICATION_H_
#define INCLUDE_APPLICATION_CLIAPPLICATION_H_

#include <application/ApplicationBase.h>
#include <application/CommandLine.h>

namespace rsp::application
{

class CliApplication: public ApplicationBase
{
public:
    CliApplication(CommandLine &aCmd);
    CliApplication(const CliApplication &other) = delete;
    CliApplication(CliApplication &&other) = delete;
    CliApplication& operator=(const CliApplication &other) = delete;
    CliApplication& operator=(CliApplication &&other) = delete;

    int Run() override;

protected:
    CommandLine &mCmd;

    virtual void handleOptions();
    virtual void showHelp();
    virtual void showVersion();
};

} /* namespace rsp::application */

#endif /* INCLUDE_APPLICATION_CLIAPPLICATION_H_ */
