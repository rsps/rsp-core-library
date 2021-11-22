#include "Event.h"
#include <fstream>
#include <iostream>

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
