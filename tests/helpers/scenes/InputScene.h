/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef TESTS_HELPERS_SCENES_INPUTSCENE_H_
#define TESTS_HELPERS_SCENES_INPUTSCENE_H_

#include <graphics/controls/Button.h>
#include <graphics/controls/Scene.h>
#include <graphics/controls/Keyboard.h>
#include <graphics/primitives/Bitmap.h>

namespace rsp::graphics {

class InputScene : public SceneBase<InputScene>
{
public:
    using Clicked_t = rsp::utils::Function<void(void)>;

    InputScene()
    {
        mKeyboard.SetArea(Rect(12, 450, 460, 350));
        mKeyboard.GetInfo().mName = "Keyboard";

        AddChild(&mKeyboard);
    };

    Keyboard& GetKeyboard() { return mKeyboard; }

protected:
    Keyboard mKeyboard{};
};

} // namespace rsp::graphics

#endif /* TESTS_HELPERS_SCENES_INPUTSCENE_H_ */
