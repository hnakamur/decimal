#include <cutter.h>
#include "decimalInt.h"

void test_countDigitOfInt(void)
{
    cut_assert_equal_uint(1, decimal_countDigitOfInt(0));
    cut_assert_equal_uint(1, decimal_countDigitOfInt(1));
    cut_assert_equal_uint(1, decimal_countDigitOfInt(-1));
    cut_assert_equal_uint(1, decimal_countDigitOfInt(9));
    cut_assert_equal_uint(2, decimal_countDigitOfInt(10));
    cut_assert_equal_uint(2, decimal_countDigitOfInt(-10));
    cut_assert_equal_uint(2, decimal_countDigitOfInt(11));
    cut_assert_equal_uint(2, decimal_countDigitOfInt(99));
    cut_assert_equal_uint(3, decimal_countDigitOfInt(100));
}

void test_convertToDecimalNonExponential(void)
{
    decimal d;
    char buf[36];
    int32_t length;
    int32_t i;

    decimal_init(&d, DECIMAL_DEFAULT_CAPACITY);
    decimal_setZero(&d);
    cut_assert_equal_uint(sizeof("0") - 1,
        decimal_getLengthOfDecimalNonExponential(&d));
    decimal_convertToDecimalNonExponential(buf, &d);
    cut_assert_equal_string("0", buf);

    decimal_setSign(&d, 1);
    cut_assert_equal_uint(sizeof("-0") - 1,
        decimal_getLengthOfDecimalNonExponential(&d));
    decimal_convertToDecimalNonExponential(buf, &d);
    cut_assert_equal_string("-0", buf);

    decimal_setSign(&d, 0);
    decimal_setLength(&d, 3);
    decimal_setSignificandDigit(&d, 0, 1);
    decimal_setSignificandDigit(&d, 1, 2);
    decimal_setSignificandDigit(&d, 2, 3);
    decimal_setExponent(&d, 2);
    cut_assert_equal_uint(sizeof("123") - 1,
        decimal_getLengthOfDecimalNonExponential(&d));
    decimal_convertToDecimalNonExponential(buf, &d);
    cut_assert_equal_string("123", buf);

    decimal_setSign(&d, 1);
    cut_assert_equal_uint(sizeof("-123") - 1,
        decimal_getLengthOfDecimalNonExponential(&d));
    decimal_convertToDecimalNonExponential(buf, &d);
    cut_assert_equal_string("-123", buf);

    decimal_setSign(&d, 0);
    decimal_setExponent(&d, 3);
    cut_assert_equal_uint(sizeof("1230") - 1,
        decimal_getLengthOfDecimalNonExponential(&d));
    decimal_convertToDecimalNonExponential(buf, &d);
    cut_assert_equal_string("1230", buf);

    decimal_setExponent(&d, 1);
    cut_assert_equal_uint(sizeof("12.3") - 1,
        decimal_getLengthOfDecimalNonExponential(&d));
    decimal_convertToDecimalNonExponential(buf, &d);
    cut_assert_equal_string("12.3", buf);

    decimal_setExponent(&d, 0);
    cut_assert_equal_uint(sizeof("1.23") - 1,
        decimal_getLengthOfDecimalNonExponential(&d));
    decimal_convertToDecimalNonExponential(buf, &d);
 
    decimal_setExponent(&d, -1);
    cut_assert_equal_uint(sizeof("0.123") - 1,
        decimal_getLengthOfDecimalNonExponential(&d));
    decimal_convertToDecimalNonExponential(buf, &d);
  
    decimal_setExponent(&d, -2);
    cut_assert_equal_uint(sizeof("0.0123") - 1,
        decimal_getLengthOfDecimalNonExponential(&d));
    decimal_convertToDecimalNonExponential(buf, &d);
    cut_assert_equal_string("0.0123", buf);
  
    decimal_setExponent(&d, -3);
    cut_assert_equal_uint(sizeof("0.00123") - 1,
        decimal_getLengthOfDecimalNonExponential(&d));
    decimal_convertToDecimalNonExponential(buf, &d);
    cut_assert_equal_string("0.00123", buf);

    decimal_setLength(&d, 1);
    decimal_setSignificandDigit(&d, 0, 5);
    decimal_setExponent(&d, -6);
    cut_assert_equal_uint(sizeof("0.000005") - 1,
        decimal_getLengthOfDecimalNonExponential(&d));
    decimal_convertToDecimalNonExponential(buf, &d);
    cut_assert_equal_string("0.000005", buf);

    decimal_setLength(&d, 2);
    decimal_setSignificandDigit(&d, 0, 5);
    decimal_setSignificandDigit(&d, 1, 0);
    decimal_setExponent(&d, -6);
    cut_assert_equal_uint(sizeof("0.0000050") - 1,
        decimal_getLengthOfDecimalNonExponential(&d));
    decimal_convertToDecimalNonExponential(buf, &d);
    cut_assert_equal_string("0.0000050", buf);

    decimal_setKind(&d, DECIMAL_KIND_INFINITY);
    cut_assert_equal_uint(sizeof("Infinity") - 1,
        decimal_getLengthOfDecimalNonExponential(&d));
    decimal_convertToDecimalNonExponential(buf, &d);
    cut_assert_equal_string("Infinity", buf);

    decimal_setSign(&d, 1);
    cut_assert_equal_uint(sizeof("-Infinity") - 1,
        decimal_getLengthOfDecimalNonExponential(&d));
    decimal_convertToDecimalNonExponential(buf, &d);
    cut_assert_equal_string("-Infinity", buf);

    decimal_setSign(&d, 0);
    decimal_setKind(&d, DECIMAL_KIND_QNAN);
    decimal_setLength(&d, 0);
    cut_assert_equal_uint(sizeof("NaN") - 1,
        decimal_getLengthOfDecimalNonExponential(&d));
    decimal_convertToDecimalNonExponential(buf, &d);
    cut_assert_equal_string("NaN", buf);

    decimal_setLength(&d, 3);
    decimal_setSignificandDigit(&d, 0, 1);
    decimal_setSignificandDigit(&d, 1, 2);
    decimal_setSignificandDigit(&d, 2, 3);
    cut_assert_equal_uint(sizeof("NaN123") - 1,
        decimal_getLengthOfDecimalNonExponential(&d));
    decimal_convertToDecimalNonExponential(buf, &d);
    cut_assert_equal_string("NaN123", buf);

    decimal_setSign(&d, 1);
    decimal_setKind(&d, DECIMAL_KIND_SNAN);
    decimal_setLength(&d, 0);
    cut_assert_equal_uint(sizeof("-sNaN") - 1,
        decimal_getLengthOfDecimalNonExponential(&d));
    decimal_convertToDecimalNonExponential(buf, &d);
    cut_assert_equal_string("-sNaN", buf);
}

void test_convertToDecimalExponential(void)
{
    decimal d;
    char buf[36];
    int32_t length;
    int32_t i;

    decimal_init(&d, DECIMAL_DEFAULT_CAPACITY);
    decimal_setZero(&d);
    cut_assert_equal_uint(sizeof("0E+0") - 1,
        decimal_getLengthOfDecimalExponential(&d));
    decimal_convertToDecimalExponential(buf, &d);
    cut_assert_equal_string("0E+0", buf);

    length = 10;
    decimal_setLength(&d, length);
    for (i = 0; i < length; ++i) {
        decimal_setSignificandDigit(&d, i, (i + 1) % 10);
    }
    decimal_setExponent(&d, 1234567890);
    cut_assert_equal_uint(sizeof("1.234567890E+1234567890") - 1,
        decimal_getLengthOfDecimalExponential(&d));
    decimal_convertToDecimalExponential(buf, &d);
    cut_assert_equal_string("1.234567890E+1234567890", buf);

    decimal_setLength(&d, 3);
    decimal_setSignificandDigit(&d, 0, 1);
    decimal_setSignificandDigit(&d, 1, 2);
    decimal_setSignificandDigit(&d, 2, 3);
    decimal_setExponent(&d, 3);
    cut_assert_equal_uint(sizeof("1.23E+3") - 1,
        decimal_getLengthOfDecimalExponential(&d));
    decimal_convertToDecimalExponential(buf, &d);
    cut_assert_equal_string("1.23E+3", buf);

    decimal_setExponent(&d, 5);
    cut_assert_equal_uint(sizeof("1.23E+5") - 1,
        decimal_getLengthOfDecimalExponential(&d));
    decimal_convertToDecimalExponential(buf, &d);
    cut_assert_equal_string("1.23E+5", buf);

    decimal_setExponent(&d, -8);
    decimal_convertToDecimalExponential(buf, &d);
    cut_assert_equal_uint(sizeof("1.23E-8") - 1,
        decimal_getLengthOfDecimalExponential(&d));
    cut_assert_equal_string("1.23E-8", buf);

    decimal_setSign(&d, 1);
    decimal_setExponent(&d, -10);
    cut_assert_equal_uint(sizeof("-1.23E-10") - 1,
        decimal_getLengthOfDecimalExponential(&d));
    decimal_convertToDecimalExponential(buf, &d);
    cut_assert_equal_string("-1.23E-10", buf);

    decimal_setSign(&d, 0);
    decimal_setLength(&d, 1);
    decimal_setSignificandDigit(&d, 0, 0);
    decimal_setExponent(&d, 2);
    cut_assert_equal_uint(sizeof("0E+2") - 1,
        decimal_getLengthOfDecimalExponential(&d));
    decimal_convertToDecimalExponential(buf, &d);
    cut_assert_equal_string("0E+2", buf);

    decimal_setSignificandDigit(&d, 0, 5);
    decimal_setExponent(&d, -7);
    cut_assert_equal_uint(sizeof("5E-7") - 1,
        decimal_getLengthOfDecimalExponential(&d));
    decimal_convertToDecimalExponential(buf, &d);
    cut_assert_equal_string("5E-7", buf);

    decimal_setKind(&d, DECIMAL_KIND_INFINITY);
    cut_assert_equal_uint(sizeof("Infinity") - 1,
        decimal_getLengthOfDecimalExponential(&d));
    decimal_convertToDecimalExponential(buf, &d);
    cut_assert_equal_string("Infinity", buf);

    decimal_setSign(&d, 1);
    cut_assert_equal_uint(sizeof("-Infinity") - 1,
        decimal_getLengthOfDecimalExponential(&d));
    decimal_convertToDecimalExponential(buf, &d);
    cut_assert_equal_string("-Infinity", buf);

    decimal_setSign(&d, 0);
    decimal_setKind(&d, DECIMAL_KIND_QNAN);
    decimal_setLength(&d, 0);
    cut_assert_equal_uint(sizeof("NaN") - 1,
        decimal_getLengthOfDecimalExponential(&d));
    decimal_convertToDecimalExponential(buf, &d);
    cut_assert_equal_string("NaN", buf);

    decimal_setLength(&d, 3);
    decimal_setSignificandDigit(&d, 0, 1);
    decimal_setSignificandDigit(&d, 1, 2);
    decimal_setSignificandDigit(&d, 2, 3);
    cut_assert_equal_uint(sizeof("NaN123") - 1,
        decimal_getLengthOfDecimalExponential(&d));
    decimal_convertToDecimalExponential(buf, &d);
    cut_assert_equal_string("NaN123", buf);

    decimal_setSign(&d, 1);
    decimal_setKind(&d, DECIMAL_KIND_SNAN);
    decimal_setLength(&d, 0);
    cut_assert_equal_uint(sizeof("-sNaN") - 1,
        decimal_getLengthOfDecimalExponential(&d));
    decimal_convertToDecimalExponential(buf, &d);
    cut_assert_equal_string("-sNaN", buf);
}

void test_convertFromDecimalCharacter()
{
    decimal d;

    decimal_init(&d, DECIMAL_DEFAULT_CAPACITY);

    cut_assert_true(decimal_convertFromDecimalCharacter(&d, "Infinity"));
    cut_assert_true(decimal_isInfinite(&d));
    cut_assert_false(decimal_isSignMinus(&d));

    cut_assert_true(decimal_convertFromDecimalCharacter(&d, "-INF"));
    cut_assert_true(decimal_isInfinite(&d));
    cut_assert_true(decimal_isSignMinus(&d));

    cut_assert_true(decimal_convertFromDecimalCharacter(&d, "NaN"));
    cut_assert_true(decimal_isQNaN(&d));
    cut_assert_false(decimal_isSignMinus(&d));

    cut_assert_true(decimal_convertFromDecimalCharacter(&d, "nan123"));
    cut_assert_true(decimal_isQNaN(&d));
    cut_assert_false(decimal_isSignMinus(&d));
    cut_assert_equal_uint(3, decimal_getLength(&d));
    cut_assert_equal_uint(1, decimal_getSignificandDigit(&d, 0));
    cut_assert_equal_uint(2, decimal_getSignificandDigit(&d, 1));
    cut_assert_equal_uint(3, decimal_getSignificandDigit(&d, 2));

    cut_assert_true(decimal_convertFromDecimalCharacter(&d, "-sNaN"));
    cut_assert_true(decimal_isSNaN(&d));
    cut_assert_true(decimal_isSignMinus(&d));

    cut_assert_true(decimal_convertFromDecimalCharacter(&d, "123"));
    cut_assert_false(decimal_isSignMinus(&d));
    cut_assert_true(decimal_isFinite(&d));
    cut_assert_equal_int(2, decimal_getExponent(&d));
    cut_assert_equal_uint(3, decimal_getLength(&d));
    cut_assert_equal_uint(1, decimal_getSignificandDigit(&d, 0));
    cut_assert_equal_uint(2, decimal_getSignificandDigit(&d, 1));
    cut_assert_equal_uint(3, decimal_getSignificandDigit(&d, 2));

    cut_assert_true(decimal_convertFromDecimalCharacter(&d, "-123"));
    cut_assert_true(decimal_isSignMinus(&d));
    cut_assert_true(decimal_isFinite(&d));
    cut_assert_equal_int(2, decimal_getExponent(&d));
    cut_assert_equal_uint(3, decimal_getLength(&d));
    cut_assert_equal_uint(1, decimal_getSignificandDigit(&d, 0));
    cut_assert_equal_uint(2, decimal_getSignificandDigit(&d, 1));
    cut_assert_equal_uint(3, decimal_getSignificandDigit(&d, 2));

    cut_assert_true(decimal_convertFromDecimalCharacter(&d, "123E+3"));
    cut_assert_false(decimal_isSignMinus(&d));
    cut_assert_true(decimal_isFinite(&d));
    cut_assert_equal_int(5, decimal_getExponent(&d));
    cut_assert_equal_uint(3, decimal_getLength(&d));
    cut_assert_equal_uint(1, decimal_getSignificandDigit(&d, 0));
    cut_assert_equal_uint(2, decimal_getSignificandDigit(&d, 1));
    cut_assert_equal_uint(3, decimal_getSignificandDigit(&d, 2));

    cut_assert_true(decimal_convertFromDecimalCharacter(&d, "123e5"));
    cut_assert_false(decimal_isSignMinus(&d));
    cut_assert_true(decimal_isFinite(&d));
    cut_assert_equal_int(7, decimal_getExponent(&d));
    cut_assert_equal_uint(3, decimal_getLength(&d));
    cut_assert_equal_uint(1, decimal_getSignificandDigit(&d, 0));
    cut_assert_equal_uint(2, decimal_getSignificandDigit(&d, 1));
    cut_assert_equal_uint(3, decimal_getSignificandDigit(&d, 2));

    cut_assert_true(decimal_convertFromDecimalCharacter(&d, "12.3"));
    cut_assert_false(decimal_isSignMinus(&d));
    cut_assert_true(decimal_isFinite(&d));
    cut_assert_equal_int(1, decimal_getExponent(&d));
    cut_assert_equal_uint(3, decimal_getLength(&d));
    cut_assert_equal_uint(1, decimal_getSignificandDigit(&d, 0));
    cut_assert_equal_uint(2, decimal_getSignificandDigit(&d, 1));
    cut_assert_equal_uint(3, decimal_getSignificandDigit(&d, 2));

    cut_assert_true(decimal_convertFromDecimalCharacter(&d, "0.00123"));
    cut_assert_false(decimal_isSignMinus(&d));
    cut_assert_true(decimal_isFinite(&d));
    cut_assert_equal_int(-3, decimal_getExponent(&d));
    cut_assert_equal_uint(3, decimal_getLength(&d));
    cut_assert_equal_uint(1, decimal_getSignificandDigit(&d, 0));
    cut_assert_equal_uint(2, decimal_getSignificandDigit(&d, 1));
    cut_assert_equal_uint(3, decimal_getSignificandDigit(&d, 2));

    cut_assert_true(decimal_convertFromDecimalCharacter(&d, "0.00123E6"));
    cut_assert_false(decimal_isSignMinus(&d));
    cut_assert_true(decimal_isFinite(&d));
    cut_assert_equal_int(3, decimal_getExponent(&d));
    cut_assert_equal_uint(3, decimal_getLength(&d));
    cut_assert_equal_uint(1, decimal_getSignificandDigit(&d, 0));
    cut_assert_equal_uint(2, decimal_getSignificandDigit(&d, 1));
    cut_assert_equal_uint(3, decimal_getSignificandDigit(&d, 2));

    cut_assert_true(decimal_convertFromDecimalCharacter(&d, "0.00123E-30"));
    cut_assert_false(decimal_isSignMinus(&d));
    cut_assert_true(decimal_isFinite(&d));
    cut_assert_equal_int(-33, decimal_getExponent(&d));
    cut_assert_equal_uint(3, decimal_getLength(&d));
    cut_assert_equal_uint(1, decimal_getSignificandDigit(&d, 0));
    cut_assert_equal_uint(2, decimal_getSignificandDigit(&d, 1));
    cut_assert_equal_uint(3, decimal_getSignificandDigit(&d, 2));

    cut_assert_true(decimal_convertFromDecimalCharacter(&d, ".0"));
    cut_assert_false(decimal_isSignMinus(&d));
    cut_assert_true(decimal_isFinite(&d));
    cut_assert_equal_int(0, decimal_getExponent(&d));
    cut_assert_equal_uint(2, decimal_getLength(&d));
    cut_assert_equal_uint(0, decimal_getSignificandDigit(&d, 0));
    cut_assert_equal_uint(0, decimal_getSignificandDigit(&d, 1));

    cut_assert_true(decimal_convertFromDecimalCharacter(&d, ".5"));
    cut_assert_false(decimal_isSignMinus(&d));
    cut_assert_true(decimal_isFinite(&d));
    cut_assert_equal_int(-1, decimal_getExponent(&d));
    cut_assert_equal_uint(1, decimal_getLength(&d));
    cut_assert_equal_uint(5, decimal_getSignificandDigit(&d, 0));

    cut_assert_true(decimal_convertFromDecimalCharacter(&d, "-.5"));
    cut_assert_true(decimal_isSignMinus(&d));
    cut_assert_true(decimal_isFinite(&d));
    cut_assert_equal_int(-1, decimal_getExponent(&d));
    cut_assert_equal_uint(1, decimal_getLength(&d));
    cut_assert_equal_uint(5, decimal_getSignificandDigit(&d, 0));

    cut_assert_true(decimal_convertFromDecimalCharacter(&d, ".50"));
    cut_assert_false(decimal_isSignMinus(&d));
    cut_assert_true(decimal_isFinite(&d));
    cut_assert_equal_int(-1, decimal_getExponent(&d));
    cut_assert_equal_uint(2, decimal_getLength(&d));
    cut_assert_equal_uint(5, decimal_getSignificandDigit(&d, 0));
    cut_assert_equal_uint(0, decimal_getSignificandDigit(&d, 1));

    cut_assert_true(decimal_convertFromDecimalCharacter(&d, "0."));
    cut_assert_false(decimal_isSignMinus(&d));
    cut_assert_true(decimal_isFinite(&d));
    cut_assert_equal_int(0, decimal_getExponent(&d));
    cut_assert_equal_uint(1, decimal_getLength(&d));
    cut_assert_equal_uint(0, decimal_getSignificandDigit(&d, 0));

    cut_assert_false(decimal_convertFromDecimalCharacter(&d, "."));

    cut_assert_true(decimal_convertFromDecimalCharacter(&d, "0.00"));
    cut_assert_false(decimal_isSignMinus(&d));
    cut_assert_true(decimal_isFinite(&d));
    cut_assert_equal_int(0, decimal_getExponent(&d));
    cut_assert_equal_uint(3, decimal_getLength(&d));
    cut_assert_equal_uint(0, decimal_getSignificandDigit(&d, 0));
    cut_assert_equal_uint(0, decimal_getSignificandDigit(&d, 1));
    cut_assert_equal_uint(0, decimal_getSignificandDigit(&d, 2));
}

void test_roundTowardZero()
{
    decimal d;
    DecimalContext ctx;
    char buf[36];

    decimal_init(&d, DECIMAL_DEFAULT_CAPACITY);
    DecimalContext_init(&ctx, 5, Decimal_RoundTowardZero);

/*
bsrx426  toSci  1.23450    ->  1.2345  Rounded
bsrx427  toSci  1.234549   ->  1.2345  Rounded Inexact
bsrx428  toSci  1.234550   ->  1.2345  Rounded Inexact
bsrx429  toSci  1.234551   ->  1.2345  Rounded Inexact
*/

    cut_assert_true(decimal_convertFromDecimalCharacter(&d, "1.23450"));
    decimal_round(&d, &d, &ctx);
    cut_assert_equal_uint(0, DecimalContext_saveAllFlags(&ctx));
    decimal_convertToDecimalNonExponential(buf, &d);
    cut_assert_equal_string("1.2345", buf);

    cut_assert_true(decimal_convertFromDecimalCharacter(&d, "1.234549"));
    decimal_round(&d, &d, &ctx);
    cut_assert_equal_uint(Decimal_Inexact, DecimalContext_saveAllFlags(&ctx));
    decimal_convertToDecimalNonExponential(buf, &d);
    cut_assert_equal_string("1.2345", buf);

    cut_assert_true(decimal_convertFromDecimalCharacter(&d, "1.234550"));
    decimal_round(&d, &d, &ctx);
    cut_assert_equal_uint(Decimal_Inexact, DecimalContext_saveAllFlags(&ctx));
    decimal_convertToDecimalNonExponential(buf, &d);
    cut_assert_equal_string("1.2345", buf);

    cut_assert_true(decimal_convertFromDecimalCharacter(&d, "1.234551"));
    decimal_round(&d, &d, &ctx);
    cut_assert_equal_uint(Decimal_Inexact, DecimalContext_saveAllFlags(&ctx));
    decimal_convertToDecimalNonExponential(buf, &d);
    cut_assert_equal_string("1.2345", buf);

}
