# Copyright (C) 1999-2004, Kelly Fitz and Lippold Haken <loris@cerlsoundgroup.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

bin_PROGRAMS  = loris_analyze loris_synthesize loris_spewmarkers loris_mark loris_unmark

INCLUDES = -I$(top_srcdir)/src

# loris_analyze: a utility program to analyze
# samples and store the partials in an SDIF file.
loris_analyze_SOURCES = loris_analyze.C
if DARWIN
loris_analyze_LDADD = $(top_builddir)/src/.libs/libloris.a $(LINK_FFTW)
else
loris_analyze_LDADD = $(top_builddir)/src/libloris.la $(LINK_FFTW)
endif
loris_analyze_LDFLAGS = -static

# loris_synthesize: a utility program to render Partials
# stored in a SDIF file, optionally with dilation and amplitude
# and frequency scaling by a constant factor.
loris_synthesize_SOURCES = loris_synthesize.C
if DARWIN
loris_synthesize_LDADD = $(top_builddir)/src/.libs/libloris.a
else
loris_synthesize_LDADD = $(top_builddir)/src/libloris.la
endif
loris_synthesize_LDFLAGS = -static

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

# loris_mark: a utility program to add a
# marker to a AIFF, SPC, or SDIF file.
loris_mark_SOURCES = loris_mark.C
if DARWIN
loris_mark_LDADD = $(top_builddir)/src/.libs/libloris.a
else
loris_mark_LDADD = $(top_builddir)/src/libloris.la
endif
loris_mark_LDFLAGS = -static


# loris_mark: a utility program to remove a
# marker or markers from a AIFF, SPC, or SDIF file.
loris_unmark_SOURCES = loris_unmark.C
if DARWIN
loris_unmark_LDADD = $(top_builddir)/src/.libs/libloris.a
else
loris_unmark_LDADD = $(top_builddir)/src/libloris.la
endif
loris_unmark_LDFLAGS = -static


MAINTAINERCLEANFILES = 	Makefile.in

