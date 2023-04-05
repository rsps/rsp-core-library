#include "GfxResources.h"

using namespace rsp::graphics;

static const std::uint8_t pixdata[44]{
    0x01, 0x00, 0x01, 0x90, 0x2a, 0xff, 0x01, 0x90, 0x01, 0x00, 0x01, 0x90, 0x2c, 0xff, 0x01, 0x90, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x69, 0xff, 0x01, 0x90, 0x2c, 0xff, 
    0x01, 0x90, 0x01, 0x00, 0x01, 0x90, 0x2a, 0xff, 0x01, 0x90, 0x01, 0x00
};

const GfxResource cSmallSpecial{3070026919u, 46u, 34u, ColorDepth::Alpha, true, 44, pixdata};