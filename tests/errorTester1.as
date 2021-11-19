Written by Barr Caplan


ValidLabel:   stop

;###############  Long lines:    ###############
.asciz "This line is reallllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllly long"
;this comment is realllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllly long
.dh 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
.asciz "This line is 81 chars long, including white notes!!!!!!!!!!!!!!!!!!!!!!!"
;This comment is 81 chars long, including white notes!~~~~~~~~~~~~~~~~~~~~~~~~~~~
                
;###############  illegal registers:    ###############          
        add     $+0 ,$1 ,$2  
        sub     $3 ,$-4 ,$5
        and     $6 ,$7 ,$32
        or      $9 ,$10,$154
        nor     $12,$1 3,$14 
        move    $+15,$16
        mvhi    $18,$-19
        mvlo    $21,$32
        addi    $-24  ,  123  ,  $25
        subi    $26 ,   0   ,   $+27  
        andi    $28,    -30,    $32
        bne     $32 ,   $23,    ValidLabel   
        beq     $16,    $-6,     ValidLabel
        blt     $ 15,    $0,     ValidLabel  
        lb      $32  ,  123  ,  $25    
        sb      $26 ,   0   ,   $ 27
        lw      $-28,    -30,    $29
        sw      $30,-32768,$+31 
        lh      $17,32767,20
        sh      26$ ,   0   ,   $27 5 
        jmp     $ 7
        and     $6 ,7 ,$5
        or      $9 ,$10,7
        nor     12,$13,$14 
        or      $$9 ,$10,$11
        nor     $12,$13,$$14 
        move    $15,$$16
        mvhi    $$18,$19

;###############  Fun with commas:      ###############
        add     $0 ,$1 ,$2 , 
        sub     $3 ,,$4 ,$5
        and     ,$6 ,$7 ,$8
        or      $9 ,$1,0,$11
        nor     $12,$13$14 
        move    $15,$16,
        mvhi    ,$18,$19
        mvlo    $21,,$22
        addi    $24  ,  123 , ,  $25
        subi    $26 ,   0   ,   $2,7  
        andi    $28,    -3,0,    $29,
        ori     ,$30,-32768,$31 
        nori    $17,327,67,$20
        bne     $23 ,   $23,    ValidLabel  , 
        beq     ,$16,    $6,     ValidLabel
        blt     $15,    $0,  ,   ValidLabel  
        bgt     $20,    $15,    ,ValidLabel
        lb      $24  ,  123  ,  $25 ,   
        sb      $26, ,   0   ,   $27
        lw      $28,    ,-30,    $29
        sw      $30,-32768,$31 ,
        lh      $17,,32767,$20
        sh      $26 ,   0   ,   $2,7  
        jmp     $7,
        jmp     ,$7
        .db     0,5,-3,+80, 1,1 ,-128,127 , 
        .dh     0,5,-3,+80, 1,,1 ,-32768,32767 
        .dw     0,5,-3,+80, 1, ,1 ,-2147483648,2147483647  
        .db     ,0,5,-3,+80, 1,1 ,-128,127  
        move    $15.$16
        .db     0,5,-3,+80. 1,1 ,-128,127  


;###############  Sneaky immed:      ###############
        addi    $24  ,  12.3  ,  $25
        subi    $26 ,   +-0   ,   $27  
        andi    $28,    --30,    $29
        lb      $24  ,  1.23  ,  $25    
        sb      $26 ,   -+0   ,   $27
        lw      $28,    -3 0,    $29
        sh      $26 ,   !0   ,   $27
        sh      $26 ,   $0   ,   $2  


;###############  Fishy operands:      ###############
        add     $0 ,$1 ,$2 ,$2 
        sub     $3 ,$4 
        and     $6 ,$7 ,$8,$3 ,$4 
        move    $15
        mvhi    $18,$19,$3 ,$4 
        mvlo    $21,$22,$0
        addi    $24   ,  $25
        subi    $26 ,   0   ,   $27  $3 
        andi    $28, 
        ori     -32768
        bne     $23 ,       ValidLabel   
        beq     $16,    $6,    
        blt     $15,    $0,   $3   ValidLabel  
        bgt     $20,    $15,    ValidLabel, ValidLabel 
        lb      $24  ,  123     
        sb      $26 ,   0   ,   $27,$3 ,$4
        lw      $28,    -30,    $29,$3 
        sw      $30,$31 
        jmp     ValidLabel, ValidLabel
        jmp     $7,$3
        la      $5
        call    $2
        stop $5
        .asciz  "forgot quatation mark!
        .asciz  me to!"
        .asciz  so do i!
        .asciz  "
        .asciz  "

;###############  getting just out of range:  ###############
        ori     $30,-32769,$31 
        nori    $17,32768,$20
        sw      $30,-32769,$31 
        lh      $17,32768,$20
        .db     0,5,-3,+80, 1,1 ,-129,127  
        .db     0,5,-3,+80, 1,1 ,-128,128  
        .dh     0,5,-3,+80, 1,1 ,-32769,32767 
        .dh     0,5,-3,+80, 1,1 ,-32768,32768 
        .dw     0,5,-3,+80, 1,1 ,-2147483649,2147483647
        .dw     0,5,-3,+80, 1,1 ,-2147483648,2147483648

;###############  not real commands:  ###############
        And     $6 ,$7 ,$8
        mov    $15,$16
        andii    $28,    -30,    $29
        sw!      $30,-32768,$31
        call?    ValidLabel
        s_top
        .asci   "not asciz enough"

;###############  Fun with labels:  ###############
LABE1:  add     $0 ,$1 ,$2
LABE1:  move    $15,$16
LABEL!:  add     $0 ,$1 ,$2
?LABEL:  move    $15,$16
BAD_LABEL:  add     $0 ,$1 ,$2
ImGoingToBeTooLongggggggggggggg:  move    $15,$16
4CantStartWithNum:  add     $0 ,$1 ,$2
NoColon  move    $15,$16
move:  add     $0 ,$1 ,$2
asciz:  move    $15,$16
:       move    $15,$16
NotAgain:  add     $0 ,$1 ,$2
        .extern NotAgain
LABE1:  move    $15,$16