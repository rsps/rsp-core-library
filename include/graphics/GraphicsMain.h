#ifndef GRAPHICSMAIN_H
#define GRAPHICSMAIN_H

#include "graphics/BufferedCanvas.h"
#include "graphics/EventCreator.h"
#include "graphics/controls/Scene.h"

namespace rsp::graphics
{

class GraphicsMain
{
  public:
    GraphicsMain(BufferedCanvas &aCanvas, EventCreator &aEvents, Scene &aScene);
    ~GraphicsMain();

    void Run();

    void Terminate() { mTerminated = true; }

  private:
    BufferedCanvas &mCanvas;
    EventCreator &mEvents;
    Scene &mActiveScene;
    bool mTerminated = false;
};
} // namespace rsp::graphics
#endif // GRAPHICSMAIN_H