#!gmake
#
#   This is the Loris C++ Class Library, implementing analysis, 
#   manipulation, and synthesis of digitized sounds using the Reassigned 
#   Bandwidth-Enhanced Additive Sound Model.
#   
#   Loris is Copyright (c) 1999-2000 by Kelly Fitz and Lippold Haken
#  
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#  
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY, without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#   GNU General Public License for more details.
#  
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#  
#  
#   Makefile for building the Loris class library.
#  
#   Kelly Fitz, Jan 2000
#   loris@cerlsoundgroup.org
#  
#   http://www.cerlsoundgroup.org/Loris/
#  
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

CFLAGS	:= 	-O2 $(Includes) $(Debugging) -Wno-multichar
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
			Channelizer.o \
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
			ReassignedSpectrum.o \
			Synthesizer.o \
			ieee.o \
			notifier.o
#
#	targets:
#
classes: ${OBJ_LORIS}

#
#	dependencies:
#	need the sed thing because g++ doesn't recognize c++ suffix,
#	so it does the wrong thing with the file names (.C.o)
#
depend: 
	${CC} -MM $(CFLAGS) *.C | sed 's/c++\.o/\o/g' > dependfile
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
