#ifndef _DEC128_H_
#define _DEC128_H_

#define dec128_radix(x) 10
#define dec128_BitWidth 128
#define dec128_Precision 34
#define dec128_EMax 6144
#define dec128_EMin (1 - dec128_EMax)
#define dec128_Bias 6176
#define dec128_CombinationFieldBitWidth 17
#define dec128_TrailingSignificandFieldBitWidth 110

/*
 * decimal128 type definition.
 *
 * This implementation uses the decimal encoding for the significand.
 */
#define dec128__ByteLength (dec128_BitWidth / 8)

typedef struct {
    uint8_t byte[dec128__ByteLength];
} dec128;

#define dec128__SignBit 0x80
#define dec128__NaNBits 0x7C
#define dec128__SNaNBits 0x7E
#define dec128__InfiniteMask 0x7C
#define dec128__InfiniteBits 0x78
#define dec128__FiniteMask 0x78

#define dec128_isSignMinus(x) (((x)->byte[0] & dec128__SignBit) != 0)
#define dec128_isNaN(x) \
    (((x)->byte[0] & dec128__NaNBits) == dec128__NaNBits)
#define dec128_isSignaling(x) \
    (((x)->byte[0] & dec128__SNaNBits) == dec128__SNaNBits)
#define dec128_isInfinite(x) \
    (((x)->byte[0] & dec128__InfiniteMask) == dec128__InfiniteBits)
#define dec128_isFinite(x) \
    (((x)->byte[0] & dec128__FiniteMask) != dec128__FiniteMask)
#define dec128_isZero(x) (dec128__getKind(x) == decimal__KindZero)
#define dec128_isNormal(x) (dec128__getKind(x) == decimal__KindNormal)
#define dec128_isSubnormal(x) (dec128__getKind(x) == decimal__KindSubnormal)
decimal_Class dec128_getClass(const dec128 *x);

bool dec128_isCanonical(const dec128 *x);

void dec128_roundToIntegral(dec128 *result, const dec128 *x, DecimalContext *ctx);
void dec128_roundToIntegralExact(dec128 *result, const dec128 *x, DecimalContext *ctx);

void dec128_nextUp(dec128 *result, const dec128 *x, DecimalContext *ctx);
void dec128_nextDown(dec128 *result, const dec128 *x, DecimalContext *ctx);

void dec128_remainder(dec128 *result, const dec128 *x, const dec128 *y, DecimalContext *ctx);

void dec128_minNum(dec128 *result, const dec128 *x, const dec128 *y, DecimalContext *ctx);
void dec128_maxNum(dec128 *result, const dec128 *x, const dec128 *y, DecimalContext *ctx);
void dec128_minNumMag(dec128 *result, const dec128 *x, const dec128 *y, DecimalContext *ctx);
void dec128_maxNumMag(dec128 *result, const dec128 *x, const dec128 *y, DecimalContext *ctx);

void dec128_quantize(dec128 *result, const dec128 *x, const dec128 *y, DecimalContext *ctx);

typedef int32_t dec128_logB_t;

void dec128_scaleB(dec128 *result, const dec128 *x, dec128_logB_t n, DecimalContext *ctx);
dec128_logB_t dec128_logB(const dec128 *x, DecimalContext *ctx);

void dec128_add(dec128 *result, const dec128 *x, const dec128 *y, DecimalContext *ctx);
void dec128_subtract(dec128 *result, const dec128 *x, const dec128 *y, DecimalContext *ctx);
void dec128_multiply(dec128 *result, const dec128 *x, const dec128 *y, DecimalContext *ctx);
void dec128_divide(dec128 *result, const dec128 *x, const dec128 *y, DecimalContext *ctx);
void dec128_squareRoot(dec128 *result, const dec128 *x, DecimalContext *ctx);
void dec128_fusedMultiplyAdd(dec128 *result, const dec128 *x, const dec128 *y, const dec128 *z, DecimalContext *ctx);

void dec128_convertFromInt64(dec128 *result, int64_t x, DecimalContext *ctx);
int64_t dec128_convertToInt64(const dec128 *x, DecimalContext *ctx);
int64_t dec128_convertToInt64Exact(const dec128 *x, DecimalContext *ctx);

void dec128_convertFromDouble(dec128 *result, double x, DecimalContext *ctx);
double dec128_convertToDouble(const dec128 *x, DecimalContext *ctx);

void dec128_convertFromDecimalString(dec128 *result, const char *str, DecimalContext *ctx);
int dec128_calcDecimalStringLength(const dec128 *x, DecimalContext *ctx);
void dec128_convertToDecimalString(char *buf, const dec128 *x, DecimalContext *ctx);

void dec128_convertFromHexString(dec128 *result, const char *str, DecimalContext *ctx);
uint32_t dec128_calcHexStringLength(const dec128 *x, DecimalContext *ctx);
void dec128_convertToHexString(const char *buf, const dec128 *x, DecimalContext *ctx);

void dec128_copy(dec128 *result, const dec128 *x);
void dec128_negate(dec128 *result, const dec128 *x);
void dec128_abs(dec128 *result, const dec128 *x);
void dec128_copySign(dec128 *result, const dec128 *x);

void dec128_encodeDecimal(dec128 *result, const dec128 *x);
void dec128_decodeDecimal(dec128 *result, const dec128 *x);
void dec128_encodeBinary(dec128 *result, const dec128 *x);
void dec128_decodeBinary(dec128 *result, const dec128 *x);

bool dec128_totalOrder(const dec128 *x, const dec128 *y, DecimalContext *ctx);
bool dec128_totalOrderMag(const dec128 *x, const dec128 *y, DecimalContext *ctx);
bool dec128_sameQuantum(const dec128 *x, const dec128 *y, DecimalContext *ctx);

decimal_Relation dec128_compareQuiet(const dec128 *x, const dec128 *y, DecimalContext *ctx);
decimal_Relation dec128_compareSignaling(const dec128 *x, const dec128 *y, DecimalContext *ctx);

bool dec128_is754version1985(void);
bool dec128_is754version2008(void);

#define dec128__setSignMinus(n) ((n)->byte[0] |= dec128__SignBit)
#define dec128__resetSignMinus(n) ((n)->byte[0] &= ~dec128__SignBit)
#define dec128__flipSignMinus(n) ((n)->byte[0] ^= dec128__SignBit)

decimal__Kind dec128__getKind(const dec128 *x);

void dec128__setInfiniy(dec128 *x);
void dec128__setQNaN(dec128 *x, const char *payload);
void dec128__setSNaN(dec128 *x, const char *payload);
void dec128__setQAndLeadDigit(dec128 *x, int32_t q, uint32_t leadDigit);

/*
 * temporary functions.
 * will be removed soon.
 */
void dec128_convertToExponentialDecimalString(char *buf, const dec128 *x,
    DecimalContext *ctx);
void dec128_convertToOrdinaryDecimalString(char *buf, const dec128 *x,
    DecimalContext *ctx);

/*
 * dec128 work
 */
typedef struct {
    decimal__Sign sign;
    decimal__Kind kind;
    int32_t q;
    char c[dec128_Precision + 1];
} dec128__wk;

#define dec128__wk_getDigit(x, i) ((x)->c[i] - '0')
#define dec128__wk_setDigit(x, i, d) ((x)->c[i] = (d) + '0')
#define dec128__wk_getE(x) ((x)->q + dec128_Precision - 1)

void dec128__wk_convertFromDec128(dec128__wk *result, const dec128 *x);
decimal_Class dec128__wk_getClass(const dec128__wk *x);
bool dec128__wk_convertFromDecimalCharacter(dec128__wk *result,
    const char *str);
int dec128__wk_countSignificantDigit(const dec128__wk *x);

#endif /* _DEC128_H_ */
