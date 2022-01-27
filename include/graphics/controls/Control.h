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
    Rect mArea;
    Color mBackground;
    bool mTransparent;
    Control *mParent;
    std::vector<Control *> mChildren;

    Control(Rect &aRect);
    ~Control();

    void SetState(States aState);
    void Invalidate();
    bool IsInvalid() const;
    bool IsTransparent() const;
    virtual void Render(Canvas &aCanvas) = 0;
    void SetArea(Rect const &aRect);
    Rect &GetArea();

    bool mIsInvalid = false;
    States mState;
};
} // namespace rsp::graphics

#endif // CONTROL_H
