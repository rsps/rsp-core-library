#ifndef COLOUR_H
#define COLOUR_H

#include <stdint.h>

#include "utils/RSPCoreExceptions.h"

typedef union {
    uint32_t rgba;
#ifdef LITTLE_ENDIAN
    struct {
        uint32_t alpha : 8;
        uint32_t blue : 8;
        uint32_t green : 8;
        uint32_t red : 8;
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

class Colour
{
  public:
    colourMap colours;

    Colour(uint8_t aRed, uint8_t aGreen, uint8_t aBlue, uint8_t aAlpha);
    Colour(uint32_t aARGB);
    Colour(const Colour &aColour);

    uint8_t GetRed() const;
    void SetRed(uint8_t aValue);
    uint8_t GetGreen() const;
    void SetGreen(uint8_t aValue);
    uint8_t GetBlue() const;
    void SetBlue(uint8_t aValue);
    uint8_t GetAlpha() const;
    void SetAlpha(uint8_t aValue);
    operator uint32_t() const;
    Colour &operator=(const Colour &aColour);
};

#endif // COLOUR_H
