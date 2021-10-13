/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_UTILS_ANSIESCAPECODES_H_
#define SRC_UTILS_ANSIESCAPECODES_H_

/**
 * \namespace AnsiEscapeCodes
 * \brief Namespace for ANSI escape codes
 *
 * \see https://en.wikipedia.org/wiki/ANSI_escape_code
 * \see http://ascii-table.com/ansi-escape-sequences-vt-100.php
 * \see https://stackoverflow.com/a/13805058
 */
namespace rsp::utils::AnsiEscapeCodes {

typedef const char * EscString;

/**
 * \class ec
 * \brief Helper class with some predefined ANSI escape codes
 */
class ec // Escape Codes
{
public:
    class fg // Foreground colors
    {
    public:
        static EscString Black;
        static EscString Blue;
        static EscString Red;
        static EscString Magenta;
        static EscString Green;
        static EscString Cyan;
        static EscString Yellow;
        static EscString White;
        static EscString Console;

        static EscString LightBlack;
        static EscString LightBlue;
        static EscString LightRed;
        static EscString LightMagenta;
        static EscString LightGreen;
        static EscString LightCyan;
        static EscString LightYellow;
        static EscString LightWhite;
    };

    class bg // Background colors
    {
    public:
        static EscString Black;
        static EscString Blue;
        static EscString Red;
        static EscString Magenta;
        static EscString Green;
        static EscString Cyan;
        static EscString Yellow;
        static EscString White;
        static EscString Console;

        static EscString LightBlack;
        static EscString LightBlue;
        static EscString LightRed;
        static EscString LightMagenta;
        static EscString LightGreen;
        static EscString LightCyan;
        static EscString LightYellow;
        static EscString LightWhite;
    };

    static EscString ConsoleDefault;
    static EscString Underline;
    static EscString Bold;
    static EscString Reset;

    enum class Color {
        Black,
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,
        Cyan,
        White,
        Default = 9
    };

    enum class Attributes {
        Reset,
        Bright,
        Dim,
        Underline,
        Blink,
        Reverse,
        Hidden
    };

    static char *color(Attributes attr, Color fg, Color bg);

    class cursor
    {
    public:
        static EscString Right;
        static EscString Left;
        static EscString EraseLineMoveToCol1;
        static EscString SetPosition(int aPos);
    };

    class arrow
    {
    public:
        static EscString Up;
        static EscString Down;
        static EscString Right;
        static EscString Left;
    };

    static EscString Delete;
    static EscString Home;
    static EscString End;
    static EscString Esc;
    static const char Backspace;


    static EscString PartialEscape;
    static EscString PartialMoveCursor;

    static EscString Degree;

    static EscString ClearDisplay;
    static EscString CursorUpperLeft;

    static EscString DisableScreenSaver;
    static EscString EnableScreenSaver;
    static EscString HideCursor;
    static EscString InverseScreenOn;
    static EscString InverseScreenOff;

    static EscString ResetAll;
    static EscString TputClear;
};

} /* namespace AnsiEscapeCodes */

#endif /* SRC_UTILS_ANSIESCAPECODES_H_ */
