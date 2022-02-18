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
    rsp::posix::FileIO touchDriver;

    InputLine inputLine;

    Input input;

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