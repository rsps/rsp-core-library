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

bool SDLEvents::Poll(GfxEvent &arInput)
{
    using namespace std::chrono;
    SDL_Event event;

    if (SDL_PollEvent(&event) == 0) {
        return false;
    }

    mLastTouchEvent.mType = TouchTypes::None;

    switch (event.type) {
        case SDL_MOUSEMOTION:
            getLatestOf(SDL_MOUSEMOTION, event);
            if (event.motion.state == 1) {
                mLastTouchEvent.mTime = steady_clock::time_point(milliseconds{event.motion.timestamp});
                mLastTouchEvent.mType = TouchTypes::Drag;
                mLastTouchEvent.mCurrent = Point(event.motion.x, event.motion.y);
                arInput = mLastTouchEvent;
            }
            else {
                return false;
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            mLastTouchEvent.mTime = steady_clock::time_point(milliseconds{event.motion.timestamp});
            mLastTouchEvent.mType = TouchTypes::Press;
            mLastTouchEvent.mCurrent = Point(event.motion.x, event.motion.y);
            mLastTouchEvent.mPress = Point(event.motion.x, event.motion.y);
            mLastTouchEvent.mPressTime = mLastTouchEvent.mTime;
            arInput = mLastTouchEvent;
            break;

        case SDL_MOUSEBUTTONUP:
            mLastTouchEvent.mTime = steady_clock::time_point(milliseconds{event.motion.timestamp});
            mLastTouchEvent.mType = TouchTypes::Lift;
            mLastTouchEvent.mCurrent = Point(event.motion.x, event.motion.y);
            arInput = mLastTouchEvent;
            break;

        case SDL_MOUSEWHEEL:
            return false;

        case SDL_QUIT:
            arInput = QuitEvent();
            break;

        case SDL_WINDOWEVENT:
            return false;

        default:
            return false;
    }

    return true;
}

void SDLEvents::getLatestOf(uint32_t aEventType, SDL_Event &aEvent)
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
