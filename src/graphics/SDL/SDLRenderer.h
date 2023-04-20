/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_GRAPHICS_SDL_SDLRENDERER_H_
#define SRC_GRAPHICS_SDL_SDLRENDERER_H_

#ifdef USE_GFX_SDL

#include <SDL2pp/SDL2pp.hh>
#include <graphics/Renderer.h>
#include <utils/Singleton.h>

namespace rsp::graphics::sdl {

class SDLRenderer: public rsp::graphics::Renderer, public rsp::utils::Singleton<SDLRenderer>
{
public:
    SDLRenderer();
    ~SDLRenderer() override;

    Renderer& Blit(const Texture &arTexture) override;
    Renderer& SetClipRect(const Rect &arClipRect) override;
    Renderer& Fill(Color aColor, rsp::graphics::OptionalRect aDestination) override;
    GuiUnit_t GetWidth() const override;
    void Present() override;
    GuiUnit_t GetHeight() const override;
    Renderer& DrawRect(Color aColor, const Rect &arRect) override;
    Renderer& SetPixel(GuiUnit_t aX, GuiUnit_t aY, const Color &arColor) override;
    Color GetPixel(GuiUnit_t aX, rsp::graphics::GuiUnit_t aY, bool aFront) const override;
    ColorDepth GetColorDepth() const override;

protected:
    Rect mClipRect{};
    SDL2pp::SDL *mpSdl = nullptr;
    SDL2pp::Window *mpWindow = nullptr;
    SDL2pp::Renderer *mpRenderer = nullptr;
};

} /* namespace rsp::graphics::sdl */

#endif /* USE_GFX_SDL */
#endif /* SRC_GRAPHICS_SDL_SDLRENDERER_H_ */
