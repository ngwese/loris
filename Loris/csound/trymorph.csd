<CsoundSynthesizer>
; tryit.csd - a Csound structured data file 
; for testing the Loris unit generators

<CsOptions>
-A -d -o trymurph.aiff
</CsOptions>

<CsVersion> ;optional section
After 4.17  ; the Loris generators can't possibly work 
            ; with Csound before version 4.18
</CsVersion>

<CsInstruments>
; originally tryit.orc
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
    ktime    linseg      0, p3, 3.5    ; linear time function from 0 to 3.5 seconds
    kmurph   linseg      0, p3/3, 0, p3/3, 1, p3/3, 1
             lorisread   ktime, "clarinet.dilated.sdif", 1, 1, 2, 1, .001
             lorisread   ktime, "flute.dilated.sdif", 2, 1, 1, 1, .001
             lorismorph  1, 2, 3, kmurph, kmurph, kmurph
	asig     lorisplay   3, 1, 1, 1
             out         asig
endin

instr 2
    ktime1   linseg      0, p3, 3.4
    ktime2   linseg      0, p3, 1.25
    kmurph   linseg      0, p3/3, 0, p3/3, 1, p3/3, 1
    
             lorisread   ktime1, "meow3.sdif", 1, 1, 1, 1, .001
             lorisread   ktime2, "carhorn.sdif", 2, 1, 1, 1, .001
             lorismorph  1, 2, 3, kmurph, kmurph, kmurph
	asig     lorisplay   3, 1, 1, 1
             out         asig
endin

instr 3
    ionset   =           .25
    idecay   =           .15
    itmorph  =           p3 - (ionset + idecay)
    ipshift  =           cpspch(8.02)/cpspch(8.08)
    
    ktcl     linseg      0, ionset, .2, itmorph, 2.0, idecay, 2.1    ; clarinet time function, morph from .2 to 2.0 seconds
    ktfl     linseg      0, ionset, .5, itmorph, 2.1, idecay, 2.3    ; flute time function, morph from .5 to 2.1 seconds
    kmurph   linseg      0, ionset, 0, itmorph, 1, idecay, 1
             lorisread   ktcl, "clarinet.sdif", 1, ipshift, 2, 1, .001
             lorisread   ktfl, "flute.sdif", 2, 1, 1, 1, .001
             lorismorph  1, 2, 3, kmurph, kmurph, kmurph
	asig     lorisplay   3, 1, 1, 1
             out         asig
endin

;;   ;
;;   ; Morph the partials in carhorn.sdif into
;;   ; the partials in meow.sdif linearly over 
;;   ; all but the last 2 seconds of the note.
;;   ; The morph is performed over the first 
;;   ; .75 seconds of the source sounds. The last
;;   ; 2.5 seconds (of meow) is unmodified.
;;   ; Use 1 ms fade time.
;;   ;
;;   instr 4
;;       ktime    linseg      0, p3-2.5, 0.75, 2.5, 3.5   ; time index function
;;       kmorph   linseg      0, p3-2.5, 1.0, 2.5, 1.0    ; morphing function
;;       asig     lorismorph  ktime, "carhorn.sdif", "meow.sdif", kmorph, kmorph, kmorph, .001
;;                out         asig
;;   endin
;;   
;;   
;;   

</CsInstruments>   

<CsScore>
; play instr 1
;     strt   dur
i 1    0      3
i 1    +      1
i 1    +      6
s
; play instr 3
;     strt   dur
i 3    0      3
i 3    +      1
i 3    +      6
s
e
</CsScore>   </CsoundSynthesizer>
