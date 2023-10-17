/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_SDLEXCEPTION_H
#define RSP_CORE_LIB_SDLEXCEPTION_H

#include <exceptions/CoreException.h>

namespace rsp::graphics::sdl {

class SDLException : public rsp::exceptions::CoreException
{
public:
    explicit SDLException(const char *apMsg)
            : CoreException(std::string(apMsg) + ": " + std::string(SDL_GetError()))
    {
    }
};

} // namespace rsp::graphics::sdl

#endif //RSP_CORE_LIB_SDLEXCEPTION_H
