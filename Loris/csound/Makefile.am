# Copyright (C) 1999-2004, Kelly Fitz and Lippold Haken <loris@cerlsoundgroup.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

EXTRA_DIST = lorisgens.C lorisgens.h tryit.csd trymorph.csd flute.sdif trombone.sdif meow.sdif

MAINTAINERCLEANFILES = 	Makefile.in

if HAVE_CSOUND
csoundlibdir = $(libdir)/csound
csoundlib_LTLIBRARIES = lorisops.la
lorisops_la_SOURCES = lorisgens.C
lorisops_CXXFLAGS = $(CSOUND_CXXFLAGS) $(AM_CXXFLAGS) -I$(top_srcdir)/src
lorisops_la_LDFLAGS = -module -avoid-version -no-undefined
lorisops_la_LIBADD = $(top_builddir)/src/libloris.la
endif
