#include "decimalInt.h"

int32_t decimal__encodeDecletFromDigits(const int32_t *digit)
{
    if (digit[0] < 8) {
        if (digit[1] < 8) {
            if (digit[2] < 8) {
                return digit[0] << 7 | digit[1] << 4 | digit[2];
            } else {
                return digit[0] << 7 | digit[1] << 4 | digit[2];
            }
        } else {
            if (digit[2] < 8) {
                return digit[0] << 7 | (digit[2] & 0x6 | digit[1] & 0x1) << 4
                    | 0xA | digit[2] & 0x1;
            } else {
                return digit[0] << 7 | (0x4 | digit[1] & 0x1) << 4
                    | 0xE | digit[2] & 0x1;
            }
        }
    } else {
        if (digit[1] < 8) {
            if (digit[2] < 8) {
                return (digit[2] & 0x6 | digit[0] & 0x1) << 7
                    | (digit[1] & 0x7) << 4 | 0xC | digit[2] & 0x1;
            } else {
                return (digit[1] & 0x6 | digit[0] & 0x1) << 7
                    | (0x2 | digit[1] & 0x1) << 4 | 0xE | digit[2] & 0x1;
            }
        } else {
            if (digit[2] < 8) {
                return (digit[2] & 0x6 | digit[0] & 0x1) << 7
                    | (digit[1] & 0x1) << 4 | 0xE | digit[2] & 0x1;
            } else {
                return (digit[0] & 0x1) << 7
                    | (0x6 | digit[1] & 0x1) << 4 | 0xE | digit[2] & 0x1;
            }
        }
    }
}

void decimal__decodeDecletToDigits(int32_t *digit, int32_t declet)
{
    if ((declet & 0x008) == 0) {
        digit[0] = declet >> 7;
        digit[1] = declet >> 4 & 0x7;
        digit[2] = declet & 0x7;
    } else {
        switch (declet & 0x00E) {
        case 0x008:
            digit[0] = declet >> 7;
            digit[1] = declet >> 4 & 0x7;
            digit[2] = 0x8 | (declet & 0x7);
            break;
        case 0x00A:
            digit[0] = declet >> 7;
            digit[1] = 0x8 | (declet >> 4 & 0x1);
            digit[2] = (declet >> 4 & 0x6) | (declet & 0x1);
            break;
        case 0x00C:
            digit[0] = 0x8 | (declet >> 7 & 0x1);
            digit[1] = declet >> 4 & 0x7;
            digit[2] = (declet >> 7 & 0x6) | (declet & 0x1);
            break;
        default:
            switch (declet & 0x060) {
            case 0x040:
                digit[0] = declet >> 7;
                digit[1] = 0x8 | (declet >> 4 & 0x1);
                digit[2] = 0x8 | (declet & 0x1);
                break;
            case 0x020:
                digit[0] = 0x8 | (declet >> 7 & 0x1);
                digit[1] = (declet >> 7 & 0x6) | (declet >> 4 & 0x1);
                digit[2] = 0x8 | (declet & 0x1);
                break;
            case 0x000:
                digit[0] = 0x8 | (declet >> 7 & 0x1);
                digit[1] = 0x8 | (declet >> 4 & 0x1);
                digit[2] = (declet >> 7 & 0x6) | (declet & 0x1);
                break;
            default:
                digit[0] = 0x8 | (declet >> 7 & 0x1);
                digit[1] = 0x8 | (declet >> 4 & 0x1);
                digit[2] = 0x8 | (declet & 0x1);
                break;
            }
            break;
        }
    }
}

int32_t decimal__encodeDecletFromInt(int32_t value)
{
    int32_t digit[decimal__DigitCountPerDeclet];

    digit[2] = value % 10;
    digit[1] = (value / 10) % 10;
    digit[0] = value / 100;
    return decimal__encodeDecletFromDigits(digit);
}

int32_t decimal__decodeDecletToInt(int32_t declet)
{
    int32_t digit[decimal__DigitCountPerDeclet];
    decimal__decodeDecletToDigits(digit, declet);
    return digit[0] * 100 + digit[1] * 10 + digit[2];
}

