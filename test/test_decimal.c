#include <stdio.h>
#include <cutter.h>
#include "decimal.h"

void test_isSignMinus(void)
{
    decimal d;

    decimal_init(&d, DECIMAL_DEFAULT_CAPACITY);
    decimal_setZero(&d);
    cut_assert_false(decimal_isSignMinus(&d));

    decimal_negate(&d, &d);
    cut_assert_true(decimal_isSignMinus(&d));
}

void test_isZero(void)
{
    decimal d;

    decimal_init(&d, DECIMAL_DEFAULT_CAPACITY);
    decimal_setZero(&d);
    cut_assert_true(decimal_isZero(&d));
}

void test_isFinite(void)
{
    decimal d;

    decimal_init(&d, DECIMAL_DEFAULT_CAPACITY);
    decimal_setZero(&d);
    cut_assert_true(decimal_isFinite(&d));

    decimal_negate(&d, &d);
    cut_assert_true(decimal_isFinite(&d));

    decimal_setInfinite(&d, DECIMAL_SIGN_PLUS);
    cut_assert_false(decimal_isFinite(&d));

    decimal_negate(&d, &d);
    cut_assert_false(decimal_isFinite(&d));

    decimal_setSNaN(&d);
    cut_assert_false(decimal_isFinite(&d));

    decimal_setQNaN(&d);
    cut_assert_false(decimal_isFinite(&d));
}

void test_isInfinite(void)
{
    decimal d;

    decimal_init(&d, DECIMAL_DEFAULT_CAPACITY);
    decimal_setInfinite(&d, DECIMAL_SIGN_PLUS);
    cut_assert_true(decimal_isInfinite(&d) && !decimal_isSignMinus(&d));

    decimal_negate(&d, &d);
    cut_assert_true(decimal_isInfinite(&d) && decimal_isSignMinus(&d));
}

void test_isSNaN(void)
{
    decimal d;

    decimal_init(&d, DECIMAL_DEFAULT_CAPACITY);
    decimal_setSNaN(&d);
    cut_assert_true(decimal_isSNaN(&d));
}

void test_isQNaN(void)
{
    decimal d;

    decimal_init(&d, DECIMAL_DEFAULT_CAPACITY);
    decimal_setQNaN(&d);
    cut_assert_true(decimal_isQNaN(&d));
}
