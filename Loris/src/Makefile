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
			BreakpointEnvelope.o \
			Dilator.o \
			Distiller.o \
			DistributeEnergy.o \
			Endian.o \
			Envelope.o \
			Exception.o \
			ExportSdif.o \
			ExportSpc.o \
			Filter.o \
			FourierTransform.o \
			Handle.o \
			ImportLemur.o \
			ImportSdif.o \
			KaiserWindow.o \
			Morpher.o \
			Oscillator.o \
			Partial.o \
			PartialView.o \
			ReassignedSpectrum.o \
			Synthesizer.o \
			ieee.o \
			notifier.o

OBJ_SDIF = sdif.o sdif-types.o

#
#	targets:
#
all: classes sdif

classes: ${OBJ_LORIS}
	echo "checked Loris objects"

#	SDIF objects:
sdif: $(OBJ_SDIF)
	echo "checked SDIF objects"

sdif.o: 
	$(cc) $(cFLAGS) -c $(SDIF)/sdif.c

sdif-types.o:
	$(cc) $(cFLAGS) -c $(SDIF)/sdif-types.c	

#
#	dependencies:
#	need the sed thing because g++ doesn't recognize c++ suffix,
#	so it does the wrong thing with the file names (.C.o)
#
depend: 
	${CC} -MM $(CFLAGS) *.C | sed 's/c++\.o/\o/g' > dependfile
	${cc} -MM $(cFLAGS) *.c >> dependfile
	${cc} -MM $(cFLAGS) $(SDIF)/sdif.c $(SDIF)/sdif-types.c >> dependfile

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
