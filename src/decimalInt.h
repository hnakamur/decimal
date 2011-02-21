#ifndef _DECIMAL_INT_H
#define _DECIMAL_INT_H 1

#include "decimal.h"

#define DECIMAL_DIGITS "0123456789"

#define decimal_setFlags(n, e) ((n)->flags = (e))
#define decimal_getFlags(n) ((n)->flags)

#define decimal_setSign(n, s) \
    ((n)->flags = ((s) \
        ? ((n)->flags | DECIMAL_MINUS_SIGN_BIT) \
        : ((n)->flags & ~DECIMAL_MINUS_SIGN_BIT)))
#define decimal_getSign(n) ((n)->flags & DECIMAL_MINUS_SIGN_BIT)
#define decimal_flipSign(n) ((n)->flags ^= DECIMAL_MINUS_SIGN_BIT)

#define decimal_setKind(n, k) \
    ((n)->flags = ((n)->flags & ~DECIMAL_KIND_BIT_MASK) | (k))
#define decimal_getKind(n) ((n)->flags & DECIMAL_KIND_BIT_MASK)

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

void decimal_round(decimal *result, const decimal *number,
    DecimalContext *context);

#endif /* _DECIMAL_INT_H */
