; make sinusoid in table 1
f 1 0 4096 10 1

; play instr 1
;     strt   dur
i 1    0      3
i 1    +      1
i 1    +      6

; play instr 2
;     strt   dur   ptch
i 2    11     3    8.08
i 2    13.5   1    8.04
i 2    14     6    8.00
i 2    14     6    8.07

s
;; i 3 0 4 8.08
;; i 3 + 3 8.01
;; 
;; i 3 8   6 8.02
;; i 3 8.01 5 8.06
;; i 3 8.02 6 8.09
;; i 3 12   2 8.11
;; 

e