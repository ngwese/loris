# Copyright (C) 1999-2004, Kelly Fitz and Lippold Haken <loris@cerlsoundgroup.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

lib_LTLIBRARIES = libloris.la

# source code for Loris classes
Loris_Classes = 	\
		AiffData.C	\
		AiffData.h \
		AiffFile.C \
		AiffFile.h \
		Analyzer.C \
		Analyzer.h \
		AssociateBandwidth.C \
		AssociateBandwidth.h \
		Breakpoint.C \
		Breakpoint.h \
		BreakpointEnvelope.h \
		BreakpointUtils.C \
		BreakpointUtils.h \
		Channelizer.C \
		Channelizer.h \
		Dilator.C \
		Dilator.h \
		Distiller.C \
		Distiller.h \
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
		FrequencyReference.C \
		FrequencyReference.h \
		ieee.c \
		ieee.h \
		ImportLemur.C \
		ImportLemur.h \
		KaiserWindow.C \
		KaiserWindow.h \
		LinearEnvelope.C \
		LinearEnvelope.h \
		Marker.C	\
		Marker.h	\
		Morpher.C \
		Morpher.h \
		NoiseGenerator.C \
		NoiseGenerator.h \
		Notifier.C \
		Notifier.h \
		Oscillator.C \
		Oscillator.h \
		Partial.C \
		Partial.h \
		PartialBuilder.C	\
		PartialBuilder.h	\
		PartialList.h \
		PartialPtrs.h \
		PartialUtils.C \
		PartialUtils.h \
		ReassignedSpectrum.C \
		ReassignedSpectrum.h \
		Resampler.C \
		Resampler.h \
		SdifFile.h \
		SdifFile.C \
		Sieve.h \
		Sieve.C \
		SpcFile.C \
		SpcFile.h \
		SpectralPeaks.h \
		SpectralPeakSelector.C \
		SpectralPeakSelector.h \
		Synthesizer.C \
		Synthesizer.h

libloris_la_SOURCES = loris.h lorisAnalyzer_pi.C lorisBpEnvelope_pi.C \
 lorisException_pi.C lorisException_pi.h lorisNonObj_pi.C \
 lorisPartialList_pi.C lorisUtilities_pi.C \
 $(Loris_Classes)
libloris_la_CPPFLAGS = $(INCLUDE_FFTW) $(default_includes)
libloris_la_LIBADD = $(LINK_FFTW)

if DARWIN
# automake doesn't like the conditional definition of 
# LDFLAGS, so use this to add Darwin-specific flags:
EXTRA_LD_FLAGS = -prebind
endif

# the library version for Loris 1.2 is 4:0:0
libloris_la_LDFLAGS = -version-info 4:0:0 $(EXTRA_LD_FLAGS)

# loris.h is generated automatically from loris.h.in
nodist_include_HEADERS = loris.h
EXTRA_DIST = loris.h.in

# installed Loris header files
pkginclude_HEADERS = \
				AiffFile.h		\
				Analyzer.h		\
				BreakpointEnvelope.h	\
				Breakpoint.h	\
				BreakpointUtils.h	\
				Channelizer.h	\
				Dilator.h	\
				Distiller.h	\
				Envelope.h	\
				Exception.h	\
				Filter.h	\
				FourierTransform.h	\
				FrequencyReference.h \
				ImportLemur.h	\
				KaiserWindow.h	\
            LinearEnvelope.h \
				Marker.h	\
				Morpher.h	\
				NoiseGenerator.h \
				Notifier.h	\
				Oscillator.h	\
				Partial.h	\
				PartialList.h	\
				PartialPtrs.h	\
				PartialUtils.h	\
				ReassignedSpectrum.h	\
				Resampler.h \
				SdifFile.h	\
				Sieve.h	\
				SpcFile.h	\
				Synthesizer.h

MAINTAINERCLEANFILES = Makefile.in

