#include <string.h>
#include "dec128.h"
#include "decimalInt.h"

#define dec128__G1G2Mask         0x60
#define dec128__TrailSigMask     0x3F
#define dec128__LeadSigMaskCase1 0x04
#define dec128__LeadSigMaskCase2 0x1C

#ifdef WORDS_BIGENDIAN

#define dec128__ub0(n, i) ((n)->u[i] >> 24)
#define dec128__ub1(n, i) (((n)->u[i] >> 16) & 0xFF)
#define dec128__ub2(n, i) (((n)->u[i] >> 8) & 0xFF)
#define dec128__ub3(n, i) ((n)->u[i] & 0xFF)

#else

#define dec128__ub0(n, i) ((n)->u[i] & 0xFF)
#define dec128__ub1(n, i) (((n)->u[i] >> 8) & 0xFF)
#define dec128__ub2(n, i) (((n)->u[i] >> 16) & 0xFF)
#define dec128__ub3(n, i) ((n)->u[i] >> 24)

#endif

#define dec128__DigitCountPerDeclet 3
#define dec128__DecletCount \
    ((dec128_Precision - 1) / dec128__DigitCountPerDeclet)

#define dec128__isSigCase1(n) \
    ((dec128__ub0(n, 0) & dec128__G1G2Mask) == dec128__G1G2Mask)

static uint32_t dec128__clength(dec128 *n);

static int32_t getB00(dec128 *n)
{
    return dec128__ub0(n, 0);
}

static int32_t getB01(dec128 *n)
{
    return dec128__ub1(n, 0);
}

static int32_t getB02(dec128 *n)
{
    return dec128__ub2(n, 0);
}

static int32_t getB03(dec128 *n)
{
    return dec128__ub3(n, 0);
}

static int32_t getB10(dec128 *n)
{
    return dec128__ub0(n, 1);
}

static int32_t getB11(dec128 *n)
{
    return dec128__ub1(n, 1);
}

static int32_t getB12(dec128 *n)
{
    return dec128__ub2(n, 1);
}

static int32_t getB13(dec128 *n)
{
    return dec128__ub3(n, 1);
}

static int32_t getC00(dec128c *n)
{
    return n->b[0];
}

static int32_t getC01(dec128c *n)
{
    return n->b[1];
}

bool dec128_isZero(dec128 *n)
{
    return dec128_isFinite(n)
        && dec128__isSigCase1(n)
        && dec128__ub(n, 0) & dec128__LeadSigMaskCase1 == 0
        && dec128__ub(n, 2) & dec128__TrailSigMask == 0
        && dec128__ub(n, 3) == 0
        && n->u[1] == 0 && n->u[2] == 0 && n->u[3] == 0;
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

+------------------+-----------------+------------------+-----------------+
|    unit[0]       |     unit[1]     |      unit[2]     |     unit[3]     |
+------------------+-----------------+------------------+-----------------+
|0                0|0               0|0                0|0               1|
|0                3|3               6|6                9|9               2|
|0                1|2               3|4                5|6               7|
+------------------+-----------------+------------------+-----------------+
byte      2     3     0  1  2    3      0   1    2   3     0    1    2  3
|      |0  0|0    0|0  0|00|00|0    0|0  0|0  0|0 0|0  0|0   1|1  1|1 1|11|
|      |1  2|2    3|3  3|44|45|5    6|6  7|7  7|8 8|8  9|9   0|0  1|1 1|22|
|      |6  3|4    1|2  9|07|85|6    3|4  1|2  9|0 7|8  5|6   3|4  1|2 9|07|

declet      0     1     2     3    4      5     6     7      8    9    10
|         |0 0|0     0|0  0|0  0|0    0|0   0|0   0|0     0|0  1|1  1|1  1|
|         |1 2|2     3|3  4|4  5|5    6|6   7|7   8|8     9|9  0|0  1|1  2|
|         |8 7|8     7|8  7|8  7|8    7|8   7|8   7|8     7|8  7|8  7|8  7|
*/

static int32_t dec128__getDeclet(dec128 *n, uint32_t offset)
{
    switch (offset) {
    case 0:
        return (dec128__ub2(n, 0) & 0x3F) << 4 | (dec128__ub3(n, 0) >> 4);
    case 1:
        return (dec128__ub2(n, 0) & 0x0F) << 2 | (dec128__ub0(n, 1) >> 2);
    case 2:
        return (dec128__ub0(n, 1) & 0x03) << 8 | dec128__ub1(n, 1);
    case 3:
        return dec128__ub2(n, 1) << 6 | dec128__ub3(n, 1) >> 6;
    case 4:
        return (dec128__ub3(n, 1) & 0x3F) << 4 | (dec128__ub0(n, 2) >> 4);
    case 5:
        return (dec128__ub0(n, 2) & 0x0F) << 2 | (dec128__ub1(n, 2) >> 2);
    case 6:
        return (dec128__ub1(n, 2) & 0x03) << 8 | dec128__ub2(n, 2);
    case 7:
        return dec128__ub3(n, 2) << 6 | dec128__ub0(n, 3) >> 6;
    case 8:
        return (dec128__ub0(n, 3) & 0x3F) << 4 | (dec128__ub1(n, 3) >> 4);
    case 9:
        return (dec128__ub1(n, 3) & 0x0F) << 2 | (dec128__ub2(n, 3) >> 2);
    case 10:
        return (dec128__ub2(n, 3) & 0x03) << 8 | dec128__ub3(n, 3);
    default:
        return -1;
    }
}

static uint32_t dec128__clength(dec128 *n)
{
    int32_t i;
    int32_t j;
    int32_t declet;
    int32_t digit[dec128__DigitCountPerDeclet];

    if (dec128__isSigCase1(n)
        || (dec128__ub0(n, 0) & dec128__LeadSigMaskCase2) != 0
    ) {
        return dec128_Precision;
    }

    for (i = 0; i < dec128__DecletCount; ++i) {
        declet = dec128__getDeclet(n, i);
        if (declet != 0) {
            decimal__decodeDPD(digit, declet);
            for (j = 0; j < dec128__DigitCountPerDeclet; ++j) {
                if (digit[j] != 0) {
                    return dec128_Precision
                        - (i * dec128__DigitCountPerDeclet + j);
                }
            }
        }
    }
    return 1;
}

static int32_t dec128__getExponent(dec128 *n)
{
    int32_t leadBits = dec128__isSigCase1(n) 
        ? dec128__ub0(n, 0) >> 3 & 0x3
        : dec128__ub0(n, 0) >> 5 & 0x3;
    int32_t b1 = (dec128__ub0(n, 0) & 0x3);
    int32_t b2 = dec128__ub1(n, 0);
    int32_t b3 = dec128__ub2(n, 0);
    int32_t e = (leadBits << 12
        | (dec128__ub0(n, 0) & 0x3) << 10
        | dec128__ub1(n, 0) << 2
        | dec128__ub2(n, 0) >> 6);

    return (leadBits << 12
        | (dec128__ub0(n, 0) & 0x3) << 10
        | dec128__ub1(n, 0) << 2
        | dec128__ub2(n, 0) >> 6)
        - dec128_Bias;
}

/**
 * a20780
 * 10100010 00000111 10000000
 *  01   10 00000111 10
 *  01 10 0001 1110
 *
 */
/*
+-+--------+--------------------------------------------------------------+
|S|   G    |                            T                                 |
+-+--------+--------------------------------------------------------------+
|0|0      0|0                                                            1|
|0|0      1|1                                                            2|
|0|1      7|8                                                            7|
+-+--------+--------------------------------------------------------------+

+------------------+-----------------+------------------+-----------------+
|    unit[0]       |     unit[1]     |      unit[2]     |     unit[3]     |
+------------------+-----------------+------------------+-----------------+
|0                0|0               0|0                0|0               1|
|0                3|3               6|6                9|9               2|
|0                1|2               3|4                5|6               7|
+------------------+-----------------+------------------+-----------------+
byte      2     3     0  1  2    3      0   1    2   3     0    1    2  3
|00|0 0|0  0|0    0|0  0|00|00|0    0|0  0|0  0|0 0|0  0|0   1|1  1|1 1|11|
|00|0 1|1  2|2    3|3  3|44|45|5    6|6  7|7  7|8 8|8  9|9   0|0  1|1 1|22|
|07|8 5|6  3|4    1|2  9|07|85|6    3|4  1|2  9|0 7|8  5|6   3|4  1|2 9|07|
*/
static uint32_t dec128__getLeadDigit(dec128 *n)
{
    return dec128__isSigCase1(n) 
        ? 8 | dec128__ub0(n, 0) >> 2 & 0x1
        : dec128__ub0(n, 0) >> 2 & 0x7;
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
    result->kind = dec128_isFinite(n) ? dec128__kindFinite
        : dec128_isInfinite(n) ? dec128__kindInfinite
        : dec128_isSignaling(n) ? dec128__kindSNaN
        : dec128__kindQNaN;

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
    char *dest = (char *)n->u;
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
        p = strchr(decimal_HexDigits, hi);
        if (!p) {
            return false;
        }
        hi = p - decimal_HexDigits;

        lo = toupper(hex[i + 1]);
        p = strchr(decimal_HexDigits, lo);
        if (!p) {
            return false;
        }
        lo = p - decimal_HexDigits;

        *dest++ = hi << 4 | lo;
    }
}
