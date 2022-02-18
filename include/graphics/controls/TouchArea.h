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
    TouchArea() {};
    TouchArea(Rect &aArea);
    ~TouchArea();

    void ProcessInput(Input &aInput);

    bool IsHit(const Point &aPoint) const;

    // Register callback method for an Image or simply a Control?
    void RegisterOnPressed(std::function<void(Control::States)> aFunc);
    void RegisterOnClicked(std::function<void()> aFunc);

    TouchArea &operator=(const TouchArea &) = default;

    TouchArea& SetArea(const Rect &arRect) { mTouchArea = arRect; return *this; }
    const Rect& GetArea() const { return mTouchArea; }

protected:
    std::function<void(Control::States)> mPressed{}; // = [](Control::States) {};
    std::function<void()> mClicked{}; // = [](void) {};

    Rect mTouchArea{};
    Point mCurrentPress{0, 0};
    Point mOriginalPress{0, 0};
};

} // namespace rsp::graphics
#endif // TOUCHAREA_H
