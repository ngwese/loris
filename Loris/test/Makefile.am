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

MAINTAINERCLEANFILES = 	Makefile.in

INCLUDES = -I$(top_srcdir)/src

# simple morphing test
test_c_SOURCES = morphtest.C
test_c_LDADD = $(top_builddir)/pi/libloris.la

# Morpher unit tests
test_morpher_SOURCES = test_Morpher.C
# Darwin is special, dynamic linking sometimes seems to fail 
# on this test, can't figure it out.
if DARWIN
test_morpher_LDADD = $(top_builddir)/pi/.libs/libloris.a
else
test_morpher_LDADD = $(top_builddir)/pi/libloris.la
endif

# Partial unit tests
test_partial_SOURCES = test_Partial.C
# Darwin is special, dynamic linking sometimes seems to fail 
# on this test, can't figure it out.
if DARWIN
test_partial_LDADD = $(top_builddir)/pi/.libs/libloris.a
else
test_partial_LDADD = $(top_builddir)/pi/libloris.la
endif

# Distiller unit tests
test_distiller_SOURCES = test_Distiller.C
if DARWIN
test_distiller_LDADD = $(top_builddir)/pi/.libs/libloris.a
else
test_distiller_LDADD = $(top_builddir)/pi/libloris.la
endif

# SdifFile unit tests
test_sdiffile_SOURCES = test_SdifFile.C
if DARWIN
test_sdiffile_LDADD = $(top_builddir)/pi/.libs/libloris.a -lsdif
else
test_sdiffile_LDADD = $(top_builddir)/pi/libloris.la
endif

if BUILD_PYTHON
PYTHON_TEST = run_pytest
$(PYTHON_TEST):
	echo "env PYTHONPATH=$(top_srcdir)/scripting:$(top_builddir)/scripting/.libs \
			$(PYTHON) $(top_srcdir)/test/morphtest.py" >> $@
	chmod +x $@
endif

if BUILD_TCL
TCL_TEST = run_tcltest
$(TCL_TEST):
	echo "env srcdir=$(srcdir) top_builddir=$(top_builddir) \
		$(TCLSH) $(top_srcdir)/test/morphtest.tcl" >> $@
	chmod +x $@
endif

if BUILD_PERL
PERL_TEST = run_perltest
$(PERL_TEST):
	echo "$(PERL) -I$(top_srcdir)/scripting:$(top_builddir)/scripting/.libs \
			 -e 'use perLoris; print perLoris::version, \"\n\";'" >> $@
	chmod +x $@
endif

check_PROGRAMS = test_partial test_distiller test_sdiffile test_morpher test_c
check_SCRIPTS = $(PYTHON_TEST) $(TCL_TEST) $(PERL_TEST)

TESTS = $(check_PROGRAMS) $(check_SCRIPTS) 

CLEANFILES = $(PYTHON_TEST) $(TCL_TEST) $(PERL_TEST)

distclean-local:
	-rm -fr *.ctest.* *.pytest.* *.tcltest.* tmp.sdif
