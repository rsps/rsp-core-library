/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef TESTS_HELPERS_OVERLAY_H_
#define TESTS_HELPERS_OVERLAY_H_

#include <graphics/GfxEngine.h>
#include <graphics/Label.h>
#include <graphics/Rect.h>

class Overlay : public rsp::graphics::Label
{
public:

    Overlay(rsp::graphics::GfxEngineBase &arGfx)
        : mrGfx(arGfx)
    {
        using namespace rsp::graphics;
        SetTransparent(false);
        mText.GetFont().SetSize(12); //.SetBackgroundColor(Color::Black);
        SetArea(Rect(0, 0, 300, 15));
        GetStyle(States::Normal).mForegroundColor = Color::Yellow;
        GetStyle(States::Normal).mBackgroundColor = Color::Black;
        SetVAlignment(Text::VAlign::Top).SetHAlignment(Text::HAlign::Left);
        SetName("Overlay");
    }

protected:
    rsp::graphics::GfxEngineBase &mrGfx;
    int mIterations = 0;
    int mTotalFps = 0;
    int mMinFps = 10000000;
    int mMaxFps = 0;

    void refresh() override
    {
        int fps = mrGfx.GetFPS();
        mIterations++;
        mTotalFps += fps;
        if (fps && (fps < mMinFps)) {
            mMinFps = fps;
        }
        if (fps > mMaxFps) {
            mMaxFps = fps;
        }
        SetCaption("FPS cur: " + std::to_string(fps) + ", avg: " + std::to_string(mTotalFps / mIterations)
            + ", min: " + std::to_string(mMinFps) + ", max: " + std::to_string(mMaxFps) + ", cnt: " + std::to_string(mIterations));
        Label::refresh();
    }
};


#endif /* TESTS_HELPERS_OVERLAY_H_ */
