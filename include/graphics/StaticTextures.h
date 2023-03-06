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

class TextureNotFound : public exceptions::CoreException
{
public:
    TextureNotFound(uintptr_t aId) : CoreException("Texture " + std::to_string(aId) + " does not exist") {};
};


class StaticTextures : public rsp::utils::Singleton<StaticTextures>
{
public:
    virtual ~StaticTextures() {}

    virtual void Load(Renderer &arRenderer) {};

    const Texture& GetTexture(uintptr_t aId) const
    {
        try {
            return *(mTextures.at(aId));
        }
        catch (const std::out_of_range &e) {
            THROW_WITH_BACKTRACE1(TextureNotFound, aId);
        }
    }

protected:
    std::map<uintptr_t, std::unique_ptr<Texture>> mTextures{};
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_STATICTEXTURES_H_ */
