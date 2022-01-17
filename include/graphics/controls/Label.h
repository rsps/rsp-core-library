#ifndef LABEL_H
#define LABEL_H

#include "graphics/controls/Control.h"

namespace rsp::graphics
{

class Label : public Control
{
  private:
    std::string mCaption;
    // Font mFont;

  public:
    Label();
    ~Label();

    void SetCaption(std::string const &aCaption);
};

} // namespace rsp::graphics
#endif // LABEL_H