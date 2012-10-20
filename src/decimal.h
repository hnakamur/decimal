#ifndef _DECIMAL_H_
#define _DECIMAL_H_ 1

#include <stdint.h>
#include "bool.h"
#include "DecimalContext.h"

/*
 * public enums.
 */

typedef enum {
    decimal_SNaN,
    decimal_QNaN,
    decimal_NegativeInfinity,
    decimal_NegativeNormal,
    decimal_NegativeSubnormal,
    decimal_NegativeZero,
    decimal_PositiveZero,
    decimal_PositiveSubnormal,
    decimal_PositiveNormal,
    decimal_PositiveInfinity
} decimal_Class;

typedef enum {
    decimal_LessThan = -1,
    decimal_Equal,
    decimal_GreaterThan,
    decimal_Unordered
} decimal_Relation;


/*
 * private enums.
 */

typedef enum {
    decimal__SignPlus,
    decimal__SignMinus
} decimal__Sign;

typedef enum {
    decimal__KindZero,
    decimal__KindSubnormal,
    decimal__KindNormal,
    decimal__KindInfinity,
    decimal__KindQNaN,
    decimal__KindSNaN
} decimal__Kind;

#include "dec128.h"

/*
 * declet functions
 */
#define decimal_DigitCountPerDeclet 3
#define decimal__makeDeclet(hi, mid, lo) ((hi) * 100 + (mid) * 10 + (lo))
#define decimal__getHiDigitInDeclet(declet) ((declet) / 100)
#define decimal__getMidDigitInDeclet(declet) (((declet) / 10) % 10)
#define decimal__getLoDigitInDeclet(declet) ((declet) % 10)
int decimal__getDigitInDeclet(int declet, int pos);


typedef uint16_t decimal__Unit;
#define decimal_DigitCountPerUnit decimal_DigitCountPerDeclet
#define decimal_DigitCountToUnitCount(count) \
    (((count) + decimal_DigitCountPerUnit - 1) / decimal_DigitCountPerUnit)

#define decimal_DefaultDigitCount 36
#define decimal_DefaultUnitCount \
    decimal_DigitCountToUnitCount(decimal_DefaultDigitCount)

typedef struct {
    int prec;
    int emax;
    decimal__Sign sign;
    decimal__Kind kind;
    int q;
    int digitCount;
    decimal__Unit unit[decimal_DefaultUnitCount];
} decimal;

#define decimal_getPrec(x) ((x)->prec)
#define decimal_getEMax(x) ((x)->emax)
#define decimal_getEMin(x) (1 - (x)->emax)
#define decimal_getQ(x) ((x)->q)
#define decimal_getDigitCount(x) ((x)->digitCount)
int decimal__getDigit(decimal *x, int pos);
void decimal__setDigit(decimal *x, int pos, int digit);

int decimal__getTrailingZeroCount(decimal *x);

#define decimal_calcSize(prec) (4 * sizeof(int) + sizeof(decimal__Sign) \
    + sizeof(decimal__Kind) \
    + decimal_DigitCountToUnitCount(prec) * sizeof(decimal__Unit))

#define decimal_isSignMinus(x) ((x)->sign == decimal__SignMinus)
#define decimal_isNaN(x) \
    ((x)->kind == decimal__KindQNaN || (x)->kind == decimal__KindSNaN)
#define decimal_isSignaling(x) ((x)->kind == decimal__KindSNaN)
#define decimal_isInfinite(x) ((x)->kind == decimal__KindInfinity)
#define decimal_isZero(x) ((x)->kind == decimal__KindZero)
#define decimal_isNormal(x) ((x)->kind == decimal__KindNormal)
#define decimal_isSubnormal(x) ((x)->kind == decimal__KindSubnormal)
#define decimal_isFinite(x) \
    (decimal_isZero(x) || decimal_isNormal(x) || decimal_isSubnormal(x))
#define decimal_isCanonical(x) true

decimal_Class decimal_getClass(const decimal *x);

void decimal_negate(decimal *result, const decimal *number);
void decimal_copy(decimal *result, const decimal *number);
void decimal_copySign(decimal *result, const decimal *number);

/*
 * utility functions.
 */
decimal *decimal_mallocDecimal(int prec);
decimal *decimal_reallocDecimal(decimal *x, int prec);
void decimal_freeDecimal(decimal *x);

void decimal_setPrecAndEmax(decimal *x, int prec, int emax);

/*
 * private functions.
 */
decimal_Class decimal__getClassFromSignAndKind(decimal__Sign sign, 
    decimal__Kind kind);


#define decimal__Radix 10
#define decimal__Digits "0123456789"
#define decimal__HexDigits "0123456789ABCDEF"
#define decimal__DigitCountPerDeclet 3

bool decimal__isDigitChar(char ch);
//#define decimal__isDigitChar(c) (strchr(decimal__Digits, (c)) != NULL)

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

int32_t decimal__encodeDecletFromDigits(const int32_t *digit);
void decimal__decodeDecletToDigits(int32_t *digit, int32_t declet);
int32_t decimal__encodeDecletFromInt(int32_t value);
int32_t decimal__decodeDecletToInt(int32_t declet);

#define decimal__isCanonicalDeclet(declet) (((declet) & 0x30) == 0)
bool decimal__isDigitString(const char *str);
char *decimal__sprintInt(char *buf, int value);
char *decimal_padZero(char *buf, int count);
#endif /* _DECIMAL_H_ */
