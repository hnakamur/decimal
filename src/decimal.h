#ifndef _DECIMAL_H
#define _DECIMAL_H 1

#include <stdint.h>

#ifndef bool
#define bool int
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#define DECIMAL_SIGN_PLUS 0
#define DECIMAL_SIGN_MINUS 1

#define DECIMAL_MINUS_SIGN_BIT 0x00000001

#define DECIMAL_KIND_BIT_MASK 0x00000006
#define DECIMAL_KIND_FINITE 0x00000000
#define DECIMAL_KIND_INFINITE 0x00000002
#define DECIMAL_KIND_QNAN 0x00000004
#define DECIMAL_KIND_SNAN 0x00000006

#define DECIMAL_DEFAULT_CAPACITY 40

typedef struct {
    uint32_t flags;
    uint32_t exponent;
    uint32_t capacity; /* coefficient capacity in digit count. */
    uint32_t length;   /* coefficient length in digit count. */
    uint8_t coefficient[DECIMAL_DEFAULT_CAPACITY / 2];
} decimal;

void decimal_init(decimal *number, uint32_t capacity);

bool decimal_isSignMinus(decimal *number);
bool decimal_isNaN(decimal *number);
bool decimal_isSNaN(decimal *number);
bool decimal_isQNaN(decimal *number);
bool decimal_isInfinite(decimal *number);
bool decimal_isFinite(decimal *number);
bool decimal_isZero(decimal *number);

void decimal_setZero(decimal *number);
void decimal_setInfinite(decimal *number, uint32_t sign);
void decimal_setSNaN(decimal *number);
void decimal_setQNaN(decimal *number);


void decimal_negate(decimal *result, decimal *number);
void decimal_copy(decimal *result, decimal *number);

#endif /* _DECIMAL_H */
