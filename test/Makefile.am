TESTS = run-test.sh
TESTS_ENVIRONMENT = NO_MAKE=yes CUTTER="$(CUTTER)"

noinst_LTLIBRARIES = test_decimal.la

INCLUDES = $(CUTTER_CFLAGS) -I$(top_srcdir)/src
LIBS = $(CUTTER_LIBS) $(top_builddir)/src/libdecimal.la

LDFLAGS = -module -rpath $(libdir) -avoid-version -no-undefined
test_decimal_la_SOURCES = test_decimal.c test_decimalInt.c test_dec128.c test_declet.c

echo-cutter:
	@echo $(CUTTER)
