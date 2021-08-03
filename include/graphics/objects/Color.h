#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

#include "../../src/utils/RSPCoreExceptions.h"

typedef struct {
    uint32_t red : 8;
    uint32_t green : 8;
    uint32_t blue : 8;
    uint32_t alpha : 8;
} colorMap;

class Color {
   public:
    colorMap color;

    Color(uint8_t aRed, uint8_t aGreen, uint8_t aBlue, uint8_t aAlpha);
    Color(uint32_t aARGB);
    Color(const Color &aColor);

    uint8_t GetRed();
    void SetRed(uint8_t aValue);
    uint8_t GetGreen();
    void SetGreen(uint8_t aValue);
    uint8_t GetBlue();
    void SetBlue(uint8_t aValue);
    uint8_t GetAlpha();
    void SetAlpha(uint8_t aValue);
    operator uint32_t();
    void operator=(const Color &aColor);
};

#endif  // COLOR_H
