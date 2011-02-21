#ifndef _DECIMAL_H_
#define _DECIMAL_H_ 1

#include <stdint.h>
#include "bool.h"
#include "DecimalContext.h"

#define decimal_DefaultCapacity 40
#define decimal_getUnitCount(capacity) (((capacity) + 1) / 2)

typedef struct {
    uint32_t flags;
    int32_t exponent;
    uint32_t capacity; /* significand capacity in digit count. */
    uint32_t length;   /* significand length in digit count. */
    uint8_t significand[decimal_getUnitCount(decimal_DefaultCapacity)];
} decimal;

#define decimal_MinusSignBit 0x00000001

#define decimal_isSignMinus(n) (((n)->flags & decimal_MinusSignBit) != 0)

#define decimal_KindBitMask  0x00000006
#define decimal_KindFinite   0x00000000
#define decimal_KindInfinite 0x00000002
#define decimal_KindNaN      0x00000004

#define decimal_SignalingBit 0x00000008

#define decimal_isFinite(n) \
    (((n)->flags & decimal_KindBitMask) == decimal_KindFinite)
#define decimal_isInfinite(n) \
    (((n)->flags & decimal_KindBitMask) == decimal_KindInfinite)
#define decimal_isNaN(n) \
    (((n)->flags & decimal_KindBitMask) == decimal_KindNaN)
#define decimal_isSignaling(n) (((n)->flags & decimal_SignalingBit) != 0)

bool decimal_isNormal(const decimal *number);
bool decimal_isSubnormal(const decimal *number);
bool decimal_isZero(const decimal *number);

void decimal_init(decimal *number, uint32_t capacity);

void decimal_negate(decimal *result, const decimal *number);
void decimal_copy(decimal *result, const decimal *number);
void decimal_copySign(decimal *result, const decimal *number);

#endif /* _DECIMAL_H_ */
