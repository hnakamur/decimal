#ifndef _DECIMAL_H
#define _DECIMAL_H 1

#include <stdint.h>
#include "bool.h"
#include "DecimalContext.h"

#define Decimal_Radix 10

#define DECIMAL_STR_NAN "NaN"
#define DECIMAL_STR_SNAN "sNaN"
#define DECIMAL_STR_INFINITY "Infinity"

#define DECIMAL_SIGN_PLUS 0
#define DECIMAL_SIGN_MINUS 1

#define DECIMAL_MINUS_SIGN_BIT 0x00000001

#define DECIMAL_KIND_BIT_MASK 0x00000006
#define DECIMAL_KIND_FINITE 0x00000000
#define DECIMAL_KIND_INFINITY 0x00000002
#define DECIMAL_KIND_QNAN 0x00000004
#define DECIMAL_KIND_SNAN 0x00000006

#define DECIMAL_DEFAULT_CAPACITY 40
#define DECIMAL_UNIT_COUNT(capacity) (((capacity) + 1) / 2)

typedef struct {
    uint32_t flags;
    int32_t exponent;
    uint32_t capacity; /* significand capacity in digit count. */
    uint32_t length;   /* significand length in digit count. */
    uint8_t significand[DECIMAL_UNIT_COUNT(DECIMAL_DEFAULT_CAPACITY)];
} decimal;

void decimal_init(decimal *number, uint32_t capacity);

bool decimal_isSignMinus(const decimal *number);
bool decimal_isNaN(const decimal *number);
bool decimal_isSNaN(const decimal *number);
bool decimal_isQNaN(const decimal *number);
bool decimal_isInfinite(const decimal *number);
bool decimal_isFinite(const decimal *number);
bool decimal_isZero(const decimal *number);

void decimal_setZero(decimal *number);
void decimal_setInfinite(decimal *number, uint32_t sign);
void decimal_setSNaN(decimal *number);
void decimal_setQNaN(decimal *number);


void decimal_negate(decimal *result, const decimal *number);
void decimal_copy(decimal *result, const decimal *number);

#endif /* _DECIMAL_H */
