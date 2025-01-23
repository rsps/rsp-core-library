/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#ifndef FIRSTSCENE_H
#define FIRSTSCENE_H

#include <graphics/Button.h>
#include <graphics/Scene.h>

namespace rsp::graphics {

class FirstScene : public SceneBase<FirstScene>
{
public:
    FirstScene()
        : mNormal("testImages/Red.bmp"),
          mPressed("testImages/Green.bmp")
    {
        Rect topRect(100, 400, 200, 100);
        mTopBtnImg.SetArea(topRect);
        mTopBtnImg.GetStyle(Control::States::Normal).mTextures.push_back(Texture::Create(mNormal, Color::None));
        mTopBtnImg.GetStyle(Control::States::Pressed).mTextures.push_back(Texture::Create(mPressed, Color::None));

        Rect botRect(100, 600, 200, 100);
        mBotBtnImg.SetArea(botRect);
        mBotBtnImg.GetStyle(Control::States::Normal).mTextures.push_back(Texture::Create(mNormal, Color::None));
        mBotBtnImg.GetStyle(Control::States::Pressed).mTextures.push_back(Texture::Create(mPressed, Color::None));

        // Add them to the lists?
        AddChild(&mTopBtnImg);
        AddChild(&mBotBtnImg);
    };

protected:
    Button mTopBtnImg{};
    Button mBotBtnImg{};
    Bitmap mNormal;
    Bitmap mPressed;
};

} // namespace rsp::graphics

#endif // FIRSTSCENE_H
