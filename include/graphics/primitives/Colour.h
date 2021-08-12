#ifndef COLOUR_H
#define COLOUR_H

#include <stdint.h>

#include "../../src/utils/RSPCoreExceptions.h"

typedef union {
    uint32_t argb;
#ifdef LITTLE_ENDIAN
    struct {
        uint32_t blue : 8;
        uint32_t green : 8;
        uint32_t red : 8;
        uint32_t alpha : 8;
    };
#else
    struct {
        uint32_t alpha : 8;
        uint32_t red : 8;
        uint32_t green : 8;
        uint32_t blue : 8;
    };
#endif
} colourMap;

class Colour {
   public:
    colourMap colour;

    Colour(uint8_t aAlpha, uint8_t aRed, uint8_t aGreen, uint8_t aBlue);
    Colour(uint32_t aARGB);
    Colour(const Colour &aColour);

    uint8_t GetRed();
    void SetRed(uint8_t aValue);
    uint8_t GetGreen();
    void SetGreen(uint8_t aValue);
    uint8_t GetBlue();
    void SetBlue(uint8_t aValue);
    uint8_t GetAlpha();
    void SetAlpha(uint8_t aValue);
    operator uint32_t();
    void operator=(const Colour &aColour);
};

#endif  // COLOUR_H
