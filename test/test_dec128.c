#include "dec128.h"

void test_initWithRawHexString(void)
{
    dec128 d;
    dec128__wk w;

    dec128__initWithRawHexString(&d, "A20780000000000000000000000003D0");
    dec128__wk_convertFromDec128(&w, &d);
    dec128_isCanonical(&d);
}

