EXTRA_DIST = clarinet.aiff flute.aiff

check_PROGRAMS = test_c
check_SCRIPTS = 

# runpy morphtest.py
TESTS = $(check_SCRIPTS) $(check_PROGRAMS)

TESTS_ENVIRONMENT = PYTHON=$(PYTHON) \
					PYTHONPATH=$(top_srcdir)/scripting:$(top_builddir)/scripting/.libs

INCLUDES = -I$(top_srcdir)/src

test_c_SOURCES = morphtest.C
test_c_LDADD = $(top_builddir)/pi/libloris.la
