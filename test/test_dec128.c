#include <stdio.h>
#include <cutter.h>
#include "decimal.h"

void test_initWithRawHexString(void)
{
    dec128 d;
    dec128__wk w;

    dec128__initWithRawHexString(&d, "A20780000000000000000000000003D0");
    dec128__wk_convertFromDec128(&w, &d);
    dec128_isCanonical(&d);

    dec128__initWithRawHexString(&d, "00084000000000000000000000000001");
    dec128__wk_convertFromDec128(&w, &d);
    dec128_isCanonical(&d);

    dec128__initWithRawHexString(&d, "00000800000000000000000000000000");
    dec128__wk_convertFromDec128(&w, &d);
    dec128_isCanonical(&d);
}

void test_dec128__setQNaN(void)
{
    dec128 d;
    char buf[dec128__ByteLength * 2 + 1];

    buf[dec128__ByteLength * 2] = '\0';

    dec128__resetSignMinus(&d);
    dec128__setQNaN(&d, NULL);
    dec128__convertToRawHexString(buf, &d);
    cut_assert_equal_string("7C000000000000000000000000000000", buf);

    dec128__resetSignMinus(&d);
    dec128__setQNaN(&d, "0");
    dec128__convertToRawHexString(buf, &d);
    cut_assert_equal_string("7C000000000000000000000000000000", buf);

    dec128__resetSignMinus(&d);
    dec128__setQNaN(&d, "1");
    dec128__convertToRawHexString(buf, &d);
    cut_assert_equal_string("7C000000000000000000000000000001", buf);

    dec128__resetSignMinus(&d);
    dec128__setQNaN(&d, "12");
    dec128__convertToRawHexString(buf, &d);
    cut_assert_equal_string("7C000000000000000000000000000012", buf);

    dec128__resetSignMinus(&d);
    dec128__setQNaN(&d, "79");
    dec128__convertToRawHexString(buf, &d);
    cut_assert_equal_string("7C000000000000000000000000000079", buf);

    dec128__resetSignMinus(&d);
    dec128__setQNaN(&d, "12345");
    dec128__convertToRawHexString(buf, &d);
    cut_assert_equal_string("7C0000000000000000000000000049C5", buf);

    dec128__resetSignMinus(&d);
    dec128__setQNaN(&d, "123456");
    dec128__convertToRawHexString(buf, &d);
    cut_assert_equal_string("7C000000000000000000000000028E56", buf);

    dec128__resetSignMinus(&d);
    dec128__setQNaN(&d, "799799");
    dec128__convertToRawHexString(buf, &d);
    cut_assert_equal_string("7C0000000000000000000000000F7FDF", buf);

    dec128__resetSignMinus(&d);
    dec128__setQNaN(&d, "799799799799799799799799799799799");
    dec128__convertToRawHexString(buf, &d);
    cut_assert_equal_string("7C003DFF7FDFF7FDFF7FDFF7FDFF7FDF", buf);

    dec128__resetSignMinus(&d);
    dec128__setQNaN(&d, "999999999999999999999999999999999");
    dec128__convertToRawHexString(buf, &d);
    cut_assert_equal_string("7C000FF3FCFF3FCFF3FCFF3FCFF3FCFF", buf);
}

void test_dec128_convertFromDecimalString(void)
{
    dec128 d;
    DecimalContext ctx;
    char buf[dec128__ByteLength * 2 + 1];

    buf[dec128__ByteLength * 2] = '\0';

    dec128_convertFromDecimalString(&d, "-7.50", &ctx);
    dec128__convertToRawHexString(buf, &d);
    cut_assert_equal_string("A20780000000000000000000000003D0", buf);

    dec128_convertFromDecimalString(&d, "-7.50E+3", &ctx);
    dec128__convertToRawHexString(buf, &d);
    cut_assert_equal_string("A20840000000000000000000000003D0", buf);

    dec128_convertFromDecimalString(&d, "-750", &ctx);
    dec128__convertToRawHexString(buf, &d);
    cut_assert_equal_string("A20800000000000000000000000003D0", buf);

    dec128_convertFromDecimalString(&d, "-75.0", &ctx);
    dec128__convertToRawHexString(buf, &d);
    cut_assert_equal_string("A207C0000000000000000000000003D0", buf);

    dec128_convertFromDecimalString(&d, "-0.750", &ctx);
    dec128__convertToRawHexString(buf, &d);
    cut_assert_equal_string("A20740000000000000000000000003D0", buf);

    dec128_convertFromDecimalString(&d, "-0.0750", &ctx);
    dec128__convertToRawHexString(buf, &d);
    cut_assert_equal_string("A20700000000000000000000000003D0", buf);

    dec128_convertFromDecimalString(&d, "-0.000750", &ctx);
    dec128__convertToRawHexString(buf, &d);
    cut_assert_equal_string("A20680000000000000000000000003D0", buf);

    dec128_convertFromDecimalString(&d, "-0.00000750", &ctx);
    dec128__convertToRawHexString(buf, &d);
    cut_assert_equal_string("A20600000000000000000000000003D0", buf);

    dec128_convertFromDecimalString(&d, "-7.50E-7", &ctx);
    dec128__convertToRawHexString(buf, &d);
    cut_assert_equal_string("A205C0000000000000000000000003D0", buf);
}

void test_dec128_convertToExponentialDecimalString(void)
{
    dec128 d;
    DecimalContext ctx;
    char buf[256];

    dec128__initWithRawHexString(&d, "A20780000000000000000000000003D0");
    dec128_convertToExponentialDecimalString(buf, &d, &ctx);
    cut_assert_equal_string("-7.50E+0", buf);

    dec128__initWithRawHexString(&d, "A20840000000000000000000000003D0");
    dec128_convertToExponentialDecimalString(buf, &d, &ctx);
    cut_assert_equal_string("-7.50E+3", buf);

    dec128__initWithRawHexString(&d, "A20800000000000000000000000003D0");
    dec128_convertToExponentialDecimalString(buf, &d, &ctx);
    cut_assert_equal_string("-7.50E+2", buf);

    dec128__initWithRawHexString(&d, "A207C0000000000000000000000003D0");
    dec128_convertToExponentialDecimalString(buf, &d, &ctx);
    cut_assert_equal_string("-7.50E+1", buf);

    dec128__initWithRawHexString(&d, "A20740000000000000000000000003D0");
    dec128_convertToExponentialDecimalString(buf, &d, &ctx);
    cut_assert_equal_string("-7.50E-1", buf);

    dec128__initWithRawHexString(&d, "A20700000000000000000000000003D0");
    dec128_convertToExponentialDecimalString(buf, &d, &ctx);
    cut_assert_equal_string("-7.50E-2", buf);

    dec128__initWithRawHexString(&d, "A20680000000000000000000000003D0");
    dec128_convertToExponentialDecimalString(buf, &d, &ctx);
    cut_assert_equal_string("-7.50E-4", buf);

    dec128__initWithRawHexString(&d, "A20600000000000000000000000003D0");
    dec128_convertToExponentialDecimalString(buf, &d, &ctx);
    cut_assert_equal_string("-7.50E-6", buf);

    dec128__initWithRawHexString(&d, "A205c0000000000000000000000003D0");
    dec128_convertToExponentialDecimalString(buf, &d, &ctx);
    cut_assert_equal_string("-7.50E-7", buf);
}

void test_dec128_convertToOrdinaryDecimalString(void)
{
    dec128 d;
    DecimalContext ctx;
    char buf[256];

    dec128__initWithRawHexString(&d, "A20780000000000000000000000003D0");
    dec128_convertToOrdinaryDecimalString(buf, &d, &ctx);
    cut_assert_equal_string("-7.50", buf);

    dec128__initWithRawHexString(&d, "A20840000000000000000000000003D0");
    dec128_convertToOrdinaryDecimalString(buf, &d, &ctx);
    cut_assert_equal_string("-7500", buf);

    dec128__initWithRawHexString(&d, "A20800000000000000000000000003D0");
    dec128_convertToOrdinaryDecimalString(buf, &d, &ctx);
    cut_assert_equal_string("-750", buf);

    dec128__initWithRawHexString(&d, "A207C0000000000000000000000003D0");
    dec128_convertToOrdinaryDecimalString(buf, &d, &ctx);
    cut_assert_equal_string("-75.0", buf);

    dec128__initWithRawHexString(&d, "A20740000000000000000000000003D0");
    dec128_convertToOrdinaryDecimalString(buf, &d, &ctx);
    cut_assert_equal_string("-0.750", buf);

    dec128__initWithRawHexString(&d, "A20700000000000000000000000003D0");
    dec128_convertToOrdinaryDecimalString(buf, &d, &ctx);
    cut_assert_equal_string("-0.0750", buf);

    dec128__initWithRawHexString(&d, "A20680000000000000000000000003D0");
    dec128_convertToOrdinaryDecimalString(buf, &d, &ctx);
    cut_assert_equal_string("-0.000750", buf);

    dec128__initWithRawHexString(&d, "A20600000000000000000000000003D0");
    dec128_convertToOrdinaryDecimalString(buf, &d, &ctx);
    cut_assert_equal_string("-0.00000750", buf);

    dec128__initWithRawHexString(&d, "A205c0000000000000000000000003D0");
    dec128_convertToOrdinaryDecimalString(buf, &d, &ctx);
    cut_assert_equal_string("-0.000000750", buf);
}

