#include "decimalInt.h"

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
        decimal_setDigit(number, i, 0);
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
    return decimal_getKind(number) == DECIMAL_KIND_INFINITE;
}

bool decimal_isFinite(decimal *number)
{
    return decimal_getKind(number) == DECIMAL_KIND_FINITE;
}

bool decimal_isZero(decimal *number)
{
    uint32_t i;

    for (i = 0; i < decimal_getLength(number); ++i) {
        if (decimal_getDigit(number, i) != 0) {
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
    decimal_setDigit(number, 0, 0);
}

void decimal_setInfinite(decimal *number, uint32_t sign)
{
    decimal_setSign(number, sign);
    decimal_setKind(number, DECIMAL_KIND_INFINITE);
    decimal_setExponent(number, 0);
    decimal_setLength(number, 1);
    decimal_setDigit(number, 0, 0);
}

void decimal_setSNaN(decimal *number)
{
    decimal_setSign(number, DECIMAL_SIGN_PLUS);
    decimal_setKind(number, DECIMAL_KIND_SNAN);
    decimal_setExponent(number, 0);
    decimal_setLength(number, 1);
    decimal_setDigit(number, 0, 0);
}

void decimal_setQNaN(decimal *number)
{
    decimal_setSign(number, DECIMAL_SIGN_PLUS);
    decimal_setKind(number, DECIMAL_KIND_QNAN);
    decimal_setExponent(number, 0);
    decimal_setLength(number, 1);
    decimal_setDigit(number, 0, 0);
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

