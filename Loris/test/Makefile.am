EXTRA_DIST = clarinet.aiff flute.aiff morphtest.py morphtest.tcl

check_PROGRAMS = test_c
check_SCRIPTS = run_pytest run_tcltest

TESTS = $(check_PROGRAMS) $(check_SCRIPTS) 

TESTS_ENVIRONMENT = PYTHON=$(PYTHON) \
					PYTHONPATH=$(top_srcdir)/scripting:$(top_builddir)/scripting/.libs

INCLUDES = -I$(top_srcdir)/src

test_c_SOURCES = morphtest.C
test_c_LDADD = $(top_builddir)/pi/libloris.la

if HAVE_PYTHON_H
run_pytest:
	echo "$(PYTHON) morphtest.py" >> $@
	chmod +x $@
endif

if HAVE_TCL_H
run_tcltest:
	echo "$(TCL) morphtest.tcl $(top_builddir)/scripting/.libs" >> $@
	chmod +x $@
endif

distclean-local:
	-rm -fr run_pytest run_tcltest
	-rm -fr *.ctest.* *.pytest.* *.tcltest.*