/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_GRAPHICS_SDL_SDLEVENTS_H_
#define SRC_GRAPHICS_SDL_SDLEVENTS_H_

#ifdef USE_GFX_SDL

#include <SDL2/SDL.h>
#include <graphics/GfxInputEvents.h>

namespace rsp::graphics::sdl {


class SDLEvents: public GfxInputEvents
{
public:
    SDLEvents();

    SDLEvents(SDLEvents&&) = default;
    SDLEvents& operator=(SDLEvents&&) = default;

    bool Poll(GfxEvent &arEvent) override;
    void Flush() override;

protected:
    rsp::graphics::TouchEvent mLastEvent{};

    static void getLatestOf(uint32_t aEventType, SDL_Event &aEvent);

};


} /* namespace rsp::graphics::sdl */

#endif /* USE_GFX_SDL */
#endif /* SRC_GRAPHICS_SDL_SDLEVENTS_H_ */
