        .ORIG   x3000
        
INIT    LD  R1, A
        STI R1, CHAR
        LD  R1, BEGIN  
        STI R1, HIGHT

PUT     LD  R3, SKY 
        LDI R2, HIGHT
LOOP    ADD R3, R3, #-1
        BRz OVER
        ADD R2, R2, #-1
        BRz BIRD
        LD  R0, DOTS
        OUT 
        BRnzp   LOOP
BIRD    LDI R0, CHAR
        OUT
        OUT 
        OUT
        ADD R3, R3, #-2
        BRnzp   LOOP
        
OVER    LD R0, LINE
        OUT
        LDI R2, HIGHT
        ADD R2, R2, #-1
        BRnz DOWN
        STI R2, HIGHT  
DOWN    JSR DELAY
        BRnzp   PUT
        
DELAY   ST  R1,Save1
        LD  R1,COUNT   ; #2500
REP     ADD R1,R1,#-1
        BRp REP
        LD  R1,Save1
        RET
        
        HALT
        
Save1   .FILL   x0
LINE    .FILL   #10
BEGIN   .FILL   #18
A       .FILL   #97
COUNT   .FILL   #5000    ; 2500
CHAR    .FILL   x4000   ; the bird
HIGHT   .FILL   x4001 ; the num
DOTS    .FILL   x2E ; .
SKY     .FILL   #21
.END