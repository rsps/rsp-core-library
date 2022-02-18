/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#ifndef BUFFEREDCANVAS_H
#define BUFFEREDCANVAS_H

#include "graphics/primitives/Canvas.h"

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
    virtual void SwapBuffer(const SwapOperations aSwapOp = SwapOperations::Copy, Color aColor = Color::Black) = 0;

  protected:
    virtual void clear(Color aColor) = 0;
    virtual void copy() = 0;

    uint8_t *mpFrontBuffer = nullptr;
    uint8_t *mpBackBuffer = nullptr;
};

} // namespace rsp::graphics
#endif // BUFFEREDCANVAS_H
