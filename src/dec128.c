#include <stdio.h>
#include <string.h>
#include "decimalInt.h"

#define dec128__G1G2Mask     0x60
#define dec128__TrailSigMask 0x3F

#define dec128__DecletCount \
    ((dec128_Precision - 1) / decimal__DigitCountPerDeclet)

#define dec128__isSigCase1(n) \
    (((n)->byte[0] & dec128__G1G2Mask) == dec128__G1G2Mask)

static int32_t dec128__getQ(const dec128 *n);
#define dec128__getE(n) (dec128__getQ(n) + dec128_Precision - 1)

static uint32_t dec128__getLeadDigit(const dec128 *n);

static int32_t dec128__getDeclet(const dec128 *n, uint32_t offset);
static void dec128__setDeclet(dec128 *x, uint32_t offset, int32_t declet);

static void dec128__wk_convertToDec128(dec128 *result, const dec128__wk *x);

static void dec128__wk_convertToExponentialDecimalNotation(char *result,
    const dec128__wk *x);
static void dec128__wk_convertToOrdinaryDecimalNotation(char *result,
    const dec128__wk *x);

bool dec128_is754version1985(void)
{
    return false;
}

bool dec128_is754version2008(void)
{
    return false;
}

static bool dec128__zeroBytesInRange(dec128 *x, int start, int end)
{
    memset(x->byte + start, 0, end - start);
}

static int32_t dec128__getQ(const dec128 *x)
{
    return ((x->byte[0] >> (dec128__isSigCase1(x) ? 3 : 5) & 0x3) << 12
        | (x->byte[0] & 0x3) << 10
        | x->byte[1] << 2
        | x->byte[2] >> 6)
        - dec128_Bias;
}

static uint32_t dec128__getLeadDigit(const dec128 *x)
{
    return dec128__isSigCase1(x) 
        ? 8 | x->byte[0] >> 2 & 0x1
        : x->byte[0] >> 2 & 0x7;
}

void dec128__setInfinity(dec128 *x)
{
    x->byte[0] = (x->byte[0] & 0x80) | 0x78;
    dec128__zeroBytesInRange(x, 1, dec128__ByteLength);
}

static void dec128__setPayload(dec128 *x, const char *payload, uint32_t len)
{
    int decletCount;
    int i;
    int offset;
    int32_t digit[3];
    int32_t declet;
    uint32_t pos;

    dec128__zeroBytesInRange(x, 1, dec128__ByteLength);

    if (len == 0) {
        return;
    }

    decletCount = (len + decimal__DigitCountPerDeclet)
        / decimal__DigitCountPerDeclet;
    for (i = 0; i < decletCount; ++i) {
        pos = dec128__DecletCount - 1 - (decletCount - 1 - i);
        offset = len - 3 - (decletCount - 1 - i) * decimal__DigitCountPerDeclet;
        digit[0] = offset < 0 ? 0 : (payload[offset] - '0');
        digit[1] = offset + 1 < 0 ? 0 : (payload[offset + 1] - '0');
        digit[2] = offset + 2 < 0 ? 0 : (payload[offset + 2] - '0');
        declet = decimal__encodeDecletFromDigits(digit);
        dec128__setDeclet(x, pos, declet);
    }
}

void dec128__setQNaN(dec128 *x, const char *payload)
{
    uint32_t len = payload ? strlen(payload) : 0;
    x->byte[0] = (x->byte[0] & 0x80) | 0x7C;
    dec128__setPayload(x, payload, len);
}

void dec128__setSNaN(dec128 *x, const char *payload)
{
    uint32_t len = payload ? strlen(payload) : 0;
    x->byte[0] = (x->byte[0] & 0x80) | 0x7E;
    dec128__setPayload(x, payload, len);
}

void dec128__setQAndLeadDigit(dec128 *x, int32_t q, uint32_t leadDigit)
{
    int32_t biasedQ = q + dec128_Bias;
    int32_t expMsb = biasedQ >> 12;
    x->byte[0] = (x->byte[0] & 0x80)
        | (leadDigit < 8
            ? expMsb << 5 | leadDigit
            : 0x60 | expMsb << 3 | leadDigit & 0x1)
        | biasedQ >> 10 & 0x03;
    x->byte[1] = biasedQ >> 2 & 0xFF;
    x->byte[2] = (biasedQ & 0x03) << 6 | x->byte[2] & 0x3F;
}

static bool dec128__hasAllZeroBytesInRange(const dec128 *x, int start, int end)
{
    int i;

    for (i = start; i < end; ++i) {
        if (x->byte[i] != 0) {
            return false;
        }
    }
    return true;
}

static bool dec128__hasAllZeroTrailingDigits(const dec128 *x)
{
    return (x->byte[2] & dec128__TrailSigMask) == 0
        && dec128__hasAllZeroBytesInRange(x, 3, dec128__ByteLength);
}

bool dec128_isCanonical(const dec128 *x)
{
    int32_t i;
    int32_t declet;

    for (i = 0; i < dec128__DecletCount; ++i) {
        declet = dec128__getDeclet(x, i);
        if (!decimal__isCanonicalDeclet(declet)) {
            return false;
        }
    }
    return true;
}

decimal_Class dec128_getClass(const dec128 *x)
{
    decimal__Sign sign = dec128_isSignMinus(x)
        ? decimal__SignMinus : decimal__SignPlus;
    return decimal__getClassFromSignAndKind(sign, dec128__getKind(x));
}

decimal__Kind dec128__getKind(const dec128 *x)
{
    if (dec128_isFinite(x)) {
        if (dec128__getLeadDigit(x) == 0
            && dec128__hasAllZeroTrailingDigits(x)
        ) {
            return decimal__KindZero;
        } else if (dec128__getE(x) <= dec128_EMin
            && dec128__getLeadDigit(x) == 0
        ) {
            return decimal__KindSubnormal;
        } else {
            return decimal__KindNormal;
        }
    } else if (dec128_isInfinite(x)) {
        return decimal__KindInfinity;
    } else if (dec128_isSignaling(x)) {
        return decimal__KindSNaN;
    } else if (dec128_isNaN(x)) {
        return decimal__KindQNaN;
    }
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

static int32_t dec128__getDeclet(const dec128 *x, uint32_t offset)
{
    switch (offset) {
    case 0:
        return (x->byte[2] & 0x3F) << 4 | (x->byte[3] >> 4);
    case 1:
        return (x->byte[3] & 0x0F) << 2 | (x->byte[4] >> 2);
    case 2:
        return (x->byte[4] & 0x03) << 8 | x->byte[5];
    case 3:
        return x->byte[6] << 6 | x->byte[7] >> 6;
    case 4:
        return (x->byte[7] & 0x3F) << 4 | (x->byte[8] >> 4);
    case 5:
        return (x->byte[8] & 0x0F) << 2 | (x->byte[9] >> 2);
    case 6:
        return (x->byte[9] & 0x03) << 8 | x->byte[10];
    case 7:
        return x->byte[11] << 6 | x->byte[12] >> 6;
    case 8:
        return (x->byte[12] & 0x3F) << 4 | (x->byte[13] >> 4);
    case 9:
        return (x->byte[13] & 0x0F) << 2 | (x->byte[14] >> 2);
    case 10:
        return (x->byte[14] & 0x03) << 8 | x->byte[15];
    }
}

static void dec128__setDeclet(dec128 *x, uint32_t offset, int32_t declet)
{
    switch (offset) {
    case 0:
        x->byte[2] = (x->byte[2] & 0xC0) | declet >> 4;
        x->byte[3] = (x->byte[3] & 0x0F) | (declet & 0xF) << 4;
        break;
    case 1:
        x->byte[3] = (x->byte[3] & 0xF0) | declet >> 6;
        x->byte[4] = (x->byte[4] & 0x03) | (declet & 0x3F) << 2;
        break;
    case 2:
        x->byte[4] = (x->byte[4] & 0xFC) | declet >> 8;
        x->byte[5] = declet & 0xFF;
        break;
    case 3:
        x->byte[6] = declet >> 2;
        x->byte[7] = (x->byte[7] & 0x3F) | (declet & 0x03) << 6;
        break;
    case 4:
        x->byte[7] = (x->byte[7] & 0xC0) | declet >> 4;
        x->byte[8] = (x->byte[8] & 0x0F) | (declet & 0xF) << 4;
        break;
    case 5:
        x->byte[8] = (x->byte[8] & 0xF0) | declet >> 6;
        x->byte[9] = (x->byte[9] & 0x03) | (declet & 0x3F) << 2;
        break;
    case 6:
        x->byte[9] = (x->byte[9] & 0xFC) | declet >> 8;
        x->byte[10] = declet & 0xFF;
        break;
    case 7:
        x->byte[11] = declet >> 2;
        x->byte[12] = (x->byte[12] & 0x3F) | (declet & 0x03) << 6;
        break;
    case 8:
        x->byte[12] = (x->byte[12] & 0xC0) | declet >> 4;
        x->byte[13] = (x->byte[13] & 0x0F) | (declet & 0xF) << 4;
        break;
    case 9:
        x->byte[13] = (x->byte[13] & 0xF0) | declet >> 6;
        x->byte[14] = (x->byte[14] & 0x03) | (declet & 0x3F) << 2;
        break;
    case 10:
        x->byte[14] = (x->byte[14] & 0xFC) | declet >> 8;
        x->byte[15] = declet & 0xFF;
        break;
    }
}

bool dec128__initWithRawHexString(dec128 *x, const char *hex)
{
    int len = strlen(hex);
    uint8_t *dest = x->byte;
    int i;
    int hi;
    int lo;
    char *p;

    if (len != dec128__ByteLength * 2) {
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

void dec128__convertToRawHexString(char *result, const dec128 *x)
{
    int i;
    uint8_t byte;

    for (i = 0; i < dec128__ByteLength; ++i) {
        byte = x->byte[i];
        *result++ = decimal__HexDigits[byte >> 4];
        *result++ = decimal__HexDigits[byte & 0xF];
    }
}

void dec128_convertFromDecimalString(dec128 *result, const char *str,
    DecimalContext *ctx)
{
    dec128__wk w;

    if (!dec128__wk_convertFromDecimalCharacter(&w, str)) {
        return;
    }
    dec128__wk_convertToDec128(result, &w);
}

void dec128_convertToExponentialDecimalString(char *buf, const dec128 *x,
    DecimalContext *ctx)
{
    dec128__wk w;

    dec128__wk_convertFromDec128(&w, x);
    dec128__wk_convertToExponentialDecimalNotation(buf, &w);
}

void dec128_convertToOrdinaryDecimalString(char *buf, const dec128 *x,
    DecimalContext *ctx)
{
    dec128__wk w;

    dec128__wk_convertFromDec128(&w, x);
    dec128__wk_convertToOrdinaryDecimalNotation(buf, &w);
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

void dec128__wk_convertFromDec128(dec128__wk *result, const dec128 *x)
{
    uint32_t i;
    int32_t declet;
    int32_t digit[3];
    uint32_t pos;

    result->sign = dec128_isSignMinus(x)
        ? decimal__SignMinus : decimal__SignPlus;
    result->kind = dec128__getKind(x);
    result->q = dec128__getQ(x);

    pos = 0;
    dec128__wk_setDigit(result, pos++, dec128__getLeadDigit(x));
    for (i = 0; i < dec128__DecletCount; ++i) {
        declet = dec128__getDeclet(x, i);
        decimal__decodeDecletToDigits(digit, declet);
        dec128__wk_setDigit(result, pos++, digit[0]);
        dec128__wk_setDigit(result, pos++, digit[1]);
        dec128__wk_setDigit(result, pos++, digit[2]);
    }
    result->c[dec128_Precision] = '\0';
}

decimal_Class dec128__wk_getClass(const dec128__wk *x)
{
    return decimal__getClassFromSignAndKind(x->sign, x->kind);
}

static void dec128__wk_convertToDec128(dec128 *result, const dec128__wk *x)
{
    uint32_t i;
    int32_t declet;
    int32_t digit[3];
    uint32_t pos;

    if (x->sign == decimal__SignMinus) {
        dec128__setSignMinus(result);
    } else {
        dec128__resetSignMinus(result);
    }

    switch (x->kind) {
    case decimal__KindNormal:
    case decimal__KindSubnormal:
        pos = 0;
        dec128__setQAndLeadDigit(result, x->q, dec128__wk_getDigit(x, pos++));
        for (i = 0; i < dec128__DecletCount; ++i) {
            digit[0] = dec128__wk_getDigit(x, pos++);
            digit[1] = dec128__wk_getDigit(x, pos++);
            digit[2] = dec128__wk_getDigit(x, pos++);
            declet = decimal__encodeDecletFromDigits(digit);
            dec128__setDeclet(result, i, declet);
        }
        break;
    case decimal__KindInfinity:
        dec128__setInfinity(result);
        break;
    case decimal__KindSNaN:
        dec128__setSNaN(result, x->c);
        break;
    case decimal__KindQNaN:
        dec128__setQNaN(result, x->c);
        break;
    }
}

bool dec128__wk_convertFromDecimalCharacter(dec128__wk *result,
    const char *str)
{
    const char *p;
    const char *start;
    const char *dot;
    const char *sigStart;
    const char *intEnd;
    const char *fracStart;
    int32_t sigLen;
    int32_t intLen;
    int32_t fracLen;
    int32_t exponent;
    decimal__Sign expSign;
    int32_t payloadLen;

    p = str;
    result->sign = decimal__SignPlus;
    if (*p == '+') {
        ++p;
    } else if (*p == '-') {
        result->sign = decimal__SignMinus;
        ++p;
    }

    if (decimal__isDigitChar(*p) || *p == '.') {
        // FIXME: care for subnormal case
        result->kind = decimal__KindNormal;

        memset(result->c, '0', dec128_Precision + 1);

        start = p;
        dot = NULL;

        while (*p == '0') {
            ++p;
        }

        if (*p == '.') {
            dot = p++;

            while (*p == '0') {
                ++p;
            }
// 0.1 -> 1E-1
// 0.001 -> 1E-3
// 0.0012 -> 12E-4
// 0.75 -> 75E-2
// 0.750 -> 750E-3
// 0. -> 0E0
// .0 -> 00E-1
// 0.00 -> 000E-2
// . -> error
            if (decimal__isDigitChar(*p)) {
                sigStart = p;
                while (decimal__isDigitChar(*p)) {
                    ++p;
                }

                result->q = -(p - (dot + 1));
                sigLen = p - sigStart;
                if (sigLen < dec128_Precision) {
                    strncpy(result->c + dec128_Precision - sigLen,
                        sigStart, sigLen);
                } else {
                    // FIXME: do rounding
                    return false;
                }
            } else {
                if (p == start + 1) { // single period and no digit
                    return false;
                }

                result->q = -(p - (dot + 1));
            }
        } else {
            if (decimal__isDigitChar(*p)) {
                sigStart = p++;
                while (decimal__isDigitChar(*p)) {
                    ++p;
                }
                intEnd = p;
                intLen = intEnd - sigStart;

                if (*p == '.') {
                    dot = p++;
                    if (decimal__isDigitChar(*p)) {
                        fracStart = p;
                        while (decimal__isDigitChar(*p)) {
                            ++p;
                        }
                    }
                }

                if (dot == NULL || p == dot + 1) {
// 0 -> 0E0
// 00 -> 0E0
// 00023 -> 23E0
// 00023. -> 23E0
                    strncpy(result->c + dec128_Precision - intLen,
                        sigStart, intLen);
                    result->q = 0;
                } else {
// 00023.0 -> 230E-1
// 00023.4 -> 234E-1
                    fracLen = p - (dot + 1);
                    strncpy(result->c + dec128_Precision - intLen - fracLen,
                        sigStart, intLen);
                    strncpy(result->c + dec128_Precision - fracLen,
                        dot + 1, fracLen);
                    result->q = -fracLen;
                }
            }
        }

        exponent = 0;
        if (*p == 'E' || *p == 'e') {
            ++p;
            expSign = decimal__SignPlus;
            if (*p == '+') {
                ++p;
            } else if (*p == '-') {
                expSign = decimal__SignMinus;
                ++p;
            }

            if (decimal__isDigitString(p)) {
                exponent = atoi(p); // FIXME check range
                if (expSign == decimal__SignMinus) {
                    exponent = -exponent;
                }
                result->q += exponent;
            } else {
                return false;
            }
        }

// 12.3E+0 -> 1.23E+1
    } else if (decimal_strncasematch(p, "INFINITY", "infinity",
            sizeof("infinity") - 1)
    ) {
        if (p[sizeof("infinity") - 1] == '\0') {
            result->kind = decimal__KindInfinity;
        } else {
            return false;
        }
    } else if (decimal_strncasematch(p, "INF", "inf", sizeof("inf") - 1)) {
        if (p[sizeof("inf") - 1] == '\0') {
            result->kind = decimal__KindInfinity;
        } else {
            return false;
        }
    } else if (decimal_strncasematch(p, "NAN", "nan", sizeof("nan") - 1)) {
        p += sizeof("nan") - 1;
        if (*p == '\0') {
            result->kind = decimal__KindQNaN;
        } else if (decimal__isDigitString(p)) {
            result->kind = decimal__KindQNaN;
            payloadLen = strlen(p);
            strncpy(result->c + dec128_Precision - payloadLen, p, payloadLen);
        } else {
            // FIXME set error
        }
    } else if (decimal_strncasematch(p, "SNAN", "snan", sizeof("snan") - 1)) {
        p += sizeof("snan") - 1;
        if (*p == '\0') {
            result->kind = decimal__KindSNaN;
        } else if (decimal__isDigitString(p)) {
            result->kind = decimal__KindSNaN;
            payloadLen = strlen(p);
            strncpy(result->c + dec128_Precision - payloadLen, p, payloadLen);
        } else {
            return false;
        }
    } else {
        return false;
    } 
    return true;
}

static bool dec128__wk_hasAllZeroDigitsInRange(const dec128__wk *x, int start,
    int end)
{
    int i;

    for (i = start; i < end; ++i) {
        if (dec128__wk_getDigit(x, i)) {
            return false;
        }
    }
    return true;
}

static void dec128__wk_convertToOrdinaryDecimalNotation(char *result,
    const dec128__wk *x)
{
    char *p;
    int sigLen;
    int32_t dotPos;

    p = result;
    if (x->sign == decimal__SignMinus) {
        *p++ = '-';
    }

    switch (x->kind) {
    case decimal__KindNormal:
    case decimal__KindSubnormal:
        sigLen = dec128__wk_countSignificantDigit(x);
        dotPos = x->q + dec128_Precision;
// 1230E+1 = 12300
// 1230E+0 = 1230
// 1230E-1 = 123.0
// 1230E-2 = 12.30
// 1230E-3 = 1.230
// 1230E-4 = 0.1230
// 1230E-5 = 0.01230
// 1230E-6 = 0.001230
        if (dotPos > dec128_Precision - sigLen) {
            if (dotPos < dec128_Precision) {
                strncpy(p, x->c + dec128_Precision - sigLen,
                    dotPos - (dec128_Precision - sigLen));
                p += dotPos - (dec128_Precision - sigLen);
                *p++ = '.';
                strncpy(p, x->c + dotPos, dec128_Precision - dotPos);
                p += dec128_Precision - dotPos;
            } else if (dotPos > dec128_Precision) {
                strncpy(p, x->c + dec128_Precision - sigLen, sigLen);
                p += sigLen;
                p = decimal_padZero(p, dotPos - dec128_Precision);
            } else {
                strncpy(p, x->c + dec128_Precision - sigLen, sigLen);
                p += sigLen;
            }
        } else {
            *p++ = '0';
            *p++ = '.';
            if (dotPos < dec128_Precision - sigLen) {
                p = decimal_padZero(p, dec128_Precision - sigLen - dotPos);
            }
            strncpy(p, x->c + dec128_Precision - sigLen, sigLen);
            p += sigLen;
        }
        break;
    case decimal__KindInfinity:
        p = strncpy(p, decimal_InfinityLiteral,
            sizeof(decimal_InfinityLiteral) - 1);
        p += sizeof(decimal_InfinityLiteral) - 1;
        break;
    case decimal__KindQNaN:
        p = strncpy(p, decimal_NaNLiteral, sizeof(decimal_NaNLiteral) - 1);
        p += sizeof(decimal_NaNLiteral) - 1;
        if (!dec128__wk_hasAllZeroDigitsInRange(x, 1, dec128_Precision)) {
            sigLen = dec128__wk_countSignificantDigit(x);
            strncpy(p, x->c + dec128_Precision - sigLen, sigLen);
            p += sigLen;
        }
        break;
    case decimal__KindSNaN:
        p = strncpy(p, decimal_sNaNLiteral, sizeof(decimal_sNaNLiteral) - 1);
        p += sizeof(decimal_sNaNLiteral) - 1;
        if (!dec128__wk_hasAllZeroDigitsInRange(x, 1, dec128_Precision)) {
            sigLen = dec128__wk_countSignificantDigit(x);
            strncpy(p, x->c + dec128_Precision - sigLen, sigLen);
            p += sigLen;
        }
        break;
    }
    *p = '\0';
}

static void dec128__wk_convertToExponentialDecimalNotation(char *result,
    const dec128__wk *x)
{
    char *p;
    uint32_t kind;
    uint32_t length;
    int32_t exponent;
    int sigLen;

    p = result;
    if (x->sign == decimal__SignMinus) {
        *p++ = '-';
    }

    switch (x->kind) {
    case decimal__KindNormal:
    case decimal__KindSubnormal:
//c='0'*31 "750", q=-2 -> 7.50E+0
//c='0'*31 "750", q=-1 -> 7.50E+1
//c='0'*31 "750", q=-5 -> 7.50E-3

        sigLen = dec128__wk_countSignificantDigit(x);
        *p++ = x->c[dec128_Precision - sigLen];
        if (sigLen > 1) {
            *p++ = '.';
            strncpy(p, x->c + dec128_Precision - (sigLen - 1), sigLen - 1);
            p += sigLen - 1;
        }
        *p++ = 'E';
        exponent = x->q + sigLen - 1;
        if (exponent >= 0) {
            *p++ = '+';
        }
        p = decimal__sprintInt(p, exponent);
        break;
    case decimal__KindInfinity:
        p = strncpy(p, decimal_InfinityLiteral,
            sizeof(decimal_InfinityLiteral) - 1);
        p += sizeof(decimal_InfinityLiteral) - 1;
        break;
    case decimal__KindQNaN:
        p = strncpy(p, decimal_NaNLiteral, sizeof(decimal_NaNLiteral) - 1);
        p += sizeof(decimal_NaNLiteral) - 1;
        if (!dec128__wk_hasAllZeroDigitsInRange(x, 1, dec128_Precision)) {
            sigLen = dec128__wk_countSignificantDigit(x);
            strncpy(p, x->c + dec128_Precision - sigLen, sigLen);
            p += sigLen;
        }
        break;
    case decimal__KindSNaN:
        p = strncpy(p, decimal_sNaNLiteral, sizeof(decimal_sNaNLiteral) - 1);
        p += sizeof(decimal_sNaNLiteral) - 1;
        if (!dec128__wk_hasAllZeroDigitsInRange(x, 1, dec128_Precision)) {
            sigLen = dec128__wk_countSignificantDigit(x);
            strncpy(p, x->c + dec128_Precision - sigLen, sigLen);
            p += sigLen;
        }
        break;
    }
    *p = '\0';
}

int dec128__wk_countSignificantDigit(const dec128__wk *x)
{
    int i;

    for (i = 0; i < dec128_Precision; ++i) {
        if (dec128__wk_getDigit(x, i)) {
            return dec128_Precision - i;
        }
    }
    return 1;
}
