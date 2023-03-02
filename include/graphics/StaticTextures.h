/*!
 * \copyright   Copyright (c) 2023 RSP Systems A/S. All rights reserved.
 * \author      steffen
 */

#ifndef INCLUDE_GRAPHICS_PRIMITIVES_STATICTEXTURES_H_
#define INCLUDE_GRAPHICS_PRIMITIVES_STATICTEXTURES_H_

#include <map>
#include <utils/Singleton.h>
#include "Renderer.h"
#include "Texture.h"

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

#endif /* INCLUDE_GRAPHICS_PRIMITIVES_STATICTEXTURES_H_ */
