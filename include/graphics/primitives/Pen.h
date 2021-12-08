#ifndef PEN_H
#define PEN_H

#include <graphics/primitives/Color.h>
#include <iostream>

#include "Bitmap.h"
#include "Color.h"
#include "Point.h"

namespace rsp::graphics
{

class Canvas;

/**
 * Class Pen
 *
 * A pen contains a size an a color.
 * Descendants can even implement pens of different sizes.
 */
class Pen
{
  public:
    /**
     * Construct a pen of a certain size and color.
     *
     * \param aSize
     * \param aColor
     */
    Pen(int aSize, Color aColor);

    /**
     * Draw the pen on the given canvas at the given point.
     *
     * \param aCanvas
     * \param aPoint
     */
    void Draw(Canvas &aCanvas, const Point &aPoint) const;

    /**
     * Get the color of the pen.
     *
     * \return
     */
    inline const Color &GetColor()
    {
        return mColor;
    }
    /**
     * Set the color of the pen.
     *
     * \param aColor
     * \return
     */
    inline Pen &SetColor(Color aColor)
    {
        mColor = aColor;
        return *this;
    }

  protected:
    int mSize;
    Color mColor;
};

} // namespace rsp::graphics
#endif // PEN_H
