/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */


#ifndef TESTS_HELPERS_SCENES_SCENE480X800_H_
#define TESTS_HELPERS_SCENES_SCENE480X800_H_

#include <graphics/controls/Scene.h>

namespace rsp::graphics {

class Scene480x800 : public Scene
{
public:
    Scene480x800(const std::string &arName): Scene({0, 0, 480, 800}, arName) {}
};

} // namespace rsp::graphics

#endif /* TESTS_HELPERS_SCENES_SCENE480X800_H_ */
