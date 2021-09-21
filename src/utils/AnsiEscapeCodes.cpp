/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <cstdio>
#include <utils/AnsiEscapeCodes.h>

namespace rsp::AnsiEscapeCodes {

#define CC_CONSOLE_COLOR_DEFAULT "\033[0m"
#define CC_FGCOLOR(C) "\033[" #C "m"
#define CC_BGCOLOR(C) "\033[" #C "m"
#define CC_ATTR(A) "\033[" #A "m"
#define ESC(A) "\033[" A


EscString ec::fg::Black   = CC_FGCOLOR(30);
EscString ec::fg::Blue    = CC_FGCOLOR(34);
EscString ec::fg::Red     = CC_FGCOLOR(31);
EscString ec::fg::Magenta = CC_FGCOLOR(35);
EscString ec::fg::Green   = CC_FGCOLOR(32);
EscString ec::fg::Cyan    = CC_FGCOLOR(36);
EscString ec::fg::Yellow  = CC_FGCOLOR(33);
EscString ec::fg::White   = CC_FGCOLOR(37);
EscString ec::fg::Console = CC_FGCOLOR(39);

EscString ec::fg::LightBlack   = CC_FGCOLOR(90);
EscString ec::fg::LightBlue    = CC_FGCOLOR(94);
EscString ec::fg::LightRed     = CC_FGCOLOR(91);
EscString ec::fg::LightMagenta = CC_FGCOLOR(95);
EscString ec::fg::LightGreen   = CC_FGCOLOR(92);
EscString ec::fg::LightCyan    = CC_FGCOLOR(96);
EscString ec::fg::LightYellow  = CC_FGCOLOR(93);
EscString ec::fg::LightWhite   = CC_FGCOLOR(97);


EscString ec::bg::Black   = CC_BGCOLOR(40);
EscString ec::bg::Blue    = CC_BGCOLOR(44);
EscString ec::bg::Red     = CC_BGCOLOR(41);
EscString ec::bg::Magenta = CC_BGCOLOR(45);
EscString ec::bg::Green   = CC_BGCOLOR(42);
EscString ec::bg::Cyan    = CC_BGCOLOR(46);
EscString ec::bg::Yellow  = CC_BGCOLOR(43);
EscString ec::bg::White   = CC_BGCOLOR(47);
EscString ec::bg::Console = CC_BGCOLOR(49);

EscString ec::bg::LightBlack   = CC_BGCOLOR(100);
EscString ec::bg::LightBlue    = CC_BGCOLOR(104);
EscString ec::bg::LightRed     = CC_BGCOLOR(101);
EscString ec::bg::LightMagenta = CC_BGCOLOR(105);
EscString ec::bg::LightGreen   = CC_BGCOLOR(102);
EscString ec::bg::LightCyan    = CC_BGCOLOR(106);
EscString ec::bg::LightYellow  = CC_BGCOLOR(103);
EscString ec::bg::LightWhite   = CC_BGCOLOR(107);

EscString ec::ConsoleDefault = CC_CONSOLE_COLOR_DEFAULT;
EscString ec::Underline = CC_ATTR(4);
EscString ec::Bold = CC_ATTR(1);
EscString ec::Reset = "\033[39;49m";

char* ec::color(Attributes attr, Color fg, Color bg)
{
    static char command[40];
    /* Command is the control command to the terminal */
    sprintf(command, "%c[%d;%d;%dm", 0x1B, static_cast<int>(attr), static_cast<int>(fg) + 30, static_cast<int>(bg) + 40);
    return command;
}


EscString ec::cursor::Right = ESC("C");
EscString ec::cursor::Left  = ESC("D");
EscString ec::cursor::EraseLineMoveToCol1 = ESC("2K") ESC("G");

EscString ec::cursor::SetPosition(int aPos)
{
    static char command[13];
    /* Command is the control command to the terminal */
    sprintf(command, ESC("%dG"), aPos);
    return command;
}

EscString ec::arrow::Up    = ESC("A");
EscString ec::arrow::Down  = ESC("B");
EscString ec::arrow::Right = ESC("C");
EscString ec::arrow::Left  = ESC("D");

EscString ec::Delete = ESC("3~");
EscString ec::Home   = ESC("H");
EscString ec::End    = ESC("F");
EscString ec::Esc    = "\033\xff\xff";
const char ec::Backspace = 0x7F;


EscString ec::PartialEscape     = ESC("");
EscString ec::PartialMoveCursor = "G";

EscString ec::Degree = "\xc2\xb0";

EscString ec::ClearDisplay    = ESC("2J");
EscString ec::CursorUpperLeft = ESC("H");

/**
 * Special escape strings for physical display control
 * Send them to /dev/ttyX
 */
EscString ec::DisableScreenSaver = ESC("9;0]");
EscString ec::EnableScreenSaver  = ESC("9;5]"); // 5 Minutes
EscString ec::HideCursor         = ESC("?17;0;0c");
EscString ec::InverseScreenOn    = ESC("?5h");
EscString ec::InverseScreenOff   = ESC("?5l");
EscString ec::ResetAll = "\033c";

EscString ec::TputClear = ESC("3J") ESC("H") ESC("2J"); //5b1b 4a33 5b1b 1b48 325b 004a

} // namespace AnsiEscapeCodes

