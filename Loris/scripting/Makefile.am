# Copyright (C) 1999-2004, Kelly Fitz and Lippold Haken <loris@cerlsoundgroup.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

EXTRA_DIST = loris.py loris.i lorisPartialList.i 

CLEANFILES = loris.pyo loris.pyc $(PYTHON_CMODULE)
MAINTAINERCLEANFILES = Makefile.in

PYTHON_WRAPPER = loris_python_wrap.C

#----------------------------------------------------------------
# If Python is available, define a target for building and
# installing the Python module
#
if BUILD_PYTHON
pythonsitepkgdir = $(pyexecdir)
pyexec_PYTHON = loris.py

#----------------------------------------------------------------
# Darwin is special, libtool doesn't work right.
# Wait, maybe it does now!
#
if DARWIN
EXTRA_LD_FLAGS = -bundle -flat_namespace -undefined suppress
endif

pythonsitepkg_LTLIBRARIES = _loris.la
_loris_la_SOURCES = $(PYTHON_WRAPPER)
_loris_la_CXXFLAGS = -I$(top_srcdir)/src $(PYFLAGS) 
_loris_la_LIBADD = $(top_builddir)/src/libloris.la $(LINK_FFTW)
_loris_la_LDFLAGS = -module -avoid-version $(EXTRA_LD_FLAGS)
	
endif

#----------------------------------------------------------------
# Special build and install rules for Darwin, libtool doesn't work.
#
# if DARWIN
# 
# all-local: $(PYTHON_MODULE)
# 
# install-exec-local: $(INSTALL_PYTHON_MODULE)
# 
# endif 

#----------------------------------------------------------------
# Define a target for building the
# wrapper from swig interface files.
#
SWIG = swig
SWIG_INTERFACE = loris.i

wrapper: $(PYTHON_WRAPPER)

$(PYTHON_WRAPPER): $(SWIG_INTERFACE)
	$(SWIG) -python -c++ -module loris -o $(PYTHON_WRAPPER) $(SWIG_INTERFACE)
