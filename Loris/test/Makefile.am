EXTRA_DIST = clarinet.aiff flute.aiff

check_PROGRAMS = test1
TESTS = $(check_PROGRAMS)

INCLUDES = -I$(top_srcdir)/src

test1_SOURCES = morphtest.C
test1_LDADD = $(top_builddir)/pi/libloris.la
