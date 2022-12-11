        .ORIG x200
        LD  R6, Stack
        LD  R0, USR_PTR
        ADD R6, R6, #-1
        STR R0, R6, #0
        LD  R0, USR
        ADD R6, R6, #-1
        STR R0, R6, #0 
        LD  R1, INKB ; x0180
        LD  R2, ADDER ; x2000
        STR R2, R1, #0
        LD  R1, MASK
        STI R1, KBSR
        RTI
    
Stack   .FILL   x3000
INKB    .FILL   x0180 ; INTV
ADDER   .FILL   x2000
MASK    .FILL   x4000
KBSR    .FILL   xFE00
USR     .FILL   x3000
USR_PTR .FILL   x8002
.END
        
        .ORIG   x2000
        LD  R0, Save1
        LD  R1, Save2
        LD  R2, Save3
        LD  R3, Save4
        LDI R1, KBDR
        LD  R2, APHU
        ADD R3, R2, R1
        BRp AP        
        LD  R0, NNUM
        ADD R1, R0, R1
        LDI R2, HIGHT1
        ADD R2, R1, R2
        LD  R0, MAX
        ADD R0, R2, R0
        BRp HIGH
        STI R2, HIGHT1
        BRnzp   OVER1
HIGH    LD  R0, MAX
        NOT R0, R0
        ADD R0, R0, #1
        STI R0, HIGHT1
        BRnzp   OVER1      
AP      STI R1, CHAR1
OVER1   LD  R0, Save1
        LD  R1, Save2
        LD  R2, Save3
        LD  R3, Save4
        RTI
        
NNUM    .FILL   #-47
MAX     .FILL   #-18
APHU    .FILL   #-64
CHAR1   .FILL   x4000
HIGHT1  .FILL   x4001
KBDR    .FILL   xFE02
Save1   .FILL   x0
Save2   .FILL   x0
Save3   .FILL   x0
Save4   .FILL   x0
        .END
        
        .ORIG   x3000
INIT    LD  R1, A
        STI R1, CHAR
        LD  R1, BEGIN  
        STI R1, HIGHT
PUT     LD  R3, SKY 
        LDI R2, HIGHT
LOOP    ADD R3, R3, #-1
        BRz OVER2
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
        
OVER2   LD R0, LINE
        OUT
        LDI R2, HIGHT
        ADD R2, R2, #-1
        BRnz DOWN
        STI R2, HIGHT  
DOWN    JSR DELAY
        BRnzp   PUT
        
DELAY   ST  R1, Save5
        LD  R1, COUNT  
REP     ADD R1, R1, #-1
        BRp REP
        LD  R1, Save5
        RET
        
Save5   .FILL   x0
LINE    .FILL   #10
BEGIN   .FILL   #18
A       .FILL   #97
COUNT   .FILL   #10000
CHAR    .FILL   x4000 ; the bird
HIGHT   .FILL   x4001 ; the hight
DOTS    .FILL   x2E ; .
SKY     .FILL   #21
        .END