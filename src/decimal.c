#include "decimalInt.h"

static char *decimal_sprintDigits(char *buf, uint8_t *digits, uint32_t offset,
    uint32_t count);
static char *decimal_padZero(char *buf, uint32_t count);
static char *decimal_strncpy(char *buf, const char *source, uint32_t count);
static char *decimal_sprintInt(char *buf, int32_t value);

void decimal_init(decimal *number, uint32_t capacity)
{
    uint32_t i;

    decimal_setFlags(number, 0);
    decimal_setSign(number, DECIMAL_SIGN_PLUS);
    decimal_setKind(number, DECIMAL_KIND_FINITE);
    decimal_setExponent(number, 0);
    decimal_setCapacity(number, capacity);
    decimal_setLength(number, 1);
    for (i = 0; i < capacity; ++i) {
        decimal_setSignificandDigit(number, i, 0);
    }
}

bool decimal_isSignMinus(decimal *number)
{
    return decimal_getSign(number) == DECIMAL_SIGN_MINUS;
}

bool decimal_isNaN(decimal *number)
{
    return decimal_isSNaN(number) || decimal_isQNaN(number);
}

bool decimal_isSNaN(decimal *number)
{
    return decimal_getKind(number) == DECIMAL_KIND_SNAN;
}

bool decimal_isQNaN(decimal *number)
{
    return decimal_getKind(number) == DECIMAL_KIND_QNAN;
}

bool decimal_isInfinite(decimal *number)
{
    return decimal_getKind(number) == DECIMAL_KIND_INFINITY;
}

bool decimal_isFinite(decimal *number)
{
    return decimal_getKind(number) == DECIMAL_KIND_FINITE;
}

bool decimal_isZero(decimal *number)
{
    uint32_t i;

    for (i = 0; i < decimal_getLength(number); ++i) {
        if (decimal_getSignificandDigit(number, i) != 0) {
            return false;
        }
    }
    return true;
}

void decimal_setZero(decimal *number)
{
    decimal_setSign(number, DECIMAL_SIGN_PLUS);
    decimal_setKind(number, DECIMAL_KIND_FINITE);
    decimal_setExponent(number, 0);
    decimal_setLength(number, 1);
    decimal_setSignificandDigit(number, 0, 0);
}

void decimal_setInfinite(decimal *number, uint32_t sign)
{
    decimal_setSign(number, sign);
    decimal_setKind(number, DECIMAL_KIND_INFINITY);
    decimal_setExponent(number, 0);
    decimal_setLength(number, 1);
    decimal_setSignificandDigit(number, 0, 0);
}

void decimal_setSNaN(decimal *number)
{
    decimal_setSign(number, DECIMAL_SIGN_PLUS);
    decimal_setKind(number, DECIMAL_KIND_SNAN);
    decimal_setExponent(number, 0);
    decimal_setLength(number, 1);
    decimal_setSignificandDigit(number, 0, 0);
}

void decimal_setQNaN(decimal *number)
{
    decimal_setSign(number, DECIMAL_SIGN_PLUS);
    decimal_setKind(number, DECIMAL_KIND_QNAN);
    decimal_setExponent(number, 0);
    decimal_setLength(number, 1);
    decimal_setSignificandDigit(number, 0, 0);
}

void decimal_negate(decimal *result, decimal *number)
{
    decimal_copy(result, number);
    decimal_flipSign(result);
}

void decimal_copy(decimal *result, decimal *number)
{
    if (result != number) {
        *result = *number;
    }
}

char *decimal_convertToDecimalCharacter(decimal *number)
{
}

void decimal_convertToDecimalNonExponential(char *result, decimal *number)
{
    char *p;
    uint32_t kind;
    uint32_t length;
    int32_t exponent;
    int32_t pos;

    p = result;
    if (decimal_isSignMinus(number)) {
        *p++ = '-';
    }

    kind = decimal_getKind(number);
    switch (kind) {
    case DECIMAL_KIND_FINITE:
        length = decimal_getLength(number);
        exponent = decimal_getExponent(number);
        pos = 1 + exponent;
// 1.230E+4 = 12300
// 1.230E+3 = 1230
// 1.230E+2 = 123.0
// 1.230E+1 = 12.30
// 1.230E+0 = 1.230
// 1.230E-1 = 0.1230
// 1.230E-2 = 0.01230
// 1.230E-3 = 0.001230
// 0.1230E+0 = 0.1230
// 0.01230E+0 = 0.01230
// 0.01230E-1 = 0.001230
        if (pos > 0) {
            if (pos < length) {
                p = decimal_sprintDigits(p, number->significand, 0, pos);
                *p++ = '.';
                p = decimal_sprintDigits(p, number->significand, pos,
                    length - pos);
            } else if (pos > length) {
                p = decimal_sprintDigits(p, number->significand, 0, length);
                p = decimal_padZero(p, pos - length);
            } else {
                p = decimal_sprintDigits(p, number->significand, 0, length);
            }
        } else {
            *p++ = '0';
            *p++ = '.';
            if (pos < 0) {
                p = decimal_padZero(p, -pos);
            }
            p = decimal_sprintDigits(p, number->significand, 0, length);
        }
        break;
    case DECIMAL_KIND_INFINITY:
        p = decimal_strncpy(p, DECIMAL_STR_INFINITY,
            sizeof(DECIMAL_STR_INFINITY) - 1);
        break;
    case DECIMAL_KIND_QNAN:
        p = decimal_strncpy(p, DECIMAL_STR_NAN, sizeof(DECIMAL_STR_NAN) - 1);
        length = decimal_getLength(number);
        if (length > 0) {
            p = decimal_sprintDigits(p, number->significand, 0, length);
        }
        break;
    case DECIMAL_KIND_SNAN:
        p = decimal_strncpy(p, DECIMAL_STR_SNAN, sizeof(DECIMAL_STR_SNAN) - 1);
        length = decimal_getLength(number);
        if (length > 0) {
            p = decimal_sprintDigits(p, number->significand, 0, length);
        }
        break;
    }
    *p = '\0';
}

uint32_t decimal_getLengthOfDecimalNonExponential(decimal *number)
{
    uint32_t kind;
    uint32_t length;
    uint32_t digit_count;
    int32_t exponent;
    int32_t pos;

    length = decimal_isSignMinus(number) ? sizeof("-") - 1 : 0;
    kind = decimal_getKind(number);
    switch (kind) {
    case DECIMAL_KIND_FINITE:
        digit_count = decimal_getLength(number);
        length += digit_count;
        exponent = decimal_getExponent(number);
        pos = 1 + exponent;
// 1.230E+4 = 12300
// 1.230E+3 = 1230
// 1.230E+2 = 123.0
// 1.230E+1 = 12.30
// 1.230E+0 = 1.230
// 1.230E-1 = 0.1230
// 1.230E-2 = 0.01230
// 0.1230E+0 = 0.1230
// 0.01230E+0 = 0.01230
// 0.01230E-1 = 0.001230
        if (pos > 0) {
            if (pos < digit_count) {
                length += sizeof(".") - 1;
            } else if (pos > length) {
                length += pos - length;
            }
        } else {
            length += sizeof("0.") - 1 - pos;
        }
        break;
    case DECIMAL_KIND_INFINITY:
        length += sizeof(DECIMAL_STR_INFINITY) - 1;
        break;
    case DECIMAL_KIND_QNAN:
        length += (sizeof(DECIMAL_STR_NAN) - 1) + decimal_getLength(number);
        break;
    case DECIMAL_KIND_SNAN:
        length += (sizeof(DECIMAL_STR_SNAN) - 1) + decimal_getLength(number);
        break;
    }
    return length;
}

void decimal_convertToDecimalExponential(char *result, decimal *number)
{
    char *p;
    uint32_t kind;
    uint32_t length;
    int32_t exponent;

    p = result;
    if (decimal_isSignMinus(number)) {
        *p++ = '-';
    }

    kind = decimal_getKind(number);
    switch (kind) {
    case DECIMAL_KIND_FINITE:
        *p++ = DECIMAL_DIGITS[decimal_getSignificandDigit(number, 0)];
        length = decimal_getLength(number);
        if (length > 1) {
            *p++ = '.';
            p = decimal_sprintDigits(p, number->significand, 1, length - 1);
        }
        *p++ = 'E';
        exponent = decimal_getExponent(number);
        if (exponent >= 0) {
            *p++ = '+';
        }
        p = decimal_sprintInt(p, exponent);
        break;
    case DECIMAL_KIND_INFINITY:
        p = decimal_strncpy(p, DECIMAL_STR_INFINITY,
            sizeof(DECIMAL_STR_INFINITY) - 1);
        break;
    case DECIMAL_KIND_QNAN:
        p = decimal_strncpy(p, DECIMAL_STR_NAN, sizeof(DECIMAL_STR_NAN) - 1);
        length = decimal_getLength(number);
        if (length > 0) {
            p = decimal_sprintDigits(p, number->significand, 0, length);
        }
        break;
    case DECIMAL_KIND_SNAN:
        p = decimal_strncpy(p, DECIMAL_STR_SNAN, sizeof(DECIMAL_STR_SNAN) - 1);
        length = decimal_getLength(number);
        if (length > 0) {
            p = decimal_sprintDigits(p, number->significand, 0, length);
        }
        break;
    }
    *p = '\0';
}

uint32_t decimal_getLengthOfDecimalExponential(decimal *number)
{
    uint32_t kind;
    uint32_t length;
    uint32_t digit_count;

    length = decimal_isSignMinus(number) ? sizeof("-") - 1 : 0;
    kind = decimal_getKind(number);
    switch (kind) {
    case DECIMAL_KIND_FINITE:
        digit_count = decimal_getLength(number);
        if (digit_count > 1) {
            length += sizeof(".") - 1;
        }
        length += digit_count + (sizeof("E") - 1)
            + (sizeof("+") - 1)
            + decimal_countDigitOfInt(decimal_getExponent(number));
        break;
    case DECIMAL_KIND_INFINITY:
        length += sizeof(DECIMAL_STR_INFINITY) - 1;
        break;
    case DECIMAL_KIND_QNAN:
        length += (sizeof(DECIMAL_STR_NAN) - 1) + decimal_getLength(number);
        break;
    case DECIMAL_KIND_SNAN:
        length += (sizeof(DECIMAL_STR_SNAN) - 1) + decimal_getLength(number);
        break;
    }
    return length;
}

static char *decimal_sprintDigits(char *buf, uint8_t *digits, uint32_t offset,
    uint32_t count)
{
    char *p;
    uint32_t i;

    p = buf;
    for (i = 0; i < count; ++i) {
        *p++ = DECIMAL_DIGITS[decimal_getDigit(digits, i + offset)];
    }
    return p;
}

static char *decimal_padZero(char *buf, uint32_t count)
{
    for (; count > 0; --count) {
        *buf++ = '0';
    }
    return buf;
}

uint32_t decimal_countDigitOfInt(int32_t value)
{
    uint32_t count;

    if (value == 0) {
        return 1;
    }

    count = 0;
    for (; value; value /= 10) {
        ++count;
    }
    return count;
}

static char *decimal_sprintInt(char *buf, int32_t value)
{
    char *p;
    uint32_t count;
    uint32_t i;
    int32_t rest;

    p = buf;
    if (value < 0) {
        *p++ = '-';
    }

    count = decimal_countDigitOfInt(value);
    rest = value < 0 ? -value : value;
    for (i = 0; i < count; ++i) {
        p[count - 1 - i] = DECIMAL_DIGITS[rest % 10];
        rest /= 10;
    }
    return p + count;
}

static char *decimal_strncpy(char *buf, const char *source, uint32_t count)
{
    uint32_t i;

    for (i = 0; i < count; ++i) {
        *buf++ = *source++;
    }
    return buf;
}
