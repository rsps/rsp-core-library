/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <graphics/primitives/Renderer.h>
#ifdef USE_SDL
    #include "SDL/SDLRenderer.h"
#else
    #include "SW/SWRenderer.h"
#endif

namespace rsp::graphics {

Renderer::Renderer()
#ifdef USE_SDL
    : mpImpl(new SDLRenderer())
#else
    : mpImpl(new SWRenderer())
#endif
{
}

} /* namespace rsp::graphics */
