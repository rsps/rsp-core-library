/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <graphics/controls/Keyboard.h>

namespace rsp::graphics {

class LowerCaseLayout: public Keyboard::Layout
{
public:
    LowerCaseLayout()
    {
        auto &btn = mKeys.emplace_back();
        Style &style = btn.GetStyle(Control::States::normal);
        style.mForegroundColor = Color::White;
        style.mBitmapView.SetBitmap(&mNormal);
    }

protected:
    Bitmap mNormal{};
    Bitmap mPressed{};
};

Keyboard::Keyboard()
{
}

Keyboard::~Keyboard()
{
    for (auto &tuple : mLayouts) {
        delete tuple.second;
    }
}

void Keyboard::doKeyClick(int aSymbol)
{
}

} /* namespace rsp::graphics */

