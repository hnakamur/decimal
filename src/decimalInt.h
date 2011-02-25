#ifndef _DECIMAL_INT_H
#define _DECIMAL_INT_H 1

#include "decimal.h"

#define decimal__Radix 10
#define decimal__Digits "0123456789"
#define decimal__HexDigits "0123456789ABCDEF"
#define decimal__DigitCountPerDeclet 3

#define decimal_NaNLiteral "NaN"
#define decimal_sNaNLiteral "sNaN"
#define decimal_InfinityLiteral "Infinity"

#define decimal_setMinusSign(n) ((n)->flags |= decimal_MinusSignBit)
#define decimal_resetMinusSign(n) ((n)->flags &= ~decimal_MinusSignBit)

#define decimal_setKind(n, k) \
    ((n)->flags = ((n)->flags & ~decimal_KindBitMask) | (k))
#define decimal_getKind(n) ((n)->flags & decimal_KindBitMask)

#define decimal_setSignaling(n) ((n)->flags |= decimal_SignalingBit)
#define decimal_resetSignaling(n) ((n)->flags &= ~decimal_SignalingBit)

#define decimal_setExponent(n, e) ((n)->exponent = (e))
#define decimal_getExponent(n) ((n)->exponent)

#define decimal_setCapacity(n, c) ((n)->capacity = (c))
#define decimal_getCapacity(n) ((n)->capacity)

#define decimal_setLength(n, l) ((n)->length = (l))
#define decimal_getLength(n) ((n)->length)

#define decimal_setSignificandDigit(n, pos, d) \
    decimal_setDigit((n)->significand, pos, d)
#define decimal_getSignificandDigit(n, pos) \
    decimal_getDigit((n)->significand, pos)

void decimal_setDigit(uint8_t *digits, uint32_t pos, uint8_t digit);
uint8_t decimal_getDigit(const uint8_t *digits, uint32_t pos);

uint32_t decimal_countDigitOfInt(int32_t value);
bool decimal_convertFromDecimalCharacter(decimal *number, const char *str);

void decimal_setZero(decimal *number);
void decimal_setInfinite(decimal *number);
void decimal_setSNaN(decimal *number);
void decimal_setQNaN(decimal *number);

void decimal_round(decimal *result, const decimal *number,
    DecimalContext *context);

int32_t decimal__encodeDPD(const int32_t *digit);
void decimal__decodeDPD(int32_t *digit, int32_t dpd);

#endif /* _DECIMAL_INT_H */
