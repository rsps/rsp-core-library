/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_GRAPHICS_SDL_SDLRECT_H_
#define SRC_GRAPHICS_SDL_SDLRECT_H_

#ifdef USE_GFX_SDL

#include <SDL2/SDL.h>
#include <graphics/Rect.h>

namespace rsp::graphics::sdl {

class SDLRect : public SDL_Rect
{
public:
    SDLRect(int aX, int aY, int aW, int aH)
        : SDL_Rect()
    {
        x = aX;
        y = aY;
        w = aW;
        h = aH;
    }

    explicit SDLRect(const Rect &arRect)
        : SDL_Rect()
    {
        x = arRect.GetLeft();
        y = arRect.GetTop();
        w = arRect.GetWidth();
        h = arRect.GetHeight();
    }
};

} /* namespace rsp::graphics::sdl */

#endif /* USE_GFX_SDL */
#endif /* SRC_GRAPHICS_SDL_SDLRECT_H_ */
