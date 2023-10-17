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

#include <string>
#include <memory>
#include <vector>
#include <SDL2/SDL.h>
#include <graphics/Rect.h>
#include <graphics/Renderer.h>
#include <utils/Singleton.h>
#include "SDLException.h"
#include "SDLEvents.h"


namespace rsp::graphics::sdl {

class SDLRenderer: public rsp::graphics::Renderer, public rsp::utils::Singleton<SDLRenderer>
{
public:
    SDLRenderer(GuiUnit_t aWidth, GuiUnit_t aHeight);
    ~SDLRenderer() override;
    SDLRenderer(const SDLRenderer&) = delete;
    SDLRenderer(SDLRenderer&&) = default;
    SDLRenderer& operator=(const SDLRenderer&) = delete;
    SDLRenderer& operator=(SDLRenderer&&) = default;

    Renderer& Blit(const Texture &arTexture) override;
    Renderer& ClearClipRect() override;
    Renderer& PushClipRect(const Rect &arClipRect) override;
    Renderer& PopClipRect() override;
    Renderer& Fill(const Color &arColor, OptionalRect aDestination) override;
    [[nodiscard]] GuiUnit_t GetWidth() const override;
    Renderer& Flush() override;
    void Present() override;
    [[nodiscard]] GuiUnit_t GetHeight() const override;
    Renderer& DrawRect(const Color &arColor, const Rect &arRect) override;
    Renderer& SetPixel(GuiUnit_t aX, GuiUnit_t aY, const Color &arColor) override;
    [[nodiscard]] Color GetPixel(GuiUnit_t aX, rsp::graphics::GuiUnit_t aY) const override;
    [[nodiscard]] ColorDepth GetColorDepth() const override;

    [[nodiscard]] SDL_Renderer* GetSDLRenderer() const { return mpRenderer; }

protected:
    Rect mArea;
    Rect mClipRect;
    std::vector<Rect> mClipRectList{};
    SDL_Window* mpWindow = nullptr;
    SDL_Renderer* mpRenderer = nullptr;
    SDLEvents mInputEvents{};

    void setClipRect();
};

} /* namespace rsp::graphics::sdl */

#endif /* USE_GFX_SDL */
#endif /* SRC_GRAPHICS_SDL_SDLRENDERER_H_ */
