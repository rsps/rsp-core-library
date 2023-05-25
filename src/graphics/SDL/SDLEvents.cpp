/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifdef USE_GFX_SDL

#include <SDL2/SDL.h>
#include "SDLEvents.h"

namespace rsp::graphics::sdl {

SDLEvents::SDLEvents()
{
}

bool SDLEvents::Poll(rsp::graphics::GfxEvent &arInput)
{
    SDL_Event event;

    if (SDL_PollEvent(&event) == 0) {
        return false;
    }

    switch (event.type) {
        case SDL_MOUSEMOTION:
            event.motion.x;
            event.motion.y;
//            Uint32 state;       /**< The current button state */
//            Sint32 x;           /**< X coordinate, relative to window */
//            Sint32 y;           /**< Y coordinate, relative to window */
//            Sint32 xrel;        /**< The relative motion in the X direction */
//            Sint32 yrel;        /**< The relative motion in the Y direction */
            break;

        case SDL_MOUSEBUTTONDOWN:
            event.button;
//            Uint8 button;       /**< The mouse button index */
//            Uint8 state;        /**< ::SDL_PRESSED or ::SDL_RELEASED */
//            Uint8 clicks;       /**< 1 for single-click, 2 for double-click, etc. */
//            Uint8 padding1;
//            Sint32 x;           /**< X coordinate, relative to window */
//            Sint32 y;           /**< Y coordinate, relative to window */

            EventTypes::Lift;

            break;

        case SDL_MOUSEBUTTONUP:
            event.button;
            EventTypes::Press;

            break;

        default:
            break;
    }

    return true;
}

void SDLEvents::Flush()
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        continue;
    }
}

} /* namespace rsp::graphics::sdl */

#endif /* USE_GFX_SDL */
