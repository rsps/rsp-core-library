#ifndef COLOR_H
#define COLOR_H

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
} colorMap;

/*typedef struct {
    uint32_t red : 8;
    uint32_t green : 8;
    uint32_t blue : 8;
    uint32_t alpha : 8;
} colorMap;*/

class Color {
   public:
    colorMap color;

    Color(uint8_t aAlpha, uint8_t aRed, uint8_t aGreen, uint8_t aBlue);
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
