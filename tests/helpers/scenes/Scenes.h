/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef TESTS_HELPERS_SCENES_SCENES_H_
#define TESTS_HELPERS_SCENES_SCENES_H_

#include <graphics/controls/SceneMap.h>

#include "FirstScene.h"
#include "SecondScene.h"


class Scenes : public rsp::graphics::SceneMap
{
public:
    Scenes()
        : SceneMap()
    {
        MakeScene<rsp::graphics::FirstScene>();
        MakeScene<rsp::graphics::SecondScene>();
    }

    rsp::graphics::FirstScene& First() { return *reinterpret_cast<rsp::graphics::FirstScene*>(&operator[]("FirstScene")); }
    rsp::graphics::SecondScene& Second() { return *reinterpret_cast<rsp::graphics::SecondScene*>(&operator[]("SecondScene")); }
};


#endif /* TESTS_HELPERS_SCENES_SCENES_H_ */
