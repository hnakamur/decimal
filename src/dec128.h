#ifndef _DEC128_H_
#define _DEC128_H_

#include <stdint.h>
#include <arpa/inet.h>
#include "bool.h"

#define dec128_BitWidth 128
#define dec128_Precision 34
#define dec128_EMax 6144
#define dec128_EMin (1 - dec128_EMAX)
#define dec128_Bias 6176
#define dec128_CombinationFieldBitWidth 17
#define dec128_TrailingSignificandFieldBitWidth 110
#define dec128_UnitSize sizeof(uint32_t)
#define dec128_UnitCount (dec128_BitWidth / 8 / dec128_UnitSize)

/*
 * decimal128 type definition.
 *
 * This implementation uses the decimal encoding for the significand.
 */
typedef struct {
    uint32_t u[dec128_UnitCount];
} dec128;

typedef struct {
    uint8_t b[dec128_BitWidth / 8];
} dec128c;

#define dec128__wk_DigitCapacity (dec128_Precision + 4 + 1)

typedef enum {
    dec128__kindFinite,
    dec128__kindInfinite,
    dec128__kindQNaN,
    dec128__kindSNaN
} dec128__kind;

typedef struct {
    uint32_t sign;
    dec128__kind kind;
    int32_t q;
    char c[dec128__wk_DigitCapacity];
} dec128__wk;

#ifdef WORDS_BIGENDIAN
#define dec128__SignBit 0x80000000
#define dec128__NaNBits 0x7C000000
#define dec128__SNaNBits 0x7E000000
#define dec128__InfiniteMask 0x7C000000
#define dec128__InfiniteBits 0x78000000
#define dec128__FiniteMask 0x78000000
#else
#define dec128__SignBit 0x00000080
#define dec128__NaNBits 0x0000007C
#define dec128__SNaNBits 0x0000007E
#define dec128__InfiniteMask 0x0000007C
#define dec128__InfiniteBits 0x00000078
#define dec128__FiniteMask 0x00000078
#endif

#define dec128_isSignMinus(n) (((n)->u[0] & dec128__SignBit) != 0)
#define dec128_isNaN(n) \
    (((n)->u[0] & dec128__NaNBits) == dec128__NaNBits)
#define dec128_isSignaling(n) \
    (((n)->u[0] & dec128__SNaNBits) == dec128__SNaNBits)
#define dec128_isInfinite(n) \
    (((n)->u[0] & dec128__InfiniteMask) == dec128__InfiniteBits)
#define dec128_isFinite(n) \
    (((n)->u[0] & dec128__FiniteMask) != dec128__FiniteMask)

#define dec128_radix(n) 10

bool dec128_isNormal(dec128 *n);
bool dec128_isZero(dec128 *n);
bool dec128_isSubnormal(dec128 *n);
bool dec128_isCanonical(dec128 *n);

#define dec128__setSignMinus(n) ((n)->u[0] |= dec128__SignBit)
#define dec128__resetSignMinus(n) ((n)->u[0] &= ~dec128__SignBit)
#define dec128__flipSignMinus(n) ((n)->u[0] ^= dec128__SignBit)

bool dec128__wk_convertFromDec128(dec128__wk *result, const dec128 *n);

#endif /* _DEC128_H_ */
