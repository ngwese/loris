# Copyright (C) 1999-2001, Kelly Fitz and Lippold Haken <loris@cerlsoundgroup.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

INCLUDES =  $(PYINCLUDE) $(TCLINCLUDE) $(PERLINCLUDE) -I$(top_srcdir)/src -I$(top_builddir)/pi

EXTRA_DIST = loris.py loris.pm loris.i lorisAiffFile.i lorisAnalyzer.i lorisBpEnvelope.i lorisPartialList.i lorisSampleVector.i

CLEANFILES = loris.pyo loris.pyc
MAINTAINERCLEANFILES = Makefile.in

PYTHON_WRAPPER = loris_python_wrap.C
TCL_WRAPPER = loris_tcl_wrap.C
PERL_WRAPPER = loris_perl_wrap.C

#----------------------------------------------------------------
# If Python is available, define a target for building and
# installing the Python module
#
if BUILD_PYTHON
if HAVE_SITEPY
pythonsitepkgdir = $(SITEPY)
else
pythonsitepkgdir = $(pkglibdir)/python
endif
# Darwin is special, see below
if DARWIN
else
pythonsitepkg_SCRIPTS = loris.py loris.pyo
pythonsitepkg_LTLIBRARIES = _loris.la
endif

# This should be an install hook, but on my machine, the
# install hook does not execute _after_ everything is 
# installed, as it is documented to do, it executes before
# loris.py and _loris.a are installed, and thus fails.
loris.pyo: loris.py
	env PYTHONPATH=$(srcdir):.libs $(PYTHON) -O -c "import loris"
	
endif

# old versions of automake don't like these things
# defined conditionally:
_loris_la_SOURCES = $(PYTHON_WRAPPER)
_loris_la_LDFLAGS = -module -avoid-version
_loris_la_LIBADD = $(top_builddir)/pi/libloris.la -lstdc++ 


#----------------------------------------------------------------
# If Tcl is available, define a target for building and
# installing the PythonTcl module
#
if BUILD_TCL
tclsitepkgdir = $(pkglibdir)/tcl
# Darwin is special, see below
if DARWIN
else
tclsitepkg_LTLIBRARIES = tcLoris.la
endif
endif

# old versions of automake don't like these things
# defined conditionally:
tcLoris_la_SOURCES = $(TCL_WRAPPER)
tcLoris_la_LDFLAGS = -module -avoid-version
tcLoris_la_LIBADD = $(top_builddir)/pi/libloris.la -lstdc++


#----------------------------------------------------------------
# Don't build or install the Perl module, it is untested.
#
#if BUILD_PERL
#
#noinst_LTLIBRARIES = loris_perl.la
#noinst_SCRIPTS = loris.pm
#
#loris_perl_la_SOURCES = $(PERL_WRAPPER)
#loris_perl_la_LDFLAGS = -module -avoid-version
#loris_perl_la_LIBADD = $(top_builddir)/pi/libloris.la -lstdc++
#
#endif

#----------------------------------------------------------------
# Special build rules for Darwin, because libtool doesn't work.
#
if DARWIN

LD_DARWINBUNDLE_FLAGS = -bundle -flat_namespace -undefined suppress

if BUILD_PYTHON
PYTHON_CMODULE = _loris.so
PYTHON_MODULE = $(PYTHON_CMODULE) loris.pyo
INSTALL_PYTHON_MODULE = install_python_module

$(PYTHON_CMODULE): loris_python_wrap.o $(top_builddir)/pi/.libs/libloris.a
	$(CXXLD) $(LD_DARWINBUNDLE_FLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@ loris_python_wrap.o $(top_builddir)/pi/.libs/libloris.a -lsdif -lfftw -lstdc++ $(LIBS)
	-mkdir .libs
	cp $@ .libs/$@

install_python_module:
	$(INSTALL) _loris.so $(pythonsitepkgdir)/_loris.so
	$(INSTALL_SCRIPT) loris.py $(pythonsitepkgdir)/loris.py
	$(INSTALL_SCRIPT) loris.pyo $(pythonsitepkgdir)/loris.pyo

endif

if BUILD_TCL
TCL_MODULE = tcLoris.so
INSTALL_TCL_MODULE = install_tcl_module

$(TCL_MODULE): loris_tcl_wrap.o $(top_builddir)/pi/.libs/libloris.a
	$(CXXLD) $(LD_DARWINBUNDLE_FLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@ loris_tcl_wrap.o $(top_builddir)/pi/.libs/libloris.a -lsdif -lfftw -lstdc++ $(LIBS)
	-mkdir .libs
	cp $@ .libs/$@

install_tcl_module:
	$(INSTALL) tcLoris.so $(tclsitepkgdir)/$(TCL_MODULE)
endif

all-local: $(PYTHON_MODULE) $(TCL_MODULE)

install-exec-local: $(INSTALL_PYTHON_MODULE) $(INSTALL_TCL_MODULE)

endif 

#----------------------------------------------------------------
# If swig is available, define a target for building the
# wrappers from swig interface files.
#
if HAVE_SWIG
SWIG_INTERFACE = loris.i

wrappers: 
	$(SWIG) -python -shadow -c++ -o $(PYTHON_WRAPPER) $(SWIG_INTERFACE)
	$(SWIG) -tcl8 -c++ -o $(TCL_WRAPPER) $(SWIG_INTERFACE)
	$(SWIG) -perl5 -c++ -o $(PERL_WRAPPER) $(SWIG_INTERFACE)
	mv loris_perl.pm loris.pm
endif
