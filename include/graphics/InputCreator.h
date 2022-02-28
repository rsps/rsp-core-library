/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#ifndef INPUTCREATOR_H
#define INPUTCREATOR_H

#include "graphics/controls/Input.h"
#include "posix/FileIO.h"
#include <fstream>
#include <iostream>

namespace rsp::graphics
{

class InputCreator
{
  private:
    rsp::posix::FileIO touchDriver{};

    InputLine inputLine{};

    Input input{};

    void ReadType();
    void ReadBody();

  public:
    InputCreator();
    ~InputCreator();

    bool HasNewInputs();
    const Input &GetInput();

};

} // namespace rsp::graphics
#endif // INPUTCREATOR_H
