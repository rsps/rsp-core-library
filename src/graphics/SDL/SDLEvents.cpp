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

namespace rsp::graphics {

GfxInputEvents& GfxInputEvents::Get()
{
    if (!sdl::SDLEvents::HasInstance()) {
        sdl::SDLEvents::CreateInstance();
    }
    return rsp::utils::Singleton<sdl::SDLEvents>::GetInstance();
}

} /* namespace rsp::graphics */

namespace rsp::graphics::sdl {

SDLEvents::SDLEvents()
{
}

bool SDLEvents::Poll(rsp::graphics::GfxEvent &arInput)
{
    using namespace std::chrono;
    SDL_Event event;

    if (SDL_PollEvent(&event) == 0) {
        return false;
    }

    arInput.mType = EventTypes::None;

    switch (event.type) {
        case SDL_MOUSEMOTION:
            getLatestOf(SDL_MOUSEMOTION, event);
            if (event.motion.state == 1) {
                arInput.mTime = steady_clock::time_point(milliseconds{event.motion.timestamp});
                arInput.mType = EventTypes::Drag;
                arInput.mCurrent = Point(event.motion.x, event.motion.y);
            }
            else {
                return false;
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            arInput.mTime = steady_clock::time_point(milliseconds{event.motion.timestamp});
            arInput.mType = EventTypes::Press;
            arInput.mCurrent = Point(event.motion.x, event.motion.y);
            arInput.mPress = Point(event.motion.x, event.motion.y);
            arInput.mPressTime = arInput.mTime;
            break;

        case SDL_MOUSEBUTTONUP:
            arInput.mTime = steady_clock::time_point(milliseconds{event.motion.timestamp});
            arInput.mType = EventTypes::Lift;
            arInput.mCurrent = Point(event.motion.x, event.motion.y);
            break;

        case SDL_MOUSEWHEEL:
            return false;

        case SDL_QUIT:
            arInput = GfxEvent(steady_clock::time_point(milliseconds{event.quit.timestamp}), EventTypes::Quit, Point(0, 0));
            break;

        case SDL_WINDOWEVENT:
            return false;

        default:
            return false;
    }

    return true;
}

void SDLEvents::getLatestOf(std::uint32_t aEventType, SDL_Event &aEvent)
{
    SDL_PumpEvents();
    while (SDL_PeepEvents(&aEvent, 1, SDL_GETEVENT, aEventType, aEventType)) {
        continue;
    }
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
