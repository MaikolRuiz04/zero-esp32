#ifndef ICON_H
#define ICON_H

#include <Arduino.h>

class ScaledIcon
{
public:
    ScaledIcon(const unsigned char *data, uint16_t size)
        : data(data), size(size) {}

    const unsigned char *data;
    uint16_t size;
};

class Icon
{
public:
    Icon(const unsigned char *icon192, const unsigned char *icon128, const unsigned char *icon64, const unsigned char *icon48)
        : icon192(icon192), icon128(icon128), icon64(icon64), icon48(icon48) {}

    const unsigned char *icon192;
    const unsigned char *icon128;
    const unsigned char *icon64;
    const unsigned char *icon48;

    ScaledIcon scaled(uint16_t size)
    {
        switch (size)
        {
        case 192:
            return ScaledIcon(icon192, 192);
        case 128:
            return ScaledIcon(icon128, 128);
        case 64:
            return ScaledIcon(icon64, 64);
        case 48:
            return ScaledIcon(icon48, 48);
        default:
            return ScaledIcon(icon48, 48);
        }
    }
};

#endif // ICON_H