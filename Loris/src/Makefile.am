# Copyright (C) 1999-2004, Kelly Fitz and Lippold Haken <loris@cerlsoundgroup.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = $(INCLUDE_FFTW) $(default_includes)

EXTRA_DIST = loris.h.in

pkginclude_HEADERS = AiffData.h	\
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
				Marker.h	\
				Morpher.h	\
				NoiseGenerator.h \
				Notifier.h	\
				Oscillator.h	\
				Partial.h	\
				PartialBuilder.h	\
				PartialList.h	\
				PartialPtrs.h	\
				PartialUtils.h	\
				ReassignedSpectrum.h	\
				Resampler.h \
				SdifFile.h	\
				Sieve.h	\
				SpcFile.h	\
				SpectralPeaks.h \
				SpectralPeakSelector.h \
				Synthesizer.h

#noinst_LTLIBRARIES = libclasses.la
#libclasses_la_SOURCES 
Loris_Classes = AiffData.C	\
		AiffData.h \
		AiffFile.C \
		AiffFile.h \
		Analyzer.C \
		Analyzer.h \
		AssociateBandwidth.C \
		AssociateBandwidth.h \
		Breakpoint.C \
		BreakpointEnvelope.C \
		BreakpointEnvelope.h \
		Breakpoint.h \
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

lib_LTLIBRARIES = libloris.la
libloris_la_SOURCES = loris.h lorisAnalyzer_pi.C lorisBpEnvelope_pi.C \
 lorisException_pi.C lorisException_pi.h lorisNonObj_pi.C \
 lorisPartialList_pi.C lorisSampleVector_pi.C lorisUtilities_pi.C \
 $(Loris_Classes)

if DARWIN
# libloris_la_LDFLAGS = -version-info 3:1:0 -prebind
# automake doesn't like the conditional definition of 
# LDFLAGS, so use this to add Darwin-specific flags:
EXTRA_LD_FLAGS = -prebind
endif

libloris_la_LIBADD = $(LINK_FFTW)
libloris_la_LDFLAGS = -version-info 4:0:4 $(EXTRA_LD_FLAGS)

nodist_include_HEADERS = loris.h

