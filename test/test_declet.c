#include <cutter.h>
#include "decimalInt.h"

void test_decimal__decodeDecletToInt(void)
{
    cut_assert_equal_uint(0, decimal__decodeDecletToInt(0x000));
    cut_assert_equal_uint(9, decimal__decodeDecletToInt(0x009));
    cut_assert_equal_uint(10, decimal__decodeDecletToInt(0x010));
    cut_assert_equal_uint(19, decimal__decodeDecletToInt(0x019));
    cut_assert_equal_uint(20, decimal__decodeDecletToInt(0x020));
    cut_assert_equal_uint(29, decimal__decodeDecletToInt(0x029));
    cut_assert_equal_uint(30, decimal__decodeDecletToInt(0x030));
    cut_assert_equal_uint(39, decimal__decodeDecletToInt(0x039));
    cut_assert_equal_uint(40, decimal__decodeDecletToInt(0x040));
    cut_assert_equal_uint(49, decimal__decodeDecletToInt(0x049));
    cut_assert_equal_uint(50, decimal__decodeDecletToInt(0x050));
    cut_assert_equal_uint(59, decimal__decodeDecletToInt(0x059));
    cut_assert_equal_uint(60, decimal__decodeDecletToInt(0x060));
    cut_assert_equal_uint(69, decimal__decodeDecletToInt(0x069));
    cut_assert_equal_uint(70, decimal__decodeDecletToInt(0x070));
    cut_assert_equal_uint(71, decimal__decodeDecletToInt(0x071));
    cut_assert_equal_uint(72, decimal__decodeDecletToInt(0x072));
    cut_assert_equal_uint(73, decimal__decodeDecletToInt(0x073));
    cut_assert_equal_uint(74, decimal__decodeDecletToInt(0x074));
    cut_assert_equal_uint(75, decimal__decodeDecletToInt(0x075));
    cut_assert_equal_uint(76, decimal__decodeDecletToInt(0x076));
    cut_assert_equal_uint(77, decimal__decodeDecletToInt(0x077));
    cut_assert_equal_uint(78, decimal__decodeDecletToInt(0x078));
    cut_assert_equal_uint(79, decimal__decodeDecletToInt(0x079));

    cut_assert_equal_uint(994, decimal__decodeDecletToInt(0x29E));
    cut_assert_equal_uint(995, decimal__decodeDecletToInt(0x29F));
    cut_assert_equal_uint(520, decimal__decodeDecletToInt(0x2A0));
    cut_assert_equal_uint(521, decimal__decodeDecletToInt(0x2A1));

    cut_assert_equal_uint(777, decimal__decodeDecletToInt(0x3F7));
    cut_assert_equal_uint(778, decimal__decodeDecletToInt(0x3F8));
    cut_assert_equal_uint(787, decimal__decodeDecletToInt(0x3EB));
    cut_assert_equal_uint(877, decimal__decodeDecletToInt(0x37D));
    cut_assert_equal_uint(997, decimal__decodeDecletToInt(0x39F));
    cut_assert_equal_uint(979, decimal__decodeDecletToInt(0x3BF));
    cut_assert_equal_uint(799, decimal__decodeDecletToInt(0x3DF));

    cut_assert_equal_uint(888, decimal__decodeDecletToInt(0x06E));
    cut_assert_equal_uint(888, decimal__decodeDecletToInt(0x16E));
    cut_assert_equal_uint(888, decimal__decodeDecletToInt(0x26E));
    cut_assert_equal_uint(888, decimal__decodeDecletToInt(0x36E));
    cut_assert_equal_uint(889, decimal__decodeDecletToInt(0x06F));
    cut_assert_equal_uint(889, decimal__decodeDecletToInt(0x16F));
    cut_assert_equal_uint(889, decimal__decodeDecletToInt(0x26F));
    cut_assert_equal_uint(889, decimal__decodeDecletToInt(0x36F));

    cut_assert_equal_uint(898, decimal__decodeDecletToInt(0x07E));
    cut_assert_equal_uint(898, decimal__decodeDecletToInt(0x17E));
    cut_assert_equal_uint(898, decimal__decodeDecletToInt(0x27E));
    cut_assert_equal_uint(898, decimal__decodeDecletToInt(0x37E));
    cut_assert_equal_uint(899, decimal__decodeDecletToInt(0x07F));
    cut_assert_equal_uint(899, decimal__decodeDecletToInt(0x17F));
    cut_assert_equal_uint(899, decimal__decodeDecletToInt(0x27F));
    cut_assert_equal_uint(899, decimal__decodeDecletToInt(0x37F));

    cut_assert_equal_uint(988, decimal__decodeDecletToInt(0x0EE));
    cut_assert_equal_uint(988, decimal__decodeDecletToInt(0x1EE));
    cut_assert_equal_uint(988, decimal__decodeDecletToInt(0x2EE));
    cut_assert_equal_uint(988, decimal__decodeDecletToInt(0x3EE));
    cut_assert_equal_uint(989, decimal__decodeDecletToInt(0x0EF));
    cut_assert_equal_uint(989, decimal__decodeDecletToInt(0x1EF));
    cut_assert_equal_uint(989, decimal__decodeDecletToInt(0x2EF));
    cut_assert_equal_uint(989, decimal__decodeDecletToInt(0x3EF));

    cut_assert_equal_uint(998, decimal__decodeDecletToInt(0x0FE));
    cut_assert_equal_uint(998, decimal__decodeDecletToInt(0x1FE));
    cut_assert_equal_uint(998, decimal__decodeDecletToInt(0x2FE));
    cut_assert_equal_uint(998, decimal__decodeDecletToInt(0x3FE));
    cut_assert_equal_uint(999, decimal__decodeDecletToInt(0x0FF));
    cut_assert_equal_uint(999, decimal__decodeDecletToInt(0x1FF));
    cut_assert_equal_uint(999, decimal__decodeDecletToInt(0x2FF));
    cut_assert_equal_uint(999, decimal__decodeDecletToInt(0x3FF));
}

void test_decimal__encodeDecletFromInt(void)
{
    cut_assert_equal_uint(0x000, decimal__encodeDecletFromInt(0));
    cut_assert_equal_uint(0x009, decimal__encodeDecletFromInt(9));
    cut_assert_equal_uint(0x010, decimal__encodeDecletFromInt(10));
    cut_assert_equal_uint(0x019, decimal__encodeDecletFromInt(19));
    cut_assert_equal_uint(0x020, decimal__encodeDecletFromInt(20));
    cut_assert_equal_uint(0x029, decimal__encodeDecletFromInt(29));
    cut_assert_equal_uint(0x030, decimal__encodeDecletFromInt(30));
    cut_assert_equal_uint(0x039, decimal__encodeDecletFromInt(39));
    cut_assert_equal_uint(0x040, decimal__encodeDecletFromInt(40));
    cut_assert_equal_uint(0x049, decimal__encodeDecletFromInt(49));
    cut_assert_equal_uint(0x050, decimal__encodeDecletFromInt(50));
    cut_assert_equal_uint(0x059, decimal__encodeDecletFromInt(59));
    cut_assert_equal_uint(0x060, decimal__encodeDecletFromInt(60));
    cut_assert_equal_uint(0x069, decimal__encodeDecletFromInt(69));
    cut_assert_equal_uint(0x070, decimal__encodeDecletFromInt(70));
    cut_assert_equal_uint(0x071, decimal__encodeDecletFromInt(71));
    cut_assert_equal_uint(0x072, decimal__encodeDecletFromInt(72));
    cut_assert_equal_uint(0x073, decimal__encodeDecletFromInt(73));
    cut_assert_equal_uint(0x074, decimal__encodeDecletFromInt(74));
    cut_assert_equal_uint(0x075, decimal__encodeDecletFromInt(75));
    cut_assert_equal_uint(0x076, decimal__encodeDecletFromInt(76));
    cut_assert_equal_uint(0x077, decimal__encodeDecletFromInt(77));
    cut_assert_equal_uint(0x078, decimal__encodeDecletFromInt(78));
    cut_assert_equal_uint(0x079, decimal__encodeDecletFromInt(79));

    cut_assert_equal_uint(0x29E, decimal__encodeDecletFromInt(994));
    cut_assert_equal_uint(0x29F, decimal__encodeDecletFromInt(995));
    cut_assert_equal_uint(0x2A0, decimal__encodeDecletFromInt(520));
    cut_assert_equal_uint(0x2A1, decimal__encodeDecletFromInt(521));

    cut_assert_equal_uint(0x3F7, decimal__encodeDecletFromInt(777));
    cut_assert_equal_uint(0x3F8, decimal__encodeDecletFromInt(778));
    cut_assert_equal_uint(0x3EB, decimal__encodeDecletFromInt(787));
    cut_assert_equal_uint(0x37D, decimal__encodeDecletFromInt(877));
    cut_assert_equal_uint(0x39F, decimal__encodeDecletFromInt(997));
    cut_assert_equal_uint(0x3BF, decimal__encodeDecletFromInt(979));
    cut_assert_equal_uint(0x3DF, decimal__encodeDecletFromInt(799));

    cut_assert_equal_uint(0x06E, decimal__encodeDecletFromInt(888));
    cut_assert_equal_uint(0x06F, decimal__encodeDecletFromInt(889));

    cut_assert_equal_uint(0x07E, decimal__encodeDecletFromInt(898));
    cut_assert_equal_uint(0x07F, decimal__encodeDecletFromInt(899));

    cut_assert_equal_uint(0x0EE, decimal__encodeDecletFromInt(988));
    cut_assert_equal_uint(0x0EF, decimal__encodeDecletFromInt(989));

    cut_assert_equal_uint(0x0FE, decimal__encodeDecletFromInt(998));
    cut_assert_equal_uint(0x0FF, decimal__encodeDecletFromInt(999));
}
