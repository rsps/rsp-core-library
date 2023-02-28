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

#include <graphics/primitives/Color.h>

namespace rsp::graphics
{
/**
 * BufferedCanvas interface class
 *
 * Abstract class with function declarations for buffering operations.
 */
class BufferedCanvas
{
  public:
    virtual ~BufferedCanvas()
    {
    }

    /**
     * \brief Abstract method for swapping back and front buffers
     */
    virtual void SwapBuffer() = 0;

    virtual void Fill(Color aColor = Color::Black) = 0;

  protected:
    uint32_t *mpFrontBuffer = nullptr;
    uint32_t *mpBackBuffer = nullptr;
};

} // namespace rsp::graphics
#endif // BUFFEREDCANVAS_H
