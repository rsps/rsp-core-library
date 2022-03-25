/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_POSIX_TERMINALIO_H_
#define INCLUDE_POSIX_TERMINALIO_H_

#include <string>
#include <vector>
#ifdef __linux__
extern "C" {
    #include <termios.h>
}
#endif

namespace rsp::posix {

/**
 * \class TerminalIO
 * \brief Terminal simulator, uses the Posix General Terminal Interface on Linux.
 *
 * A class to read input from a virtual command line terminal.
 * The default settings would allow an application to read commands from a command line by calling the GetLine method.
 * It can be pre-filled with content, e.g. from a real command line, which then is return by the GetLine method.
 *
 * The virtual terminal has behavior similar to a Linux bash terminal, in regards to auto completion and multiple choice lookup.
 */
class TerminalIO
{
public:
    /**
     * \enum Used to map ANSI escape strings to enumerable type.
     */
    enum class EscapeCodes { Character, ArrowUp, ArrowDown, ArrowRight, ArrowLeft, Backspace, Delete, Home, End, Tabulator, Esc, Unknown };

    /**
     * Constructors an empty terminal.
     */
    TerminalIO();

    /**
     * Construct a terminal with preset commands.
     *
     * \param arPreset
     */
    TerminalIO(const std::string &arPreset);

    /**
     * Destructor
     */
    ~TerminalIO();

    /**
     * Set this if the input character should be shown directly on the terminal.
     *
     * \param aEcho, defaults off.
     * \return Reference to self for fluent call chain.
     */
    TerminalIO& SetEcho(bool aEcho = true);
    /**
     * Set if additional information should be output.
     *
     * \param aVerbose
     * \return Reference to self for fluent call chain.
     */
    TerminalIO& SetVerbose(bool aVerbose = true) { mVerbose = aVerbose; return *this; }

    /**
     * Read a single character from the input device.
     *
     * \param arEscCode
     * \return char
     */
    char GetChar(EscapeCodes &arEscCode);

    /**
     * Read a full line from the input device.
     *
     * \return string
     */
    std::string GetLine();

    /**
     * Check if presets have been added.
     *
     * \return bool
     */
    bool HasPreset() const { return (mPreset.length() > 0); }
    /**
     * Remove all presets.
     */
    void ClearPreset() { mPreset.clear(); }

    /**
     * Set the prompt prefix to be shown on the terminal.
     *
     * \param aPrompt
     * \return Reference to self for fluent call chain.
     */
    TerminalIO& SetPrompt(const std::string aPrompt) { mPrompt = aPrompt; return *this; }

    /**
     * Set a list of strings that is used for auto completion in case the tab character is pressed on the terminal.
     *
     * \param aPrompt
     * \return Reference to self for fluent call chain.
     */
    TerminalIO& SetAutocompletionDictionary(std::vector<std::string> const aDict) { mDictionary = aDict; return *this;}

    /**
     * Simply call this to wait for the "any key" to be pressed on the terminal.
     *
     * \return char
     */
    static char WaitForAnyKey();

protected:
    bool mVerbose = false;
    std::vector<std::string> mHistory{};
    std::vector<std::string> mDictionary{};
    std::string mPreset{};
    std::string mPrompt{};
#ifdef __linux__
    struct termios mOldTermios{};
    struct termios mCurrentTermios{};

    EscapeCodes escString2Code(const char *apEscStr);
    void handleTabulator(int &tab_count, std::string &line, unsigned int &cursor);
#endif
};

} /* namespace rsp::posix */

#endif /* INCLUDE_POSIX_TERMINALIO_H_ */
