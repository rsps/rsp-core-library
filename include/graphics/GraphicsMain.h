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
    void Terminate() { mTerminated = true; }

    // Temp
    void ChangeScene();

  private:
    BufferedCanvas &mBufferedCanvas;
    InputCreator &mInputs;
    Scene &mActiveScene;
    bool mTerminated = false;
    std::vector<Input> inputCache{};

    // Temp
    Scene &otherScene;
    void PrintInput(Input aInput);
};
} // namespace rsp::graphics
#endif // GRAPHICSMAIN_H