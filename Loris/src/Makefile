#!gmake
#
#	Makefile for building the Loris class library.
#

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

cFLAGS	:=	-O2 $(Includes) $(Debugging)
CFLAGS	:= 	-O2 $(Includes) $(Debugging)

#
#	targets:
#
classes: ${OBJ_LORIS}

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

#
#	suffixes:
#
.SUFFIXES: .c .C .o

.C.o:
	$(CC) $(CFLAGS) -c $<

.c.o:
	$(cc) $(cFLAGS) -c $<
