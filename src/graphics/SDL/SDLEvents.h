/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_SRC_GRAPHICS_SDL_SDL_EVENTS_H
#define RSP_CORE_LIB_SRC_GRAPHICS_SDL_SDL_EVENTS_H

#ifdef RSP_CORE_LIB_USE_GFX_SDL2

#include <SDL2/SDL.h>
#include <rsp/graphics/GfxInputEvents.h>

namespace rsp::graphics::sdl {


class SDLEvents: public GfxInputEvents
{
public:
    SDLEvents();

    bool Poll(GfxEvent &arEvent) override;
    void Flush() override;

protected:
    rsp::graphics::TouchEvent mLastEvent{};

    static void getLatestOf(uint32_t aEventType, SDL_Event &aEvent);

};


} /* namespace rsp::graphics::sdl */

#endif /* RSP_CORE_LIB_USE_GFX_SDL2 */
#endif // RSP_CORE_LIB_SRC_GRAPHICS_SDL_SDL_EVENTS_H
