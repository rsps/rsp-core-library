#ifndef GRAPHICSMAIN_H
#define GRAPHICSMAIN_H

#include "graphics/BufferedCanvas.h"
#include "graphics/InputCreator.h"
#include "graphics/controls/Scene.h"

namespace rsp::graphics
{

class GraphicsMain
{
  public:
    GraphicsMain(BufferedCanvas &aCanvas, InputCreator &aInputs, Scene &aScene, Scene &aOtherScene);
    ~GraphicsMain();

    void Run();
    // Temp
    void ChangeScene();

    void Terminate() { mTerminated = true; }

  private:
    BufferedCanvas &mBufferedCanvas;
    InputCreator &mInputs;
    Scene &mActiveScene;
    bool mTerminated = false;
    void PrintInput(Input aInput);

    // Temp
    Scene &otherScene;
};
} // namespace rsp::graphics
#endif // GRAPHICSMAIN_H