sr = 44100
kr = 4410
ksmps = 10
nchnls = 1

; 
; Play the partials in clarinet.sdif 
; from 0 to 3 sec with 1 ms fadetime 
; and no frequency , amplitude, or 
; bandwidth modification.
;
instr 1
    ktime    linseg      0, p3, 3.0    ; linear time function from 0 to 3 seconds
    asig     lorisplay   ktime, "clarinet.sdif", 1, 1, 1, .001
             out         asig
endin


; 
; Play the partials in clarinet.sdif 
; from 0 to 3 sec with 1 ms fadetime 
; adding vibrato, increasing the 
; "breathiness" (noisiness) and overall
; amplitude, and adding a highpass filter.
;
instr 2
    ktime    linseg      0, p3, 3.0    ; linear time function from 0 to 3 seconds

    ; make a vibrato envelope
    kvenv    linseg      0, p3/6, 0, p3/6, .02, p3/3, .02, p3/6, 0, p3/6, 0
    kvib     oscil       kvenv, 4, 1   ; table 1, sinusoid

    kbwenv   linseg      1, p3/6, 1, p3/6, 2, 2*p3/3, 2
    a1       lorisplay   ktime, "clarinet.sdif", 1+kvib, 2, kbwenv, .001  
    a2       atone       a1, 1000      ; highpass filter, cutoff 1000 Hz
             out         a2
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

	a1 lorisplay ktime, "clarinet.sdif", ifscale+kvib, 2, 1, ifadetime  

	a2 atone a1, 1000  
	out a2
endin
