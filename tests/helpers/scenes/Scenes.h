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
#include "InputScene.h"


class Scenes : public rsp::graphics::SceneMap
{
public:
    Scenes()
        : SceneMap()
    {
        using namespace rsp::graphics;
        AddFactory(FirstScene);
        AddFactory(SecondScene);
        AddFactory(InputScene);
    }
};


#endif /* TESTS_HELPERS_SCENES_SCENES_H_ */
