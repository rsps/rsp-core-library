/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#include <application/Console.h>
#include <posix/TerminalIO.h>
#include <cstdio>
#include <string>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <utils/AnsiEscapeCodes.h>
#include <utils/StrUtils.h>

using namespace rsp::utils;
using namespace rsp::application;

namespace rsp::posix {

#ifdef __linux__

TerminalIO::TerminalIO()
{
    tcgetattr(0, &mOldTermios);             // grab old terminal i/o settings

    SetEcho(false);
}

TerminalIO::TerminalIO(const std::string &arPreset)
{
    TerminalIO();
    mPreset = arPreset;
}

TerminalIO::~TerminalIO()
{
    tcsetattr(0, TCSANOW, &mOldTermios);
}

TerminalIO& TerminalIO::SetEcho(bool aEcho)
{
    mCurrentTermios = mOldTermios;          // make new settings same as old settings

    mCurrentTermios.c_lflag &= static_cast<tcflag_t>(~ICANON);     // disable buffered i/o
    if (aEcho) {
        mCurrentTermios.c_lflag |= ECHO;    // set echo mode
    }
    else {
        mCurrentTermios.c_lflag &= static_cast<tcflag_t>(~ECHO);   // set no echo mode
    }

    mCurrentTermios.c_cc[VTIME] = 1; // is non-zero: Wait up to 20ms for a character
    mCurrentTermios.c_cc[VMIN]  = 0; // is zero: Return after timeout if no character has been received

    tcsetattr(0, TCSANOW, &mCurrentTermios); // Use these new terminal i/o settings now

    return *this;
}

char TerminalIO::GetChar(EscapeCodes &arEscCode)
{
    char c;

    do {
        c = std::getchar();
    }
    while (c == static_cast<char>(-1));

    arEscCode = EscapeCodes::Character;

    if (c == 0x1b) {
        // Escape character
        char ext[6] {0};
        ext[0] = c;
        ext[1] = std::getchar();
        if (ext[1] == 0) {
            return c; // The Esc key is not followed by any extended values
        }
        else
        {
            ext[2] = std::getchar();
            if (ext[2] < 0x3F) {
                ext[3] = std::getchar();
                if (ext[3] < 0x3F) {
                    ext[4] = std::getchar();
                }
            }
        }

        if (mVerbose) {
            Console::Debug() << std::hex << "(";
            for (auto a : ext) {
                Console::Debug() << static_cast<int>(a) << " ";
            }
            Console::Debug() << ") " << std::endl;
        }

        arEscCode = escString2Code(ext);

        if (mVerbose) {
            Console::Debug() << "ExcCode: " << static_cast<int>(arEscCode) << std::endl;
        }
    }
    else if (c == AnsiEscapeCodes::ec::Backspace) {
        arEscCode = EscapeCodes::Backspace;
    }
    else if (c == '\t') {
        // Tab character
        arEscCode = EscapeCodes::Tabulator;
    }

    return c;
}

/**
 * Get a command line from the terminal
 *
 * The terminal is simulated, with history and cursor controlled by ASCII escape commands.
 * @see https://en.wikipedia.org/wiki/ANSI_escape_code
 *
 * @returns string with input command line.
 */
std::string TerminalIO::GetLine()
{
    char ch;
    std::string line;
    unsigned int cursor = 0;
    unsigned int history_cursor = mHistory.size();
    bool edit_mode = false;
    int tab_count = 0;
    EscapeCodes esc;

    if (HasPreset()) {
        return StrUtils::Trim(mPreset);
    }

    Console::Info() << mPrompt << TextColor::Input;

    while( (ch = GetChar(esc)) ) {

        switch (esc) {
            case EscapeCodes::Character:
                if (ch != '\n') {
                    line.insert(cursor++, &ch, 1);
                    edit_mode = true;
                }
                break;

            case EscapeCodes::Backspace:
                if (cursor > 0) {
                    line.erase(--cursor, 1);
                }
                break;

            case EscapeCodes::Delete:
                if (cursor < line.length()) {
                    line.erase(cursor--, 1);
                }
                break;

            case EscapeCodes::Home:
                cursor = 0;
                break;

            case EscapeCodes::End:
                cursor = line.length();
                break;

            case EscapeCodes::ArrowRight:
                if (cursor < line.length()) {
                    cursor++;
                    std::cout << AnsiEscapeCodes::ec::cursor::Right << std::flush;
                }
                break;

            case EscapeCodes::ArrowLeft:
                if (cursor > 0) {
                    cursor--;
                    std::cout << AnsiEscapeCodes::ec::cursor::Left << std::flush;
                }
                break;

            case EscapeCodes::ArrowDown:
                if ((history_cursor+1) < mHistory.size()) {
                    if (edit_mode) {
                        if (history_cursor <= mHistory.size()) {
                            mHistory[history_cursor] = line;
                        }
                        else {
                            mHistory.push_back(line);
                        }
                    }
                    line = mHistory.at(++history_cursor);
                    cursor = line.length();
                    edit_mode = false;
                }
                break;

            case EscapeCodes::ArrowUp:
                if (history_cursor > 0) {
                    if (edit_mode) {
                        if (history_cursor <= mHistory.size()) {
                            mHistory[history_cursor] = line;
                        }
                        else {
                            mHistory.push_back(line);
                        }
                    }
                    line = mHistory.at(--history_cursor);
                    cursor = line.length();
                    edit_mode = false;
                }
                break;

            case EscapeCodes::Tabulator:
            {
                std::vector<std::string> matches = StrUtils::FindMatches(line, mDictionary);

                if (++tab_count == 2) {
                    tab_count = 0;

                    if (matches.empty() ||
                       (matches.size() == 1 && matches[0] == line)) {
                        break;
                    }

                    int n = std::count(line.begin(), line.end(), ' ');

                    Console::Info() << std::endl;
                    for (auto s : matches) {
                        if (std::count(s.begin(), s.end(), ' ') == n) {
                            Console::HighLightInfo() << s << std::endl;
                        }
                    }
                    break;
                }

                std::string common = StrUtils::ReduceToCommon(matches);

                if (common.length() > line.length()) {
                    line = common;
                    if (matches[0] == line && matches.size() > 1) {
                        line += ' ';
                    }
                    else {
                        tab_count = 0;
                    }
                    cursor = line.length();
                }
                break;
            }

            case EscapeCodes::Esc:
                line.clear();
                cursor = 0;
                edit_mode = true;

                break;

            case EscapeCodes::Unknown:
            default:
                break;
        }

        if (esc != EscapeCodes::Tabulator) {
            tab_count = 0; // Clear tabulator double click on all other keys.
        }

        std::cout << AnsiEscapeCodes::ec::cursor::EraseLineMoveToCol1 << std::flush; // Erase line and move cursor to column 1.
        Console::Info() << mPrompt << TextColor::Input << line << std::flush;
        if (ch == '\n') {
            std::cout << ch;
            break;
        }
        std::cout << AnsiEscapeCodes::ec::cursor::SetPosition(cursor + 1 + mPrompt.length()) << std::flush; // Move cursor to correct position
    }

    if (mVerbose) {
        Console::Debug() << "$Debug: ";
        for (auto c : line) {
            Console::Debug() << std::hex << static_cast<int>(c) << " ";
        }
        Console::Debug() << "-- " << line << std::endl;
    }

    StrUtils::Trim(line);
    if (line.length() > 0) {
        if (mHistory.size()) {
            if (mHistory.back().compare(line)) {
                mHistory.push_back(line);
            }
        }
        else {
            mHistory.push_back(line);
        }
    }

    return line;
}

TerminalIO::EscapeCodes TerminalIO::escString2Code(const char *apEscStr)
{
    struct ansi2io_t {
        AnsiEscapeCodes::EscString aec;
        EscapeCodes ec;
    };

    const ansi2io_t map[] {
        { AnsiEscapeCodes::ec::arrow::Up, EscapeCodes::ArrowUp },
        { AnsiEscapeCodes::ec::arrow::Down, EscapeCodes::ArrowDown },
        { AnsiEscapeCodes::ec::arrow::Right, EscapeCodes::ArrowRight },
        { AnsiEscapeCodes::ec::arrow::Left, EscapeCodes::ArrowLeft },
        { AnsiEscapeCodes::ec::Delete, EscapeCodes::Delete },
        { AnsiEscapeCodes::ec::Home, EscapeCodes::Home },
        { AnsiEscapeCodes::ec::End, EscapeCodes::End },
        { AnsiEscapeCodes::ec::Esc, EscapeCodes::Esc },
    };

    for (auto &a : map) {
        if (std::strcmp(a.aec, apEscStr) == 0) {
            return a.ec;
        }
    }

    return EscapeCodes::Unknown;
}

char TerminalIO::WaitForAnyKey()
{
    char c;
    do {
        c = std::getchar(); // getchar behavior changed by TerminalIO: timeout of 20ms on key wait.
    }
    while (c == static_cast<char>(-1));

    return c;
}

#else /* __linux__ */
#warning "This is a POSIX file, only cleared for building for Linux"
#endif /* __linux__ */
} /* namespace hwtest */
