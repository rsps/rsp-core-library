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
  public:
    InputCreator();
    ~InputCreator();

    bool HasNewInputs();
    const Input &GetInput();

  private:
    // std::ifstream touchDriver;
    rsp::posix::FileIO touchDriver{};

    InputLine inputLine{};

    Input input{};

    void readType();
    void readBody();
};

} // namespace rsp::graphics
#endif // INPUTCREATOR_H
