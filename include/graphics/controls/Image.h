#ifndef IMAGE_H
#define IMAGE_H

#include "graphics/controls/Control.h"
#include "graphics/primitives/Bitmap.h"

namespace rsp::graphics
{

class Image : public Control
{
public:
    Image() : Control(){};
    Image(std::string aNormal, std::string aPressed, Rect &aRect);
    ~Image();

    void Render(Canvas &aCanvas) override;
    void HandleCallback(States aState);

protected:
    std::map<States, Bitmap> mStateMap{};
};

} // namespace rsp::graphics
#endif // IMAGE_H
