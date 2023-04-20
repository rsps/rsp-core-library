/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifdef USE_GFX_SDL

#include <SDL2pp/SDL2pp.hh>
#include <SDL2/SDL.h>
#include "SDLRenderer.h"

namespace rsp::graphics {

Renderer& Renderer::Get()
{
    if (!sdl::SDLRenderer::HasInstance()) {
        sdl::SDLRenderer::CreateInstance();
    }
    return rsp::utils::Singleton<sdl::SDLRenderer>::GetInstance();
}

} /* namespace rsp::graphics */

namespace rsp::graphics::sdl {

SDLRenderer::SDLRenderer()
{
    mpSdl = new SDL(SDL_INIT_VIDEO);

    mpWindow = new Window("Framebuffer Emulator",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            480, 800,
            SDL_WINDOW_SHOWN | SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_MOUSE_CAPTURE);

    // Create accelerated video renderer with default driver
    mpRenderer = new Renderer(*mpWindow, -1, SDL_RENDERER_ACCELERATED);
    mpRenderer->SetDrawBlendMode(SDL_BLENDMODE_NONE);
}

SDLRenderer::~SDLRenderer()
{
}

rsp::graphics::Renderer& SDLRenderer::Blit(const rsp::graphics::Texture &arTexture)
{
    return *this;
}

rsp::graphics::Renderer& SDLRenderer::SetClipRect(const rsp::graphics::Rect &arClipRect)
{
    mClipRect = arClipRect; // TODO: & mRendererRect;
    return *this;
}

rsp::graphics::Renderer& SDLRenderer::Fill(rsp::graphics::Color aColor, rsp::graphics::OptionalRect aDestination)
{
    return *this;
}

rsp::graphics::GuiUnit_t SDLRenderer::GetWidth() const
{
    return *this;
}

void SDLRenderer::Present()
{
}

rsp::graphics::GuiUnit_t SDLRenderer::GetHeight() const
{
}

rsp::graphics::Renderer& SDLRenderer::DrawRect(rsp::graphics::Color aColor, const rsp::graphics::Rect &arRect)
{
    return *this;
}

rsp::graphics::Renderer& SDLRenderer::SetPixel(rsp::graphics::GuiUnit_t aX, rsp::graphics::GuiUnit_t aY,
    const rsp::graphics::Color &arColor)
{
    return *this;
}

rsp::graphics::Color SDLRenderer::GetPixel(rsp::graphics::GuiUnit_t aX, rsp::graphics::GuiUnit_t aY, bool aFront) const
{
}

rsp::graphics::ColorDepth SDLRenderer::GetColorDepth() const
{
}

} /* namespace rsp::graphics::sdl */

#endif /* USE_GFX_SDL */
