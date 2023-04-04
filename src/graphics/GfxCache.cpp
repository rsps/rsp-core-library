/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */


#include <graphics/GfxCache.h>
#include <graphics/TextureMap.h>

namespace rsp::graphics {

PixelData& GfxCache::MakePixelData(const GfxResource &arResource)
{
    return MakePixelData(arResource, arResource.Id);
}

PixelData& GfxCache::MakePixelData(const GfxResource &arResource, uint32_t aId)
{
    auto pd = findPixelData(aId);
    if (!pd) {
        auto pair = mPixelDataList.emplace(aId, arResource);
        pd = &(pair.first->second);
    }
    return *pd;
}

PixelData& GfxCache::GetPixelData(uint32_t aId)
try
{
    return mPixelDataList.at(aId);
}
catch(const std::out_of_range &e) {
    THROW_WITH_BACKTRACE1(PixelDataNotFound, aId);
}

TexturePtr_t& GfxCache::MakeTexture(const PixelData &arPixelData)
{
    return MakeTexture(arPixelData, arPixelData.GetId());
}

TexturePtr_t& GfxCache::MakeTexture(const PixelData &arPixelData, uint32_t aId)
{
    auto result = findTexture(aId);
    if (!result) {
        auto pair = mTextureList.insert({aId, Texture::Create(arPixelData)});
        result = &(pair.first->second);
    }
    return *result;
}

TexturePtr_t& GfxCache::GetTexture(uint32_t aId)
try
{
    return mTextureList.at(aId);
}
catch(const std::out_of_range &e) {
    THROW_WITH_BACKTRACE1(TextureNotFound, aId);
}

PixelData* GfxCache::findPixelData(uint32_t aId)
{
    auto it = mPixelDataList.find(aId);
    if (it != mPixelDataList.end()) {
        return &it->second;
    }
    return nullptr;
}

TexturePtr_t* GfxCache::findTexture(uint32_t aId)
{
    auto it = mTextureList.find(aId);
    if (it != mTextureList.end()) {
        return &it->second;
    }
    return nullptr;
}

} /* namespace rsp::graphics */
