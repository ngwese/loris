#! make

# LORIS = /Volumes/Buddha/Loris
# need to define LORIS, the directory where Loris lives.

Loris_Objects = AiffFile.o Breakpoint.o BreakpointEnvelope.o \
	BreakpointUtils.o Endian.o Envelope.o Exception.o \
	Filter.o  Notifier.o Partial.o \
	PartialUtils.o SdifFile.o \
	SpcFile.o ieee.o
	
New_Objects = JitterySynthesizer.o NoiseGenerator.o ODonnellOscil.o 

CC = g++
cc = gcc
CC_FLAGS = -O3 -I. -I$(LORIS) -I$(LORIS)/classes -DHAVE_CONFIG_H
LD_FLAGS = -L/sw/lib -L/usr/local/lib -lfftw -lsdif

.SUFFIXES: .c++ .o

all: test_jitter jittery_synth
	
test_jitter: jitterMain.o $(Loris_Objects) $(New_Objects)
	$(CC) -o $@ jitterMain.o $(Loris_Objects) $(New_Objects) $(LD_FLAGS)
	
jittery_synth: jitterySynth.o $(Loris_Objects) $(New_Objects)
	$(CC) -o $@ jitterySynth.o $(Loris_Objects) $(New_Objects) $(LD_FLAGS)

AiffFile.o: $(LORIS)/classes/AiffFile.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

Breakpoint.o: $(LORIS)/classes/Breakpoint.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

BreakpointEnvelope.o: $(LORIS)/classes/BreakpointEnvelope.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

BreakpointUtils.o: $(LORIS)/classes/BreakpointUtils.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

Endian.o: $(LORIS)/classes/Endian.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

Envelope.o: $(LORIS)/classes/Envelope.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

Exception.o: $(LORIS)/classes/Exception.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

Filter.o: $(LORIS)/classes/Filter.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<
 
Notifier.o: $(LORIS)/classes/Notifier.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

Partial.o: $(LORIS)/classes/Partial.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

PartialUtils.o: $(LORIS)/classes/PartialUtils.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

SdifFile.o: $(LORIS)/classes/SdifFile.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

SpcFile.o: $(LORIS)/classes/SpcFile.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

JitterySynthesizer.o: JitterySynthesizer.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

NoiseGenerator.o: NoiseGenerator.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

ODonnellOscil.o: ODonnellOscil.c++
	$(CC) $(CC_FLAGS) -c -o $@ $<

ieee.o: $(LORIS)/classes/ieee.c
	$(cc) $(CC_FLAGS) -c -o $@ $<
	
.c++.o:
	$(CC) $(CC_FLAGS) -c -o $@ $<
	
clean:
	-rm *.o test_jitter jittery_synth
