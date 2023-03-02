/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_GRAPHICS_SW_SWTEXTURE_H_
#define SRC_GRAPHICS_SW_SWTEXTURE_H_

#include <graphics/Texture.h>

namespace rsp::graphics {

class SWTexture: public Texture
{
public:
    SWTexture(GuiUnit_t aWidth, GuiUnit_t aHeight, PixelData::ColorDepth aDepth);

    const PixelData& GetPixelData() const { return mPixelData; }

    void Fill(rsp::graphics::Color aColor) override;
    void Update(const PixelData &arPixelData) override;

protected:
    PixelData mPixelData;
};

} /* namespace rsp::graphics */

#endif /* SRC_GRAPHICS_SW_SWTEXTURE_H_ */
