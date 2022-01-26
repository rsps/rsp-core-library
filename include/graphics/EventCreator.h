#ifndef EVENTCREATOR_H
#define EVENTCREATOR_H

#include "graphics/controls/Event.h"
#include <fstream>
#include <iostream>

namespace rsp::graphics
{

class EventCreator
{
  private:
    std::ifstream touchDriver;
    EventInput eventInput;

    Event event;

    void ReadType();
    void ReadBody();

  public:
    EventCreator();
    ~EventCreator();

    bool HasNewEvents();
    const Event &GetEvent();
};

} // namespace rsp::graphics
#endif // EVENTCREATOR_H