#include <string.h>
#include "dec128.h"
#include "decimalInt.h"

#define dec128__G1G2Mask         0x60
#define dec128__TrailSigMask     0x3F

#define dec128__DecletCount \
    ((dec128_Precision - 1) / decimal__DigitCountPerDeclet)

#define dec128__isSigCase1(n) \
    (((n)->byte[0] & dec128__G1G2Mask) == dec128__G1G2Mask)

static uint32_t dec128__clength(dec128 *n);

static int32_t dec128__getExponent(dec128 *n)
{
    return ((n->byte[0] >> (dec128__isSigCase1(n) ? 3 : 5) & 0x3) << 12
        | (n->byte[0] & 0x3) << 10
        | n->byte[1] << 2
        | n->byte[2] >> 6)
        - dec128_Bias;
}

static uint32_t dec128__getLeadDigit(dec128 *n)
{
    return dec128__isSigCase1(n) 
        ? 8 | n->byte[0] >> 2 & 0x1
        : n->byte[0] >> 2 & 0x7;
}

static dec128__isByteRangeAllZero(dec128 *n, int start, int end)
{
    int i;

    for (i = start; i < end; ++i) {
        if (n->byte[i] != 0) {
            return false;
        }
    }
    return true;
}

bool dec128_isZero(dec128 *n)
{
    return dec128_isFinite(n)
        && dec128__getLeadDigit(n) == 0
        && (n->byte[2] & dec128__TrailSigMask) == 0
        && dec128__isByteRangeAllZero(n, 3, dec128__ByteLength);
}

bool dec128_isNormal(dec128 *n)
{
    return dec128_isFinite(n) && !dec128_isZero(n) && !dec128_isSubnormal(n);
}

bool dec128_isSubnormal(dec128 *n)
{
    if (!dec128_isFinite(n) || dec128_isZero(n)) {
        return false;
    }
}

bool dec128_isCanonical(dec128 *n)
{
    return false;
}

/*
 * decimal128 encoding
+-+--------+--------------------------------------------------------------+
|S|   G    |                            T                                 |
+-+--------+--------------------------------------------------------------+
|0|0      0|0                                                            1|
|0|0      1|1                                                            2|
|0|1      7|8                                                            7|
+-+--------+--------------------------------------------------------------+

byte
 0   1    2     3     4  5  6    7      8   9   10   11   12    13  14  15
|00|0 0|0  0|0    0|0  0|00|00|0    0|0  0|0  0|0 0|0  0|0   1|1  1|1 1|11|
|00|0 1|1  2|2    3|3  3|44|45|5    6|6  7|7  7|8 8|8  9|9   0|0  1|1 1|22|
|07|8 5|6  3|4    1|2  9|07|85|6    3|4  1|2  9|0 7|8  5|6   3|4  1|2 9|07|

declet      0     1     2     3    4      5     6     7      8    9    10
|         |0 0|0     0|0  0|0  0|0    0|0   0|0   0|0     0|0  1|1  1|1  1|
|         |1 2|2     3|3  4|4  5|5    6|6   7|7   8|8     9|9  0|0  1|1  2|
|         |8 7|8     7|8  7|8  7|8    7|8   7|8   7|8     7|8  7|8  7|8  7|
*/

static int32_t dec128__getDeclet(dec128 *n, uint32_t offset)
{
    switch (offset) {
    case 0:
        return (n->byte[2] & 0x3F) << 4 | (n->byte[3] >> 4);
    case 1:
        return (n->byte[3] & 0x0F) << 2 | (n->byte[4] >> 2);
    case 2:
        return (n->byte[4] & 0x03) << 8 | n->byte[5];
    case 3:
        return n->byte[6] << 6 | n->byte[7] >> 6;
    case 4:
        return (n->byte[7] & 0x3F) << 4 | (n->byte[8] >> 4);
    case 5:
        return (n->byte[8] & 0x0F) << 2 | (n->byte[9] >> 2);
    case 6:
        return (n->byte[9] & 0x03) << 8 | n->byte[10];
    case 7:
        return n->byte[11] << 6 | n->byte[12] >> 6;
    case 8:
        return (n->byte[12] & 0x3F) << 4 | (n->byte[13] >> 4);
    case 9:
        return (n->byte[13] & 0x0F) << 2 | (n->byte[14] >> 2);
    case 10:
        return (n->byte[14] & 0x03) << 8 | n->byte[15];
    }
}

static uint32_t dec128__clength(dec128 *n)
{
    int32_t i;
    int32_t j;
    int32_t declet;
    int32_t digit[decimal__DigitCountPerDeclet];

    if (dec128__getLeadDigit(n)) {
        return dec128_Precision;
    }

    for (i = 0; i < dec128__DecletCount; ++i) {
        declet = dec128__getDeclet(n, i);
        if (declet != 0) {
            decimal__decodeDPD(digit, declet);
            for (j = 0; j < decimal__DigitCountPerDeclet; ++j) {
                if (digit[j]) {
                    return dec128_Precision
                        - (i * decimal__DigitCountPerDeclet + j);
                }
            }
        }
    }
    return 1;
}

/*
 * memo
 * a = a1 * 100 + a2 (0 <= a1 < 100, 0 <= a2 < 100)
 * b = b1 * 100 + b2 (0 <= b1 < 100, 0 <= b2 < 100)
 *
 * 0 <= a1 * b1 < 10000
 *
 * a * b = (a1 * 100 + a2) * (b1 * 100 + b2)
 *       = a1b1 * 10000 + (a1b2 + a2b1) * 100 + a2b2
 *       = (H(a1b1)*100+L(a1b1)) * 10000
 *         + (H(a1b2)*100+L(a1b2) + H(a2b1)*100+L(a2b1)) * 100
 *         + H(a2b2)*100+L(a2b2)
 *       = H(a1b1) * 1000000
 *         + (L(a1b1) + H(a1b2) + H(a2b1)) * 10000
 *         + (L(a1b2) + L(a2b1) + H(a2b2)) * 100
 *         + L(a2b2)
 *
 * a * b = (ha * 100 + la) * (hb * 100 + lb)
 *       = hahb * 10000 + (halb + lahb) * 100 + lalb
 *       = (H(hahb)*100+L(hahb)) * 10000
 *         + (H(halb)*100+L(halb) + H(lahb)*100+L(lahb)) * 100
 *         + H(lalb)*100+L(lalb)
 *       = H(hahb) * 1000000
 *         + (L(hahb) + H(halb) + H(lahb)) * 10000
 *         + (L(halb) + L(lahb) + H(lalb)) * 100
 *         + L(lalb)
 */

bool dec128__wk_convertFromDec128(dec128__wk *result, const dec128 *n)
{
    int32_t i;
    int32_t declet;
    int32_t digit[3];
    char *p;

    result->sign = dec128_isSignMinus(n) ? 1 : 0;
    result->kind = dec128_isFinite(n) ? dec128__kind_Finite
        : dec128_isInfinite(n) ? dec128__kind_Infinite
        : dec128_isSignaling(n) ? dec128__kind_SNaN
        : dec128__kind_QNaN;

    result->q = dec128__getExponent(n);

    p = result->c;
    *p++ = dec128__getLeadDigit(n) + '0';
    for (i = 0; i < dec128__DecletCount; ++i) {
        declet = dec128__getDeclet(n, i);
        decimal__decodeDPD(digit, declet);
        *p++ = digit[0] + '0';
        *p++ = digit[1] + '0';
        *p++ = digit[2] + '0';
    }
    while (p < result->c + dec128__wk_DigitCapacity) {
        *p++ = '\0';
    }
}

bool dec128__initWithRawHexString(dec128 *n, const char *hex)
{
    int len = strlen(hex);
    uint8_t *dest = n->byte;
    int i;
    int hi;
    int lo;
    char *p;
    char *q;

    if (len != (dec128_BitWidth / 8) * 2) {
        return false;
    }

    for (i = 0; i < len; i += 2) {
        hi = toupper(hex[i]);
        p = strchr(decimal__HexDigits, hi);
        if (!p) {
            return false;
        }
        hi = p - decimal__HexDigits;

        lo = toupper(hex[i + 1]);
        p = strchr(decimal__HexDigits, lo);
        if (!p) {
            return false;
        }
        lo = p - decimal__HexDigits;

        *dest++ = hi << 4 | lo;
    }
}
