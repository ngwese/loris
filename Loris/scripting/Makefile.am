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
# PYTHON_CMODULE = _loris.so
# PYTHON_MODULE = $(PYTHON_CMODULE) 
# INSTALL_PYTHON_MODULE = install_python_module
# CPPFLAGS = $(CPPFLAGS) -I$(top_srcdir)/src $(PYFLAGS) 
# 
# $(PYTHON_CMODULE): loris_python_wrap.o $(top_builddir)/src/.libs/libloris.a
# 	$(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) $(LDFLAGS) \
# 		-bundle -flat_namespace -undefined suppress \
# 		-o $@ loris_python_wrap.o $(top_builddir)/src/.libs/libloris.a \
# 		$(LINK_FFTW) $(LIBS)
# 	-mkdir .libs
# 	cp $@ .libs/$@
# 
# loris_python_wrap.o: loris_python_wrap.C ../src/loris.h \
#   ../src/Exception.h ../src/Notifier.h ../src/BreakpointEnvelope.h \
#   ../src/Envelope.h ../src/Marker.h ../src/AiffFile.h \
#   ../src/Synthesizer.h ../src/Oscillator.h ../src/NoiseGenerator.h \
#   ../src/Filter.h ../src/PartialUtils.h ../src/Partial.h \
#   ../src/Breakpoint.h ../src/Analyzer.h ../src/PartialList.h \
#   ../src/SdifFile.h ../src/SpcFile.h
# 
# install_python_module:
# 	$(mkinstalldirs) $(pythonsitepkgdir)
# 	$(INSTALL) $(PYTHON_CMODULE) $(pythonsitepkgdir)/$(PYTHON_CMODULE)

# don't seem to need this, libtool works correctly, even 
# under Darwin
#	$(INSTALL) $(pyexec_PYTHON) $(pythonsitepkgdir)/$(pyexec_PYTHON)

pythonsitepkg_LTLIBRARIES = _loris.la
_loris_la_SOURCES = $(PYTHON_WRAPPER)
_loris_la_CXXFLAGS = $(AM_CXXFLAGS) -I$(top_srcdir)/src $(PYFLAGS) 
_loris_la_LDFLAGS = -module -avoid-version -bundle -flat_namespace -undefined suppress
_loris_la_LIBADD = $(top_builddir)/src/libloris.la $(LINK_FFTW)
	
else
#
# not Darwin, use libtool
#
pythonsitepkg_LTLIBRARIES = _loris.la
_loris_la_SOURCES = $(PYTHON_WRAPPER)
_loris_la_CXXFLAGS = $(AM_CXXFLAGS) -I$(top_srcdir)/src $(PYFLAGS) 
_loris_la_LDFLAGS = -module -avoid-version
_loris_la_LIBADD = $(top_builddir)/src/libloris.la $(LINK_FFTW) -lstdc++
endif

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
