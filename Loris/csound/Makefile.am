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

INCLUDES = -I$(top_srcdir)/src -I$(CSINCLUDE)

if HAVE_CSOUND
lib_LTLIBRARIES = lorisgens.la
lorisgens_la_SOURCES = lorisgens.C
lorisgens_la_CXXFLAGS = $(AM_CXXFLAGS)
lorisgens_la_LDFLAGS = -module -avoid-version
lorisgens_la_LIBADD = $(top_builddir)/src/libloris.la -lstdc++
endif
