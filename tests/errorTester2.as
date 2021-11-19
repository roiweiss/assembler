;Written by Barr Caplan


.extern	ExternLabel
;These commands cant accept extern label:
        bne     $23 ,   $23,    ExternLabel   
        beq     $16,    $6,     ExternLabel
        blt     $15,    $0,     ExternLabel  
        bgt     $20,    $15,    ExternLabel
;not not defined labels
        jmp		NotDefined
        call	NotDefined
        bne     $23 ,   $23,    NotDefined
.entry	ExternLabel