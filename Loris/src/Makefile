#!gmake
#
#	Makefile for building the Loris class library.
#
TOP = ../..
include Rules.loris

#
#	tools:
#
cc = gcc	
CC = g++

#
#	flags:
#
Includes = $(INC_LORIS)
Debugging = $(DEBUG_LORIS)

CFLAGS	:= 	-O2 $(Includes) $(Debugging) 
cFLAGS	:=	$(CFLAGS)

#
#	suffixes:
#
.SUFFIXES: .c .C .o

#
#	sources:
#	(redefine here because gmake does the wrong thing
#	when checking files with a path prefix)
#
OBJ_LORIS = AiffFile.o \
			Analyzer.o \
			AssociateBandwidth.o \
			Breakpoint.o \
			Dilator.o \
			Distiller.o \
			DistributeEnergy.o \
			Endian.o \
			Exception.o \
			Filter.o \
			FourierTransform.o \
			ImportLemur.o \
			KaiserWindow.o \
			Map.o \
			Morph.o \
			Oscillator.o \
			Partial.o \
			PartialView.o \
			ReassignedSpectrum.o \
			Synthesizer.o \
			ieee.o \
			notifier.o \
			SpcFile.o \
			SdifReader.o \
			SdifWriter.o \
#			FloatingThreshold.o \


#
#	targets:
#
classes: ${OBJ_LORIS}

# this should use gcc probably
sdif: 
	cd $(SDIF); $(MAKE) sdif.o sdif-types.o 

#
#	dependencies:
#	doesn't work because g++ doesn't recognize c++ suffix,
#	so it does the wrong thing with the file names (.C.o)
#
depend: 
	${CC} -MM $(CFLAGS) *.C > dependfile
	${cc} -MM $(cFLAGS) *.c >> dependfile

include dependfile

#
#	admin:
#
clean:
	-rm *.o

.C.o:
	$(CC) $(CFLAGS) -c $< 

.c.o:
	$(cc) $(cFLAGS) -c $<
