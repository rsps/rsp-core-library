#ifndef CONTROL_H
#define CONTROL_H

#include "graphics/primitives/Color.h"
#include "graphics/primitives/Rect.h"
#include <vector>

namespace rsp::graphics
{
class Control
{
  private:
    bool mIsInvalid = false;

  public:
    Rect mArea;
    Color mBackground;
    bool mTransparent;
    Control *mParent;
    std::vector<Control *> mChildren;

    Control(Rect &aRect);
    ~Control();

    void Invalidate();
    bool IsInvalid() const;
    bool IsTransparent() const;
    void Render(Canvas &aCanvas);
    void SetArea(Rect const &aRect);
    Rect &GetArea();
};
} // namespace rsp::graphics

#endif // CONTROL_H
