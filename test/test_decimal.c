#include <stdio.h>
#include <cutter.h>
#include "decimal.h"

void test_isSignMinus(void)
{
    decimal d;

    decimal_init(&d, decimal_DefaultCapacity);
    decimal_setZero(&d);
    cut_assert_false(decimal_isSignMinus(&d));

    decimal_negate(&d, &d);
    cut_assert_true(decimal_isSignMinus(&d));
}

void test_isZero(void)
{
    decimal d;

    decimal_init(&d, decimal_DefaultCapacity);
    decimal_setZero(&d);
    cut_assert_true(decimal_isZero(&d));
}

void test_isFinite(void)
{
    decimal d;

    decimal_init(&d, decimal_DefaultCapacity);
    decimal_setZero(&d);
    cut_assert_true(decimal_isFinite(&d));

    decimal_negate(&d, &d);
    cut_assert_true(decimal_isFinite(&d));

    decimal_setInfinite(&d);
    decimal_abs(&d, &d);
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

    decimal_init(&d, decimal_DefaultCapacity);
    decimal_setInfinite(&d);
    decimal_abs(&d, &d);
    cut_assert_true(decimal_isInfinite(&d) && !decimal_isSignMinus(&d));

    decimal_negate(&d, &d);
    cut_assert_true(decimal_isInfinite(&d) && decimal_isSignMinus(&d));
}

void test_isSignaling(void)
{
    decimal d;

    decimal_init(&d, decimal_DefaultCapacity);
    decimal_setSNaN(&d);
    cut_assert_true(decimal_isSignaling(&d));
}

void test_isNaN(void)
{
    decimal d;

    decimal_init(&d, decimal_DefaultCapacity);
    decimal_setQNaN(&d);
    cut_assert_true(decimal_isNaN(&d));
    cut_assert_false(decimal_isSignaling(&d));
}
