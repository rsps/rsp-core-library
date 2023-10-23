/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <functional>
#include <logging/Logger.h>
#include <application/ApplicationBase.h>

#ifndef TESTS_TEST_APPLICATION_H_
#define TESTS_TEST_APPLICATION_H_

/**
 * \class TestApplication
 * \brief Application customized for use during unit and integration testing.
 */
class TestApplication: public rsp::application::ApplicationBase
{
public:
    explicit TestApplication(int argc = 0, const char **argv = nullptr);
    ~TestApplication() override;

    typedef std::function<bool(TestApplication&)> Callback_t;

    /**
     * Set this callback to execute specialized code during the Run loop.
     *
     * \param aCb
     * \return
     */
    TestApplication& SetCallback(Callback_t aCb) { mCallback = std::move(aCb); return *this; }

protected:
    std::function<bool(TestApplication&)> mCallback{};

    void execute() override;
    void handleOptions() override;
    void showHelp() override;

private:
    rsp::logging::LoggerInterface::Handle_t mLogHandle = 0;
};

#endif /* TESTS_TEST_APPLICATION_H_ */
