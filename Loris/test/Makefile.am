# Copyright (C) 1999-2001, Kelly Fitz and Lippold Haken <loris@cerlsoundgroup.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

EXTRA_DIST = clarinet.aiff flute.aiff morphtest.py morphtest.tcl

INCLUDES = -I$(top_srcdir)/src

test_c_SOURCES = morphtest.C
test_c_LDADD = $(top_builddir)/pi/libloris.la

if BUILD_PYTHON
PYTHON_TEST = run_pytest
$(PYTHON_TEST):
	echo "$(PYTHON) $(top_srcdir)/test/morphtest.py" >> $@
	chmod +x $@
endif

if BUILD_TCL
TCL_TEST = run_tcltest
$(TCL_TEST):
	echo "$(TCL) $(top_srcdir)/test/morphtest.tcl" >> $@
	chmod +x $@
endif

check_PROGRAMS = test_c
check_SCRIPTS = $(PYTHON_TEST) $(TCL_TEST)

TESTS = $(check_PROGRAMS) $(check_SCRIPTS) 

TESTS_ENVIRONMENT = PYTHON=$(PYTHON) \
					PYTHONPATH=$(top_srcdir)/scripting:$(top_builddir)/scripting/.libs \
					top_builddir=$(top_builddir)

CLEANFILES = $(PYTHON_TEST) $(TCL_TEST)

distclean-local:
	-rm -fr *.ctest.* *.pytest.* *.tcltest.*
