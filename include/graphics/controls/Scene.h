#ifndef SCENE_H
#define SCENE_H

#include "graphics/controls/Control.h"
#include "graphics/controls/Event.h"
#include "graphics/controls/TouchArea.h"
#include <vector>

namespace rsp::graphics
{

class Scene : public Control
{
  private:
    std::vector<TouchArea *> mTouchables;

  public:
    Scene();
    ~Scene();

    void ProcessEvent(Event &aEvent);
};

} // namespace rsp::graphics
#endif // SCENE_H