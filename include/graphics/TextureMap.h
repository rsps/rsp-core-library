/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_TEXTUREMAP_H_
#define INCLUDE_GRAPHICS_TEXTUREMAP_H_

#include <graphics/Texture.h>
#include <map>
#include <utils/Singleton.h>
#include <utils/ConstTypeInfo.h>

namespace rsp::graphics {

class TextureNotFound : public exceptions::CoreException
{
public:
    TextureNotFound(uint32_t aId) : CoreException("Texture " + std::to_string(aId) + " does not exist") {};
};

class TextureExists : public exceptions::CoreException
{
public:
    TextureExists(uint32_t aId) : CoreException("Texture " + std::to_string(aId) + " already exists") {};
};


class TextureMap
{
public:
    virtual ~TextureMap() {}

    virtual void Load() {};

    std::unique_ptr<const Texture> Get(uint32_t aId) const
    {
        return get(aId);
    }

    template<class T>
    std::unique_ptr<const Texture> Get() const
    {
        return get(rsp::utils::ID<T>());
    }

protected:
    std::map<uint32_t, std::unique_ptr<Texture> > mTextures{};

    std::unique_ptr<Texture> get(uint32_t aId) const
    {
        try {
            return mTextures.at(aId)->Clone();
        }
        catch (const std::out_of_range &e) {
            THROW_WITH_BACKTRACE1(TextureNotFound, aId);
        }
    }

    void add(uint32_t aId, Texture* apTexture)
    {
        auto result = mTextures.emplace(aId, apTexture);
        if (result.second == false) {
            THROW_WITH_BACKTRACE1(TextureExists, aId);
        }
    }
};

class StaticTextures : public TextureMap, public rsp::utils::Singleton<StaticTextures>
{
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_TEXTUREMAP_H_ */
