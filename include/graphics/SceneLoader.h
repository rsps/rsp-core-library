#ifndef SCENELOADER_H
#define SCENELOADER_H

#include "graphics/controls/Scene.h"
#include "graphics/controls/scenes/firstScene.h"
#include "graphics/controls/scenes/secondScene.h"
#include <map>

namespace rsp::graphics
{

class SceneLoader
{
  public:
    SceneLoader();
    ~SceneLoader();

    Scene &GetFirstScene();
    Scene &GetScene(std::string aSceneName);

    Rect screenSize = Rect(0, 0, 480, 800);
    std::unordered_map<std::string, Scene> mSceneMap = {
        {"first", FirstScene(screenSize)},
        {"second", SecondScene(screenSize)}};
};

} // namespace rsp::graphics
#endif // SCENELOADER_H