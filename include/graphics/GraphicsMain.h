#ifndef GRAPHICSMAIN_H
#define GRAPHICSMAIN_H

#include "graphics/InputCreator.h"
#include "graphics/SceneLoader.h"
#include "graphics/messaging/Subscriber.h"
#include "graphics/primitives/BufferedCanvas.h"

namespace rsp::graphics
{

class GraphicsMain : public Subscriber
{
  public:
    GraphicsMain(BufferedCanvas &aCanvas, InputCreator &aInputs, SceneLoader &aSceneLoader);
    ~GraphicsMain();

    void Run();
    void Terminate() { mTerminated = true; }
    void ChangeScene(std::string aSceneName);
    void handleEvent(Event &newEvent) override;

  private:
    BufferedCanvas &mBufferedCanvas;
    InputCreator &mInputs;
    SceneLoader &mSceneLoader;
    Scene mActiveScene;
    bool mTerminated = false;
    std::vector<Input> inputCache{};

    // Temp
    // Scene &otherScene;
    void PrintInput(Input aInput);
};
} // namespace rsp::graphics
#endif // GRAPHICSMAIN_H