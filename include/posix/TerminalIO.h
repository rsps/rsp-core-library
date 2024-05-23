/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_POSIX_TERMINAL_IO_H
#define RSP_CORE_LIB_POSIX_TERMINAL_IO_H

#include <string>
#include <vector>
#include <sys/termios.h>

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
    explicit TerminalIO(const std::string &arPreset);

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
    char GetChar(EscapeCodes &arEscCode) const;

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
    [[nodiscard]] bool HasPreset() const { return (!mPreset.empty()); }
    /**
     * Remove all presets.
     */
    void ClearPreset() { mPreset.clear(); }

    /**
     * Set the prompt prefix to be shown on the terminal.
     *
     * \param arPrompt
     * \return Reference to self for fluent call chain.
     */
    TerminalIO& SetPrompt(const std::string& arPrompt) { mPrompt = arPrompt; return *this; }

    /**
     * Set a list of strings that is used for auto completion in case the tab character is pressed on the terminal.
     *
     * \param aPrompt
     * \return Reference to self for fluent call chain.
     */
    TerminalIO& SetAutocompletionDictionary(std::vector<std::string> aDict)
    {
        mDictionary = std::move(aDict);
        return *this;
    }

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
    struct termios mOldTermios{};
    struct termios mCurrentTermios{};

    static char getChar();
    static EscapeCodes escString2Code(const char *apEscStr);
    void handleTabulator(int &tab_count, std::string &line, unsigned int &cursor);
};

} /* namespace rsp::posix */

#endif // RSP_CORE_LIB_POSIX_TERMINAL_IO_H
