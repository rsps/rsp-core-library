/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <graphics/primitives/Texture.h>
#ifdef USE_SDL
    #include "SDL/SDLTexture.h"
#else
    #include "SW/SWTexture.h"
#endif

namespace rsp::graphics {

Texture::Texture(const Renderer &arRenderer, const PixelData &arPixelData)
#ifdef USE_SDL
    : mpImpl(new SDLTexture(arRenderer, arPixelData))
#else
    : mpImpl(new SWTexture(arRenderer, arPixelData))
#endif
{
}

Texture::Texture(Texture &&arOther)
    : mpImpl(std::move(arOther.mpImpl))
{
}

Texture& Texture::operator =(Texture &&arOther)
{
    if (this != &arOther) {
        mpImpl = std::move(arOther.mpImpl);
    }
    return *this;
}

Texture& Texture::operator =(const PixelData &arPixelData)
{
    mpImpl->assign(arPixelData);
    return *this;
}

void Texture::assign(const PixelData &arPixelData)
{
}

} /* namespace rsp::graphics */
