sr = 44100
kr = 4410
ksmps = 10
nchnls = 1

instr 1
	idur = p3 		; duration of instrument play
	ktime linseg 0, p3, 3.0	; a linear time vector that just goes from 0 to 3 
				; sec over the duration of instrument play

	a1 lorisplay ktime, "clarinet.sdif", 1, 1, 1 	; will just play the partials contained
							; within clarinet.sdif straight through
					     		; from 0 to 3 sec with no fadetime 
							; and no envelope on the amplitude,
							; frequency and bandwidth
	out a1
endin



instr 2
	idur = p3		; duration of instrument play
	ifadetime = 0.001 	; a fadetime of 1 ms (same as Loris) will be used

	ktime linseg 0, idur/2, 2, idur/2, 0 	; a linear time vector that will 
						; play the partials from 0 to 2 seconds over half the
						; duration, and then in reverse from 2 to 0 seconds 
						; over the other half.
	kampenv linen 1, 0, idur, idur/3	; amplitude envelope
	kfreqenv expseg 1, p3/2, 1, p3/2, .95 	; frequency envelope
	kbwenv linseg 1, p3/2, 2, p3/2, 0  	; bandwidth envelope

	a1 lorisplay ktime, "clarinet.sdif", kampenv, kfreqenv, kbwenv, ifadetime  
						; plays through the partials over the time vector
						; ktime created, with the evelopes created and a
						; partial fade time as given.

	a2 butterlp a1, 3000  			; apply a lowpass filter with a cutoff frequency of 3kHz
	out a2
endin


instr 3
	idur = p3		; duration of instrument play
	ipitch = cpspch(p4) ; pitch to play
	
	ifscale = ipitch/415	; the original fundamental was about 415 Hz
	ifadetime = 0.001 	; a fadetime of 1 ms (same as Loris) will be used
	
	; make a time index signal
	ktime linseg	0.15, idur/4, 1.8, idur/4, .8, idur/2, 2.3
	
	; make a vibrato envelope
	kvenv linseg	0, idur/6, 0, idur/2, .02, idur/3, 0
	kvib oscil		kvenv, 4, 1		; table 1, sinusoid

	a1 lorisplay ktime, "clarinet.sdif", 2, ifscale+kvib, 1, ifadetime  

	a2 atone a1, 1000  
	out a2
endin
