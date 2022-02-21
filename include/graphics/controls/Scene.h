#ifndef SCENE_H
#define SCENE_H

#include "graphics/controls/Control.h"
#include "graphics/controls/Input.h"
#include "graphics/controls/TouchArea.h"
#include "graphics/messaging/Publisher.h"
#include <vector>

namespace rsp::graphics
{

class Scene : public Control, public Publisher
{
  public:
    Scene(Rect &aRect);
    ~Scene();

    void ProcessInput(Input &aInput);
    void AddArea(TouchArea &aArea);
    void Render(Canvas &aCanvas) override;

    void AddChildren(Control &aCtrl);

    std::vector<TouchArea *> mTouchables;
};

} // namespace rsp::graphics
#endif // SCENE_H