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

#include <SDL2/SDL.h>
#include "SDLRenderer.h"
#include "SDLTexture.h"
#include "SDLRect.h"

namespace rsp::graphics {

Renderer& Renderer::Init(GuiUnit_t aWidth, GuiUnit_t aHeight)
{
    if (!sdl::SDLRenderer::HasInstance()) {
        sdl::SDLRenderer::CreateInstance(aWidth, aHeight);
    }
    return rsp::utils::Singleton<sdl::SDLRenderer>::GetInstance();
}

Renderer& Renderer::Get()
{
    return rsp::utils::Singleton<sdl::SDLRenderer>::GetInstance();
}

} /* namespace rsp::graphics */

namespace rsp::graphics::sdl {

SDLRenderer::SDLRenderer(GuiUnit_t aWidth, GuiUnit_t aHeight)
    : mArea(0, 0, aWidth, aHeight),
      mClipRect(0, 0, aWidth, aHeight)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        THROW_WITH_BACKTRACE1(SDLException, "SDL_Init");
    }

    mpWindow = SDL_CreateWindow("RSP", 0, 0, aWidth, aHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_MOUSE_CAPTURE);
    if (!mpWindow) {
        THROW_WITH_BACKTRACE1(SDLException, "SDL_CreateWindow");
    }

    mpRenderer = SDL_CreateRenderer(mpWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!mpRenderer) {
        THROW_WITH_BACKTRACE1(SDLException, "SDL_CreateRenderer");
    }

//    SDL_RendererInfo info;
//    if (SDL_GetRendererInfo(mpRenderer, &info)) {
//        THROW_WITH_BACKTRACE1(SDLException, "SDL_GetRendererInfo");
//    }
//    std::cout << "Renderer: " << info.name << " Flags: 0x" << std::hex << std::setfill('0') << std::setw(8) << info.flags << "\n";
//    for (uint32_t i = 0 ; i < info.num_texture_formats ; ++i) {
//        std::cout << "0x" << info.texture_formats[i] << "\n";
//    }
//    std::cout << std::dec << std::endl;

    if (SDL_SetRenderDrawBlendMode(mpRenderer, SDL_BLENDMODE_NONE)) {
        THROW_WITH_BACKTRACE1(SDLException, "SDL_SetRenderDrawBlendMode");
    }
}

SDLRenderer::~SDLRenderer()
{
    if (mpRenderer) {
        SDL_DestroyRenderer(mpRenderer);
    }
    if (mpWindow) {
        SDL_DestroyWindow(mpWindow);
    }
    SDL_Quit();
}

Renderer& SDLRenderer::Blit(const Texture &arTexture)
{
    const SDLTexture &tex = dynamic_cast<const SDLTexture&>(arTexture);
    SDLRect dr(arTexture.GetDestinationRect());
    SDLRect sr(arTexture.GetSourceRect());

//    if (tex.mBlendOperation != Texture::BlendOperation::Copy) {
//        if (SDL_SetRenderDrawBlendMode(mpRenderer, SDL_BLENDMODE_BLEND)) {
//            THROW_WITH_BACKTRACE1(SDLException, "SDL_SetRenderDrawBlendMode");
//        }
//    }

    if (SDL_RenderCopy(mpRenderer, tex.GetSDLTexture(), &sr, &dr)) {
        THROW_WITH_BACKTRACE1(SDLException, "SDL_RenderCopy");
    }

//    if (SDL_SetRenderDrawBlendMode(mpRenderer, SDL_BLENDMODE_NONE)) {
//        THROW_WITH_BACKTRACE1(SDLException, "SDL_SetRenderDrawBlendMode");
//    }
    return *this;
}

Renderer& SDLRenderer::SetClipRect(const Rect &arClipRect)
{
    mClipRect = arClipRect & mArea;
    return *this;
}

Renderer& SDLRenderer::Fill(const Color &arColor, OptionalRect aDestination)
{
    Rect dest = aDestination ? *aDestination : mArea;
    dest &= mClipRect;
    SDLRect r(dest);

    if (SDL_SetRenderDrawColor(mpRenderer, arColor.GetRed(), arColor.GetGreen(), arColor.GetBlue(), arColor.GetAlpha())) {
        THROW_WITH_BACKTRACE1(SDLException, "SDL_SetRenderDrawColor");
    }
    if (SDL_RenderFillRect(mpRenderer, &r)) {
        THROW_WITH_BACKTRACE1(SDLException, "SDL_RenderFillRect");
    }
    return *this;
}

void SDLRenderer::Present()
{
    if (SDL_RenderFlush(mpRenderer)) {
        THROW_WITH_BACKTRACE1(SDLException, "SDL_RenderFlush");
    }
    SDL_RenderPresent(mpRenderer);
//    SDL_RenderClear(mpRenderer);
}

GuiUnit_t SDLRenderer::GetWidth() const
{
    return mArea.GetWidth();
}

GuiUnit_t SDLRenderer::GetHeight() const
{
    return mArea.GetHeight();
}

Renderer& SDLRenderer::DrawRect(const Color &arColor, const Rect &arRect)
{
    SDLRect r(arRect);

    if (SDL_SetRenderDrawColor(mpRenderer, arColor.GetRed(), arColor.GetGreen(), arColor.GetBlue(), arColor.GetAlpha())) {
        THROW_WITH_BACKTRACE1(SDLException, "SDL_SetRenderDrawColor");
    }
    if (SDL_RenderDrawRect(mpRenderer, &r)) {
        THROW_WITH_BACKTRACE1(SDLException, "SDL_RenderDrawRect");
    }
    return *this;
}

Renderer& SDLRenderer::SetPixel(GuiUnit_t aX, GuiUnit_t aY, const Color &arColor)
{
    if (SDL_SetRenderDrawColor(mpRenderer, arColor.GetRed(), arColor.GetGreen(), arColor.GetBlue(), arColor.GetAlpha())) {
        THROW_WITH_BACKTRACE1(SDLException, "SDL_SetRenderDrawColor");
    }
    if (SDL_RenderDrawPoint(mpRenderer, aX, aY)) {
        THROW_WITH_BACKTRACE1(SDLException, "SDL_RenderDrawPoint");
    }

    return *this;
}

Color SDLRenderer::GetPixel(GuiUnit_t aX, GuiUnit_t aY) const
{
    Rect r(aX, aY, 1, 1);
    r &= mArea;
    SDLRect sr(r);

    uint32_t raw = 0;
    if (SDL_RenderReadPixels(mpRenderer, &sr, SDL_PIXELFORMAT_ARGB8888, &raw, int(sizeof(raw)))) {
        THROW_WITH_BACKTRACE1(SDLException, "SDL_RenderReadPixels");
    }

    return Color(raw);
}

ColorDepth SDLRenderer::GetColorDepth() const
{
    return ColorDepth::RGBA;
}

} /* namespace rsp::graphics::sdl */

#endif /* USE_GFX_SDL */
