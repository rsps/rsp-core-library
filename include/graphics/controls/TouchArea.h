#ifndef TOUCHAREA_H
#define TOUCHAREA_H

#include "graphics/controls/Image.h"
#include "graphics/controls/Input.h"
#include "graphics/messaging/eventTypes/ClickedEvent.h"
#include "graphics/primitives/Rect.h"

namespace rsp::graphics
{

class TouchArea
{
  public:
    TouchArea() : mTouchArea(0, 0, 0, 0), mClickedTopic(Topic::Base), mclickEvent(""){};
    TouchArea(Rect &aArea, Topic aClickTopic = Topic::Base, std::string aClickInfo = "");
    ~TouchArea();

    void ProcessInput(Input &aInput);
    bool IsHit(const Point &aPoint) const;
    void RegisterOnPressed(std::function<void(Control::States)> aFunc);
    void RegisterOnClicked(std::function<void(Topic, ClickedEvent &)> aFunc);

    TouchArea &operator=(const TouchArea &) = default;

    std::function<void(Control::States)> mPressed = [](Control::States) {};
    std::function<void(Topic, ClickedEvent &)> mClicked = [](Topic, ClickedEvent &) {};

    Rect mTouchArea;
    Point mCurrentPress{0, 0};
    Point mOriginalPress{0, 0};
    Topic mClickedTopic;
    ClickedEvent mclickEvent;
};

} // namespace rsp::graphics
#endif // TOUCHAREA_H