#ifndef _DECIMAL_INT_H
#define _DECIMAL_INT_H 1

#include "decimal.h"

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

#define decimal_setDigitInCoef(coef, pos, d) \
    ((coef)[(pos) / 2] = ((pos) % 2 \
        ? ((coef)[(pos) / 2] & 0xf0) | (d) \
        : ((d) << 4) | ((coef)[(pos) / 2] & 0x0f)))
#define decimal_getDigitInCoef(coef, pos) \
    ((pos) % 2 \
        ? (coef)[(pos) / 2] & 0x0f \
        : (coef)[(pos) / 2] >> 4)

#define decimal_setDigit(n, pos, d) \
    decimal_setDigitInCoef((n)->coefficient, pos, d)
#define decimal_getDigit(n, pos) \
    decimal_getDigitInCoef((n)->coefficient, pos)

#endif /* _DECIMAL_INT_H */
