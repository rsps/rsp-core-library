#include "EventCreator.h"

#include <linux/input.h>

EventCreator::EventCreator()
{
    touchDriver.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    touchDriver.open("/dev/input/event1", std::ifstream::binary);
}

EventCreator::~EventCreator()
{
    touchDriver.close();
}

bool EventCreator::HasNewEvents()
{
    if (touchDriver.peek() == EOF) {
        return false;
    }
    return true;
}

const Event &EventCreator::GetEvent()
{
    ReadType();
    ReadBody();

    return event;
}

void EventCreator::ReadType()
{
    touchDriver.read(reinterpret_cast<char *>(&eventInput), sizeof(eventInput));
    if (eventInput.type == EV_ABS && eventInput.code == ABS_MT_TRACKING_ID && eventInput.value == 0xFFFFFFFF) {
        event.type = EventType::Lift;
    } else if (eventInput.type == EV_ABS && eventInput.code == ABS_MT_TRACKING_ID) {
        event.type = EventType::Press;
    } else if (eventInput.type == EV_ABS &&
               (eventInput.code == ABS_MT_POSITION_X || eventInput.code == ABS_MT_POSITION_Y)) {
        event.type = EventType::Drag;
    }
}

void EventCreator::ReadBody()
{
    while (eventInput.type != 0) {
        if (eventInput.type == EV_ABS && eventInput.code == ABS_X) {
            event.x = eventInput.value;
        }
        if (eventInput.type == EV_ABS && eventInput.code == ABS_Y) {
            event.y = eventInput.value;
        }
        touchDriver.read(reinterpret_cast<char *>(&eventInput), sizeof(eventInput));
    }
}