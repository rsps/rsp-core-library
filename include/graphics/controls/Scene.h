#ifndef SCENE_H
#define SCENE_H

#include "graphics/controls/Control.h"
#include "graphics/controls/Input.h"
#include "graphics/controls/TouchArea.h"
#include <vector>

namespace rsp::graphics
{

class Scene : public Control
{
  public:
    Scene(Rect &aRect);
    ~Scene();

    void ProcessInput(Input &aInput);
    void AddArea(TouchArea &aArea);
    void Render(Canvas &aCanvas) override;

    void AddChildren(Image &aImg);

    std::vector<TouchArea *> mTouchables;
};

} // namespace rsp::graphics
#endif // SCENE_H