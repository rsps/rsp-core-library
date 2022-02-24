#ifndef CONTROL_H
#define CONTROL_H

#include "graphics/primitives/Color.h"
#include "graphics/primitives/Rect.h"
#include <vector>

namespace rsp::graphics
{
class Control
{
  public:
    enum class States : int {
        pressed,
        normal
    };
    Rect mArea{};
    Color mBackground{0};
    bool mTransparent = false;
    Control *mParent;
    std::vector<Control *> mChildren{};
    bool mIsInvalid = true;
    States mState = States::normal;

    Control(Control *aParent = nullptr) : mParent(aParent){};
    Control(Rect &aRect, Control *aParent = nullptr);
    ~Control();

    void SetState(States aState);
    void Invalidate();
    bool IsInvalid() const;
    bool IsTransparent() const;
    virtual void Render(Canvas &aCanvas) = 0;
    void SetArea(Rect const &aRect);
    Rect &GetArea();

    std::string myName = "Base";
};
} // namespace rsp::graphics

#endif // CONTROL_H
