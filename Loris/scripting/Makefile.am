# Copyright (C) 1999-2001, Kelly Fitz and Lippold Haken <loris@cerlsoundgroup.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

INCLUDES = $(PYINCLUDE) $(TCLINCLUDE) $(PERLINCLUDE) -I$(top_srcdir)/src -I$(top_builddir)/pi

EXTRA_DIST = loris.py loris.pm loris.i lorisPartialList.i 

CLEANFILES = loris.pyo loris.pyc $(PYTHON_CMODULE) $(TCL_MODULE)
MAINTAINERCLEANFILES = Makefile.in

PYTHON_WRAPPER = loris_python_wrap.C
TCL_WRAPPER = loris_tcl_wrap.C
PERL_WRAPPER = loris_perl_wrap.C

#----------------------------------------------------------------
# If Python is available, define a target for building and
# installing the Python module
#
if BUILD_PYTHON
pythonsitepkgdir = $(pyexecdir)
pyexec_PYTHON = loris.py

#----------------------------------------------------------------
# Darwin is special, libtool doesn't work right.
#
if DARWIN
PYTHON_CMODULE = _loris.so
PYTHON_MODULE = $(PYTHON_CMODULE) 
INSTALL_PYTHON_MODULE = install_python_module

$(PYTHON_CMODULE): loris_python_wrap.o $(top_builddir)/pi/.libs/libloris.a
	$(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) $(LDFLAGS) \
		-bundle -flat_namespace -undefined suppress \
		-o $@ loris_python_wrap.o $(top_builddir)/pi/.libs/libloris.a \
		-lsdif -lfftw $(LIBS)
	-mkdir .libs
	cp $@ .libs/$@

install_python_module:
	$(INSTALL) _loris.so $(pythonsitepkgdir)/_loris.so
	
else
#
# not Darwin, use libtool
#
pythonsitepkg_LTLIBRARIES = _loris.la
_loris_la_SOURCES = $(PYTHON_WRAPPER)
_loris_la_LDFLAGS = -module -avoid-version
_loris_la_LIBADD = $(top_builddir)/pi/libloris.la -lstdc++
endif

endif


#----------------------------------------------------------------
# If Tcl is available, define a target for building and
# installing the Tcl module
#
if BUILD_TCL
tclsitepkgdir = $(pkglibdir)/tcl

#----------------------------------------------------------------
# Darwin is special, libtool doesn't work right.
#
if DARWIN
TCL_MODULE = tcLoris.so
INSTALL_TCL_MODULE = install_tcl_module

$(TCL_MODULE): loris_tcl_wrap.o $(top_builddir)/pi/.libs/libloris.a
	$(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) $(LDFLAGS) \
		-dynamiclib -flat_namespace -undefined suppress \
		-o $@ loris_tcl_wrap.o $(top_builddir)/pi/.libs/libloris.a \
		-lsdif -lfftw $(LIBS)
	-mkdir .libs
	cp $@ .libs/$@

install_tcl_module:
	$(INSTALL) tcLoris.so $(tclsitepkgdir)/$(TCL_MODULE)

else
#
# not Darwin, use libtool
#
tclsitepkg_LTLIBRARIES = tcLoris.la
tcLoris_la_SOURCES = $(TCL_WRAPPER)
tcLoris_la_LDFLAGS = -module -avoid-version
tcLoris_la_LIBADD = $(top_builddir)/pi/libloris.la -lstdc++
endif

endif

#----------------------------------------------------------------
# If the user insists, build the Perl module. It is untested.
# Do not install it.
#
if BUILD_PERL

noinst_LTLIBRARIES = perLoris.la
noinst_SCRIPTS = perLoris.pm

perLoris_la_SOURCES = $(PERL_WRAPPER)
perLoris_la_LDFLAGS = -module -avoid-version
perLoris_la_LIBADD = $(top_builddir)/pi/libloris.la -lstdc++

endif

#----------------------------------------------------------------
# Special build and install rules for Darwin, libtool doesn't work.
#
if DARWIN

all-local: $(PYTHON_MODULE) $(TCL_MODULE) $(PERL_MODULE)

install-exec-local: $(INSTALL_PYTHON_MODULE) $(INSTALL_TCL_MODULE) $(INSTALL_PERL_MODULE)

endif 

#----------------------------------------------------------------
# If swig is available, define a target for building the
# wrappers from swig interface files.
#
if HAVE_SWIG
SWIG_INTERFACE = loris.i

wrappers: 
	$(SWIG) -python -c++ -o $(PYTHON_WRAPPER) $(SWIG_INTERFACE)
	$(SWIG) -tcl -c++ -o $(TCL_WRAPPER) $(SWIG_INTERFACE)
	$(SWIG) -perl -c++ -o $(PERL_WRAPPER) $(SWIG_INTERFACE)
endif
