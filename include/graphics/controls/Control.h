
#ifndef CONTROL_H
#define CONTROL_H

#include "primitives/Rect.h"

namespace rsp::graphics
{
class Control
{
  private:
    bool mIsInvalid = false;

  public:
    Rect mArea;

    Control(Rect &aRect);
    ~Control();

    void Invalidate();
    bool IsInvalid() const;
    bool IsHit();
};
} // namespace rsp::graphics

#endif //CONTROL_H
