#include "Erase.h"

static const std::uint8_t cErasePixData[408] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x30, 0x00, 0x00, 0x00, 0x00, 0x90, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0,
    0x00, 0x00, 0x00, 0x50, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x10, 0xf0, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0xb0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xe0, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xf0, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x60, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xe0, 0x30, 0xb0, 0xff, 0xff, 0xff, 0xd0, 0x40, 0xb0, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x20, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x40, 0x80, 0xff, 0xc0,
    0x30, 0xd0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0x60, 0x30, 0x30, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x60, 0x40, 0x30,
    0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x70, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xf0, 0x30, 0xa0, 0xff, 0xd0, 0x20, 0xd0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0xd0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x20, 0xc0, 0xff, 0xff, 0xff,
    0xe0, 0x30, 0xb0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x30, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xf0, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x20, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x60, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xa0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xa0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
    0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x80, 0x10
};

using namespace rsp::graphics;

const PixelData cErase(24u, 17u, PixelData::ColorDepth::Alpha, cErasePixData);
