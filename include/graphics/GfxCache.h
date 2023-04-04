/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */


#ifndef INCLUDE_GRAPHICS_GFXCACHE_H_
#define INCLUDE_GRAPHICS_GFXCACHE_H_

#include <map>
#include "GfxResource.h"
#include "PixelData.h"
#include "Texture.h"
#include <exceptions/CoreException.h>
#include <utils/Singleton.h>

namespace rsp::graphics {

class PixelDataNotFound : public exceptions::CoreException
{
public:
    PixelDataNotFound(uint32_t aId) : CoreException("PixelData " + std::to_string(aId) + " does not exist") {};
};

class GfxCache : public rsp::utils::Singleton<GfxCache>
{
public:
    PixelData& MakePixelData(const GfxResource &arResource);
    PixelData& MakePixelData(const GfxResource &arResource, uint32_t aId);
    PixelData& GetPixelData(uint32_t aId);

    TexturePtr_t& MakeTexture(const PixelData &arPixelData);
    TexturePtr_t& MakeTexture(const PixelData &arPixelData, uint32_t aId);
    TexturePtr_t& GetTexture(uint32_t aId);

protected:
    std::map<uint32_t, PixelData>    mPixelDataList{};
    std::map<uint32_t, TexturePtr_t> mTextureList{};

    PixelData* findPixelData(uint32_t aId);
    TexturePtr_t* findTexture(uint32_t aId);
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_GFXCACHE_H_ */
