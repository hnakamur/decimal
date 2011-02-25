#include "decimalInt.h"

int32_t decimal__encodeDPD(const int32_t *digit)
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
                return (digit[2] & 0x6 | digit[0] & 0x1) << 7
                    | (digit[1] & 0x7) << 4 | 0xC | digit[2] & 0x1;
            }
        }
    } else {
        if (digit[1] < 8) {
            if (digit[2] < 8) {
                return digit[0] << 7 | (0x4 | digit[1] & 0x1) << 4
                    | 0x7 | digit[2] & 0x1;
            } else {
                return (digit[1] & 0x6 | digit[0] & 0x1) << 7
                    | (0x4 | digit[1] & 0x1) << 4 | 0xE | digit[2] & 0x1;
            }
        } else {
            if (digit[2] < 8) {
                return (digit[2] & 0x6 | digit[0] & 0x1) << 7
                    | digit[1] & 0x1 << 4 | 0xE | digit[2] & 0x1;
            } else {
                return digit[0] & 0x1 << 7
                    | (0x6 | digit[1] & 0x1) << 4 | 0xE | digit[2] & 0x1;
            }
        }
    }
}

void decimal__decodeDPD(int32_t *digit, int32_t dpd)
{
    if ((dpd & 0x008) == 0) {
        digit[0] = dpd >> 7;
        digit[1] = dpd >> 4 & 0x7;
        digit[2] = dpd & 0x7;
    } else {
        switch (dpd & 0x00E) {
        case 0x008:
            digit[0] = dpd >> 7;
            digit[1] = dpd >> 4 & 0x7;
            digit[2] = dpd & 0x7;
            break;
        case 0x00A:
            digit[0] = dpd >> 7;
            digit[1] = 0x8 | (dpd >> 4 & 0x1);
            digit[2] = (dpd >> 4 & 0x6) | (dpd & 0x1);
            break;
        case 0x00C:
            digit[0] = 0x8 | (dpd >> 7 & 0x1);
            digit[1] = dpd >> 4 & 0x7;
            digit[2] = (dpd >> 8) | (dpd & 0x1);
            break;
        default:
            switch (dpd & 0x060) {
            case 0x040:
                digit[0] = dpd >> 7;
                digit[1] = 0x8 | (dpd >> 4 & 0x1);
                digit[2] = 0x8 | (dpd & 0x1);
                break;
            case 0x020:
                digit[0] = 0x8 | (dpd >> 7 & 0x1);
                digit[1] = (dpd >> 8 & 0x3) | (dpd >> 4 & 0x1);
                digit[2] = 0x8 | (dpd & 0x1);
                break;
            case 0x000:
                digit[0] = 0x8 | (dpd >> 7 & 0x1);
                digit[1] = 0x8 | (dpd >> 4 & 0x1);
                digit[2] = (dpd >> 8) | (dpd & 0x1);
                break;
            default:
                digit[0] = 0x8 | (dpd >> 7 & 0x1);
                digit[1] = 0x8 | (dpd >> 4 & 0x1);
                digit[2] = 0x8 | (dpd & 0x1);
                break;
            }
            break;
        }
    }
}
