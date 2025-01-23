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

#include "SDLEvents.h"
#include "SDLException.h"

namespace rsp::graphics::sdl {

SDLEvents::SDLEvents()
    : GfxInputEvents(true)
{
}

bool SDLEvents::Poll(GfxEvent &arInput)
{
    using namespace std::chrono;
    SDL_Event event;

    if (SDL_PollEvent(&event) == 0) {
        return false;
    }

    mLastEvent.mType = TouchTypes::None;

    switch (event.type) {
        case SDL_MOUSEMOTION:
            getLatestOf(SDL_MOUSEMOTION, event);
            if (event.motion.state == 1) {
                mLastEvent.mTime = steady_clock::time_point(milliseconds{event.motion.timestamp});
                mLastEvent.mType = TouchTypes::Drag;
                mLastEvent.mCurrent = Point(event.motion.x, event.motion.y);
                arInput = std::make_shared<TouchEvent>(mLastEvent);
            }
            else {
                return false;
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            mLastEvent.mTime = steady_clock::time_point(milliseconds{event.motion.timestamp});
            mLastEvent.mType = TouchTypes::Press;
            mLastEvent.mCurrent = Point(event.motion.x, event.motion.y);
            mLastEvent.mPress = Point(event.motion.x, event.motion.y);
            mLastEvent.mPressTime = mLastEvent.mTime;
            arInput = std::make_shared<TouchEvent>(mLastEvent);
            break;

        case SDL_MOUSEBUTTONUP:
            mLastEvent.mTime = steady_clock::time_point(milliseconds{event.motion.timestamp});
            mLastEvent.mType = TouchTypes::Lift;
            mLastEvent.mCurrent = Point(event.motion.x, event.motion.y);
            arInput = std::make_shared<TouchEvent>(mLastEvent);
            break;

        case SDL_QUIT:
            arInput = std::make_shared<QuitEvent>();
            break;

        case SDL_MOUSEWHEEL:
        case SDL_WINDOWEVENT:
        default:
            return false;
    }

    return true;
}

void SDLEvents::getLatestOf(uint32_t aEventType, SDL_Event &aEvent)
{
    SDL_PumpEvents();
    for (int err = 1; err ; ) {
        err = SDL_PeepEvents(&aEvent, 1, SDL_GETEVENT, aEventType, aEventType);
        if (err < 0) {
            THROW_WITH_BACKTRACE1(SDLException, "SDL_PeepEvents failed.");
        }
    }
}

void SDLEvents::Flush()
{
    SDL_Event event;
    for (int count = 1; count ; ) {
        count = SDL_PollEvent(&event);
    }
}

} /* namespace rsp::graphics::sdl */

#endif /* USE_GFX_SDL */
