#ifndef TOUCHAREA_H
#define TOUCHAREA_H

#include "graphics/controls/Image.h"
#include "graphics/controls/Input.h"
#include "graphics/primitives/Rect.h"

namespace rsp::graphics
{

class TouchArea
{
  public:
    TouchArea() : mTouchArea(0, 0, 0, 0){};
    TouchArea(Rect &aArea);
    ~TouchArea();

    void ProcessInput(Input &aInput);
    bool IsHit(const Point &aPoint) const;
    // Register callback method for an Image or simply a Control?
    void RegisterOnPressed(std::function<void(Control::States)> aFunc);
    void RegisterOnClicked(std::function<void(std::string)> aFunc, std::string aClickedInfo);

    TouchArea &operator=(const TouchArea &) = default;

    std::function<void(Control::States)> mPressed = [](Control::States) {};
    std::function<void(std::string)> mClicked = [](std::string) {};

    Rect mTouchArea;
    Point mCurrentPress{0, 0};
    Point mOriginalPress{0, 0};
    std::string mClickedInfo = "";
};

} // namespace rsp::graphics
#endif // TOUCHAREA_H