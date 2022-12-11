        .ORIG   x2000
        ADD R6, R6, #-1
        STR R0, R6, #0
        ADD R6, R6, #-1
        STR R1, R6, #0   
        ADD R6, R6, #-1
        STR R2, R6, #0
        ADD R6, R6, #-1
        STR R3, R6, #0
        
        LDI R1, KBDR
        LD  R2, APHU
        ADD R3, R2, R1
        BRp AP        
        LD  R0, NNUM
        ADD R1, R0, R1
        LDI R2, HIGHT
        ADD R2, R1, R2
        LD  R0, MAX
        ADD R0, R2, R0
        BRp HIGH
        STI R2, HIGHT
        BRnzp   OVER
HIGH    LD  R0, MAX
        NOT R0, R0
        ADD R0, R0, #1
        STI R0, HIGHT
        BRnzp   OVER        
AP      STI R1, CHAR

OVER    LDR R3, R6, #0
        ADD R6, R6, #1
        LDR R2, R6, #0
        ADD R6, R6, #1
        LDR R1, R6, #0
        ADD R6, R6, #1
        LDR R0, R6, #0
        ADD R6, R6, #1
        RTI
        
NNUM    .FILL   #-47
MAX     .FILL   #-18
APHU    .FILL   #-64
CHAR    .FILL   x4000
HIGHT   .FILL   x4001
KBSR    .FILL   xFE00
KBDR    .FILL   xFE02
.END