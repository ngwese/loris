# Copyright (C) 1999-2004, Kelly Fitz and Lippold Haken <loris@cerlsoundgroup.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

bin_PROGRAMS  = loris_spewmarkers

INCLUDES = -I$(top_srcdir)/src

# loris_spewmarkers: a utility program to read the
# markers stored in a AIFF, SPC, or SDIF file and
# print them to standard output.
loris_spewmarkers_SOURCES = loris_spewmarkers.C
if DARWIN
loris_spewmarkers_LDADD = $(top_builddir)/src/.libs/libloris.a
else
loris_spewmarkers_LDADD = $(top_builddir)/src/libloris.la
endif
loris_spewmarkers_LDFLAGS = -static

MAINTAINERCLEANFILES = 	Makefile.in

