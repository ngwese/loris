#!/usr/local/bin/gmake -j3
#
#	Makefile for building the Loris class library.
#

#
#	directories:
#
LorisRoot	:= /usr/people/kfitz/Loris
ClasslibDir	:= $(LorisRoot)/classlib
ClasslibCommon	:= $(ClasslibDir)/common
ClasslibUnix	:= $(ClasslibDir)/unix
Clients		:= $(LorisRoot)/clients

#
#	tools:
#
cc	:= cc	
CC	:= CC
AR	:= CC -ar
LD	:= CC

#
#	objects:
#
ObjCommon	:= $(ClasslibCommon)/AiffFile.o \
				$(ClasslibCommon)/BinaryFile.o \
				$(ClasslibCommon)/Breakpoint.o \
				$(ClasslibCommon)/Dilator.o \
				$(ClasslibCommon)/Distiller.o \
				$(ClasslibCommon)/Exception.o \
				$(ClasslibCommon)/Filter.o \
				$(ClasslibCommon)/Import.o \
				$(ClasslibCommon)/ImportLemur5.o \
				$(ClasslibCommon)/LoFreqBweKludger.o \
				$(ClasslibCommon)/LorisInit.o \
				$(ClasslibCommon)/LowMem.o \
				$(ClasslibCommon)/Map.o \
				$(ClasslibCommon)/Morph.o \
				$(ClasslibCommon)/Notifier.o \
				$(ClasslibCommon)/Oscillator.o \
				$(ClasslibCommon)/Partial.o \
				$(ClasslibCommon)/PartialIterator.o \
				$(ClasslibCommon)/SamplesFile.o \
				$(ClasslibCommon)/Synthesizer.o \
				$(ClasslibCommon)/endian.o \
				$(ClasslibCommon)/ieee.o \
				$(ClasslibCommon)/typetest.o \
				$(ClasslibCommon)/typetest_c.o 
# PartialPredicate.o \
				
ObjCli		:= console_test.o $(Clients)/TwoWayMorph.o

LorisLib	:= LorisLib.a

#
#	includes and libaries:
#
Includes	:= -I$(ClasslibUnix) -I$(ClasslibCommon) 
IncludesSTL	:= -I/usr/people/kfitz/stl/STLport/stlport
IncludesCli	:=	-I$(Clients)
Libs		:= -lm -lc

#
#	flags:
#
Deprecation	:= 	-DDeprecated_iostream_headers \
				-DDeprecated_cstd_headers \
				-DDeprecated_string_stream
No_stupid_warnings	:= -woff 1155,1001,1110
Ansi		:= -LANG:ansi-for-init-scope=ON


cFLAGS	:=	-xansi -O2 -mips3 $(No_stupid_warnings) $(Includes)
CFLAGS	:= 	-O2 -mips3 -n32 $(No_stupid_warnings) $(Deprecation) $(Ansi) $(Includes) $(IncludesCli)
LDFLAGS	:=  -n32

#
#	targets:
#
Targets	:= $(LorisLib) z test_morph

lib:	$(LorisLib)

$(LorisLib):	$(ObjCommon)
	$(AR) -o $(LorisLib) $(ObjCommon)

test:	test_morph

test_morph: 	lib  $(ObjCli)
	$(LD) -o test_morph $(LDFLAGS) $(ObjCli) $(LorisLib) $(Libs)

z: 	lib z.o
	$(LD) -o z $(LDFLAGS) z.o $(LorisLib) $(Libs)

all:	lib z test

depend:	
	$(CC) -M $(CFLAGS) $(ClasslibCommon)/*.C | grep -v /usr/include > $(ClasslibUnix)/dependfile
	$(cc) -M $(cFLAGS) $(ClasslibCommon)/*.c | grep -v /usr/include >> $(ClasslibUnix)/dependfile

include $(ClasslibUnix)/dependfile

clean:
	-rm *.o core dependfile; touch dependfile
	-rm $(ClasslibCommon)/*.o
	-rm $(Targets)

#
#	suffixes:
#
.SUFFIXES: .c .C .o

.C.o:
	cd $(<D); $(CC) $(CFLAGS) -c $(<F)

.c.o:
	cd $(<D); $(cc) $(cFLAGS) -c $(<F)


