Loris_Objects = AiffFile.o Breakpoint.o BreakpointEnvelope.o \
	BreakpointUtils.o Endian.o Envelope.o Exception.o \
	Filter.o  Notifier.o Partial.o \
	PartialUtils.o SdifFile.o \
	SpcFile.o ieee.o
	
Loris_Dir = /Volumes/Buddha/Loris

New_Objects = JitterySynthesizer.o NoiseGenerator.o ODonnellOscil.o 

CC = g++
cc = gcc
CC_FLAGS = -I. -I$(Loris_Dir) -I$(Loris_Dir)/classes -DHAVE_CONFIG_H
LD_FLAGS = -L/sw/lib -L/usr/local/lib -lfftw -lsdif

.SUFFIXES: .c++ .o
	
test_jitter: jitterMain.o $(Loris_Objects) $(New_Objects)
	$(CC) -o $@ jitterMain.o $(Loris_Objects) $(New_Objects) $(LD_FLAGS)

AiffFile.o: $(Loris_Dir)/classes/AiffFile.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

Breakpoint.o: $(Loris_Dir)/classes/Breakpoint.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

BreakpointEnvelope.o: $(Loris_Dir)/classes/BreakpointEnvelope.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

BreakpointUtils.o: $(Loris_Dir)/classes/BreakpointUtils.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

Endian.o: $(Loris_Dir)/classes/Endian.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

Envelope.o: $(Loris_Dir)/classes/Envelope.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

Exception.o: $(Loris_Dir)/classes/Exception.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

Filter.o: $(Loris_Dir)/classes/Filter.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<
 
Notifier.o: $(Loris_Dir)/classes/Notifier.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

Partial.o: $(Loris_Dir)/classes/Partial.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

PartialUtils.o: $(Loris_Dir)/classes/PartialUtils.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

SdifFile.o: $(Loris_Dir)/classes/SdifFile.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

SpcFile.o: $(Loris_Dir)/classes/SpcFile.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

JitterySynthesizer.o: JitterySynthesizer.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

NoiseGenerator.o: NoiseGenerator.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

ODonnellOscil.o: ODonnellOscil.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

ieee.o: $(Loris_Dir)/classes/ieee.c
	$(cc) $(CC_FLAGS) -c -o $@ $<
	
.c++.o:
	$(CC) $(CC_FLAGS) -c -o $@ $<
