#ifndef BUFFEREDCANVAS_H
#define BUFFEREDCANVAS_H

#include "primitives/Canvas.h"

namespace rsp::graphics
{
/**
 * BufferedCanvas interface class
 *
 * Abstract class with function declarations for buffering operations.
 */
class BufferedCanvas : public Canvas
{
  public:
    virtual ~BufferedCanvas()
    {
    }
    /**
     * Enumerated SwapOperations
     *
     * In case of double buffer support, these values can control
     * the content of the new buffer:
     *  NoOp:  No initialization of the buffer is performed
     *  Copy:  The current view content is copied int new buffer
     *  Clear: The new buffer is filled with the background color.
     */
    enum class SwapOperations {
        NoOp,
        Copy,
        Clear
    };
    virtual void SwapBuffer(const SwapOperations aSwapOp = SwapOperations::Copy) = 0;

  protected:
    virtual void clear() = 0;
    virtual void copy() = 0;

    uint8_t *mpFrontBuffer;
    uint8_t *mpBackBuffer;
};

} // namespace rsp::graphics
#endif // BUFFEREDCANVAS_H