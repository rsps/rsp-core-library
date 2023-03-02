/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_STATICTEXTURES_H_
#define INCLUDE_GRAPHICS_STATICTEXTURES_H_

#include <graphics/Renderer.h>
#include <graphics/Texture.h>
#include <map>
#include <utils/Singleton.h>

namespace rsp::graphics {

class StaticTextures : public rsp::utils::Singleton<StaticTextures>
{
public:
    virtual ~StaticTextures() {}

    virtual void Load(Renderer &arRenderer) {};

    const Texture& GetTexture(uintptr_t aId) const { return *(mTextures.at(aId)); }

protected:
    std::map<uintptr_t, std::unique_ptr<Texture>> mTextures{};
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_STATICTEXTURES_H_ */
