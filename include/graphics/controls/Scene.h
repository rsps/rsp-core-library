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
  public:
    Scene();
    ~Scene();

    void ProcessEvent(Event &aEvent);
    void AddArea(TouchArea *aArea);

    std::vector<TouchArea *> mTouchables;
};

} // namespace rsp::graphics
#endif // SCENE_H