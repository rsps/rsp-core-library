#include "GfxResources.h"

using namespace rsp::graphics;

static const std::uint8_t pixdata[1022]{
    0x75, 0x00, 0x01, 0x18, 0x0c, 0x00, 0x01, 0x6c, 0x01, 0x30, 0x0b, 0x00, 0x01, 0x44, 0x01, 0x38, 
    0x04, 0x00, 0x01, 0x40, 0x06, 0x00, 0x01, 0x44, 0x01, 0x16, 0x04, 0x00, 0x01, 0x40, 0x06, 0x00, 
    0x01, 0x84, 0x01, 0x13, 0x04, 0x00, 0x01, 0x40, 0x06, 0x00, 0x01, 0x0c, 0x01, 0x10, 0x01, 0x4c, 
    0x01, 0x07, 0x01, 0x3c, 0x01, 0x00, 0x01, 0x40, 0x04, 0x00, 0x01, 0xc0, 0x01, 0x01, 0x01, 0x08, 
    0x01, 0x10, 0x01, 0xd7, 0x01, 0x04, 0x01, 0x43, 0x01, 0xf8, 0x01, 0x41, 0x04, 0x00, 0x01, 0x70, 
    0x01, 0x03, 0x01, 0x08, 0x01, 0x90, 0x01, 0x50, 0x01, 0x84, 0x01, 0x83, 0x01, 0x0c, 0x01, 0x41, 
    0x01, 0x0e, 0x01, 0x0c, 0x01, 0xdf, 0x01, 0xe7, 0x01, 0x10, 0x01, 0x02, 0x01, 0x08, 0x01, 0x90, 
    0x01, 0x59, 0x01, 0x84, 0x01, 0x80, 0x01, 0x04, 0x01, 0xc0, 0x01, 0x89, 0x01, 0xf6, 0x01, 0xd1, 
    0x01, 0x9c, 0x01, 0x10, 0x01, 0x02, 0x01, 0x08, 0x01, 0x10, 0x01, 0x4f, 0x01, 0x84, 0x01, 0x83, 
    0x01, 0x04, 0x01, 0x40, 0x01, 0x98, 0x01, 0x33, 0x01, 0xd1, 0x01, 0x8c, 0x01, 0xd4, 0x01, 0x03, 
    0x01, 0x0c, 0x01, 0x00, 0x01, 0x40, 0x01, 0x0c, 0x01, 0x66, 0x01, 0x0c, 0x01, 0x60, 0x01, 0x10, 
    0x01, 0x61, 0x01, 0x50, 0x01, 0x8c, 0x01, 0x38, 0x05, 0x00, 0x01, 0x1e, 0x01, 0xf8, 0x01, 0x61, 
    0x01, 0x18, 0x01, 0xc1, 0x01, 0xff, 0x01, 0x8c, 0x01, 0x10, 0x08, 0x00, 0x01, 0x08, 0x01, 0x01, 
    0x01, 0x60, 0x01, 0x8c, 0x01, 0x20, 0x01, 0x01, 0x07, 0x00, 0x01, 0x08, 0x01, 0x00, 0x01, 0x60, 
    0x01, 0x80, 0x01, 0xe1, 0x01, 0x01, 0x0d, 0x00, 0x01, 0x60, 0x0c, 0x00, 0x01, 0xe0, 0x01, 0x01, 
    0x0b, 0x00, 0x01, 0x80, 0x01, 0x07, 0x0c, 0x00, 0x01, 0x0f, 0x0c, 0x00, 0x01, 0x1e, 0x08, 0x00, 
    0x01, 0x80, 0x01, 0x03, 0x02, 0x00, 0x01, 0x6c, 0x08, 0x00, 0x01, 0xf0, 0x03, 0x00, 0x01, 0xd8, 
    0x01, 0x01, 0x07, 0x00, 0x01, 0x38, 0x03, 0x00, 0x01, 0xb0, 0x01, 0x03, 0x07, 0x00, 0x01, 0x0e, 
    0x03, 0x00, 0x01, 0x60, 0x01, 0x06, 0x06, 0x00, 0x01, 0xc0, 0x01, 0x03, 0x03, 0x00, 0x01, 0xc0, 
    0x01, 0x1c, 0x06, 0x00, 0x01, 0xf0, 0x01, 0x01, 0x03, 0x00, 0x01, 0x80, 0x01, 0x30, 0x01, 0x03, 
    0x05, 0x00, 0x01, 0x3c, 0x04, 0x00, 0x01, 0x80, 0x01, 0x61, 0x01, 0x07, 0x01, 0x0c, 0x01, 0xc0, 
    0x01, 0x01, 0x01, 0x00, 0x01, 0x80, 0x01, 0x07, 0x05, 0x00, 0x01, 0xc3, 0x01, 0x1d, 0x01, 0x1e, 
    0x01, 0x38, 0x01, 0x0f, 0x01, 0x00, 0x01, 0xc0, 0x01, 0x03, 0x05, 0x00, 0x01, 0x06, 0x01, 0x71, 
    0x01, 0x33, 0x01, 0x0e, 0x01, 0xf8, 0x01, 0x07, 0x01, 0xe0, 0x01, 0x01, 0x05, 0x00, 0x01, 0x0c, 
    0x01, 0xc0, 0x01, 0xe1, 0x01, 0x01, 0x01, 0x00, 0x01, 0xfc, 0x01, 0xff, 0x06, 0x00, 0x01, 0x18, 
    0x04, 0x00, 0x01, 0x80, 0x01, 0xf8, 0x01, 0xf0, 0x01, 0x01, 0x04, 0x00, 0x01, 0x30, 0x05, 0x00, 
    0x01, 0x90, 0x01, 0x3f, 0x05, 0x00, 0x01, 0x20, 0x01, 0x00, 0x01, 0x60, 0x03, 0x00, 0x01, 0x80, 
    0x01, 0x03, 0x05, 0x00, 0x01, 0x20, 0x01, 0x4c, 0x01, 0x7f, 0x01, 0x00, 0x01, 0x70, 0x01, 0x18, 
    0x01, 0xc0, 0x07, 0x00, 0x01, 0xf8, 0x01, 0x71, 0x01, 0x00, 0x01, 0xdf, 0x01, 0x1f, 0x01, 0x60, 
    0x07, 0x00, 0x01, 0x30, 0x02, 0x00, 0x01, 0x07, 0x01, 0x00, 0x01, 0x30, 0x06, 0x00, 0x01, 0x07, 
    0x01, 0xe0, 0x01, 0x03, 0x03, 0x00, 0x01, 0x18, 0x05, 0x00, 0x01, 0xc0, 0x01, 0x0d, 0x01, 0x30, 
    0x01, 0x3e, 0x01, 0x00, 0x01, 0xe0, 0x01, 0x00, 0x01, 0x1c, 0x05, 0x00, 0x01, 0x78, 0x02, 0x18, 
    0x01, 0x60, 0x01, 0x00, 0x01, 0xfc, 0x01, 0x1f, 0x01, 0x18, 0x01, 0x3c, 0x04, 0x00, 0x01, 0x0c, 
    0x01, 0x00, 0x01, 0x08, 0x01, 0x40, 0x01, 0x00, 0x01, 0x06, 0x01, 0x10, 0x01, 0x30, 0x01, 0x27, 
    0x04, 0x00, 0x01, 0x04, 0x01, 0x00, 0x01, 0x0c, 0x01, 0xc3, 0x01, 0x00, 0x01, 0xc2, 0x01, 0x10, 
    0x01, 0xe0, 0x01, 0x20, 0x04, 0x00, 0x01, 0x0c, 0x01, 0x18, 0x01, 0x0c, 0x01, 0x83, 0x01, 0x01, 
    0x01, 0xc2, 0x01, 0x10, 0x01, 0xc0, 0x01, 0x30, 0x04, 0x00, 0x01, 0x38, 0x01, 0x0c, 0x01, 0x08, 
    0x01, 0x00, 0x01, 0x01, 0x01, 0x03, 0x01, 0x10, 0x01, 0x80, 0x01, 0x19, 0x04, 0x00, 0x01, 0xe0, 
    0x01, 0x0f, 0x01, 0x18, 0x01, 0x80, 0x02, 0x01, 0x01, 0x18, 0x01, 0x00, 0x01, 0x09, 0x05, 0x00, 
    0x01, 0x06, 0x01, 0x30, 0x01, 0xe0, 0x01, 0x00, 0x01, 0x03, 0x01, 0x0f, 0x01, 0x00, 0x01, 0x0d, 
    0x05, 0x00, 0x01, 0x02, 0x01, 0xe0, 0x01, 0x3f, 0x01, 0x03, 0x01, 0xfe, 0x01, 0x01, 0x01, 0x80, 
    0x01, 0x07, 0x05, 0x00, 0x01, 0x03, 0x01, 0x00, 0x01, 0x80, 0x01, 0x01, 0x02, 0x00, 0x01, 0x80, 
    0x01, 0x06, 0x05, 0x00, 0x01, 0x01, 0x01, 0x00, 0x01, 0x80, 0x01, 0x00, 0x01, 0x03, 0x01, 0x00, 
    0x01, 0x80, 0x06, 0x00, 0x01, 0x01, 0x01, 0x00, 0x01, 0xc0, 0x01, 0x00, 0x01, 0x02, 0x01, 0x00, 
    0x01, 0x80, 0x06, 0x00, 0x01, 0x01, 0x01, 0x00, 0x01, 0x80, 0x01, 0x00, 0x01, 0x06, 0x01, 0x00, 
    0x01, 0x80, 0x06, 0x00, 0x01, 0x03, 0x01, 0x00, 0x01, 0x80, 0x01, 0x00, 0x01, 0x04, 0x01, 0x00, 
    0x01, 0x80, 0x06, 0x00, 0x01, 0x02, 0x01, 0x00, 0x01, 0x80, 0x01, 0x01, 0x01, 0x06, 0x01, 0x00, 
    0x01, 0x80, 0x06, 0x00, 0x01, 0x02, 0x02, 0x00, 0x01, 0xc3, 0x01, 0x03, 0x01, 0x00, 0x01, 0x80, 
    0x06, 0x00, 0x01, 0x02, 0x02, 0x00, 0x01, 0x7e, 0x02, 0x00, 0x01, 0x80, 0x01, 0x01, 0x05, 0x00, 
    0x01, 0x06, 0x05, 0x00, 0x01, 0x80, 0x01, 0x01, 0x05, 0x00, 0x01, 0x04, 0x01, 0x00, 0x01, 0x0e, 
    0x03, 0x00, 0x01, 0x80, 0x06, 0x00, 0x01, 0x0c, 0x01, 0x00, 0x01, 0x1c, 0x01, 0x00, 0x01, 0x70, 
    0x01, 0x00, 0x01, 0x80, 0x06, 0x00, 0x01, 0x18, 0x01, 0x00, 0x01, 0x7c, 0x01, 0x00, 0x01, 0x38, 
    0x01, 0x00, 0x01, 0x80, 0x06, 0x00, 0x01, 0x10, 0x01, 0x00, 0x01, 0xd8, 0x01, 0x00, 0x01, 0x3c, 
    0x01, 0x00, 0x01, 0x80, 0x06, 0x00, 0x01, 0x30, 0x01, 0x00, 0x01, 0x90, 0x01, 0x03, 0x01, 0x17, 
    0x01, 0x00, 0x01, 0xc0, 0x06, 0x00, 0x01, 0x60, 0x01, 0x00, 0x01, 0x30, 0x01, 0xfe, 0x01, 0x11, 
    0x01, 0x00, 0x01, 0x40, 0x06, 0x00, 0x01, 0x40, 0x01, 0x00, 0x01, 0x60, 0x01, 0x00, 0x01, 0x18, 
    0x01, 0x00, 0x01, 0x40, 0x06, 0x00, 0x01, 0xc0, 0x01, 0x00, 0x01, 0xc0, 0x01, 0x00, 0x01, 0x0f, 
    0x01, 0x00, 0x01, 0x60, 0x06, 0x00, 0x01, 0x80, 0x01, 0x01, 0x01, 0x80, 0x01, 0xf7, 0x01, 0x01, 
    0x01, 0x00, 0x01, 0x20, 0x07, 0x00, 0x01, 0x03, 0x01, 0x00, 0x01, 0x1c, 0x02, 0x00, 0x01, 0x30, 
    0x07, 0x00, 0x01, 0x06, 0x04, 0x00, 0x01, 0x18, 0x07, 0x00, 0x01, 0x0c, 0x04, 0x00, 0x01, 0x0c, 
    0x07, 0x00, 0x01, 0x18, 0x04, 0x00, 0x01, 0x06, 0x07, 0x00, 0x01, 0x70, 0x03, 0x00, 0x01, 0x80, 
    0x01, 0x03, 0x07, 0x00, 0x01, 0xc0, 0x03, 0x00, 0x01, 0xe0, 0x08, 0x00, 0x01, 0x80, 0x01, 0x03, 
    0x02, 0x00, 0x01, 0x3c, 0x09, 0x00, 0x01, 0x1c, 0x01, 0x00, 0x01, 0x80, 0x01, 0x07, 0x09, 0x00, 
    0x01, 0xf0, 0x01, 0x01, 0x01, 0xf0, 0x0b, 0x00, 0x01, 0xfe, 0x01, 0x1f, 0xe3, 0x00
};

const GfxResource cMonochrome{4221501913u, 100u, 100u, ColorDepth::Monochrome, true, 1022, pixdata};