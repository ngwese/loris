# Copyright (C) 2001 Kelly Fitz <kfitz@cerlsoundgroup.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

MAINTAINERCLEANFILES    = Makefile.in

INCLUDES = -I$(FFTWINCLUDE) -I$(SDIFINCLUDE) $(default_includes)
LDFLAGS = -L$(libdir)

AM_CFLAGS = $(DEBUG_LORIS)
AM_CXXFLAGS = $(DEBUG_LORIS)


pkginclude_HEADERS = AiffFile.h	\
				Analyzer.h		\
				AssociateBandwidth.h	\
				BreakpointEnvelope.h	\
				Breakpoint.h	\
				BreakpointUtils.h	\
				Channelizer.h	\
				Dilator.h	\
				Distiller.h	\
				DistributeEnergy.h	\
				Endian.h	\
				Envelope.h	\
				Exception.h	\
				Filter.h	\
				FourierTransform.h	\
				ImportLemur.h	\
				KaiserWindow.h	\
				Morpher.h	\
				Notifier.h	\
				Oscillator.h	\
				Partial.h	\
				PartialUtils.h	\
				ReassignedSpectrum.h	\
				SdifFile.h	\
				Sieve.h	\
				SpcFile.h	\
				Synthesizer.h

noinst_LTLIBRARIES = libclasses.la
libclasses_la_SOURCES = AiffFile.C \
		AiffFile.h \
		Analyzer.C \
		Analyzer.h \
		AssociateBandwidth.C \
		AssociateBandwidth.h \
		Breakpoint.C \
		BreakpointEnvelope.C \
		BreakpointEnvelope.h \
		Breakpoint.h \
		BreakpointUtils.h \
		Channelizer.C \
		Channelizer.h \
		Dilator.C \
		Dilator.h \
		Distiller.C \
		Distiller.h \
		DistributeEnergy.C \
		DistributeEnergy.h \
		Endian.C \
		Endian.h \
		Envelope.C \
		Envelope.h \
		Exception.C \
		Exception.h \
		Filter.C \
		Filter.h \
		FourierTransform.C \
		FourierTransform.h \
		ieee.c \
		ieee.h \
		ImportLemur.C \
		ImportLemur.h \
		KaiserWindow.C \
		KaiserWindow.h \
		Loris_types.h \
		Morpher.C \
		Morpher.h \
		Notifier.C \
		Notifier.h \
		Oscillator.C \
		Oscillator.h \
		Partial.C \
		Partial.h \
		PartialUtils.h \
		random.h \
		ReassignedSpectrum.C \
		ReassignedSpectrum.h \
		SdifFile.h \
		SdifFile.C \
		Sieve.C \
		Sieve.h \
		SpcFile.C \
		SpcFile.h \
		Synthesizer.C \
		Synthesizer.h

libclasses_la_LIBADD = -lfftw -lsdif

