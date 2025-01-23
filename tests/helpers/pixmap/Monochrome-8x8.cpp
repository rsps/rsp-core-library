#include "GfxResources.h"

using namespace rsp::graphics;

static const uint8_t pixdata[14]{
    0x02, 0xfb, 0x01, 0xfa, 0x01, 0xf8, 0x01, 0xf1, 0x01, 0xe6, 0x01, 0x8c, 0x01, 0x3d
};

const GfxResource cMonochrome_8x8{3516800709u, 8u, 8u, ColorDepth::Monochrome, true, 14, pixdata};
