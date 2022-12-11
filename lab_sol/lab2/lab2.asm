        .ORIG x3000
        AND R2, R2, #0
        LEA R3, CHAR1
        LD R4, NASCIID
        LD R5, NENTER
GETNUMS GETC
        ADD R1, R0, R5
        BRz DONE
        ADD R1, R0, R4
        STR R1, R3, #0
        ADD R3, R3, #1
        ADD R2, R2, #1
        BRnzp GETNUMS
    ; NUMS getting is over
DONE    AND R0, R0, #0 ; binary num
        LEA R3, CHAR1
LOOP    LDR R1, R3, #0
        ADD R3, R3, #1
        ADD R0, R0, R1
        ADD R2, R2, #-1
        BRz LOOPUOT
        ADD R1, R0, R0
        ADD R0, R1, R1
        ADD R4, R0, R0
        ADD R0, R4, R1
        BRnzp LOOP
    ; R0 has the number
LOOPUOT LEA R3, MARK ; output char addressing-1
        ADD R2, R2, #5 ; counter
        AND R5, R5, #0
        LD  R4, PDIV ; mask
    ; bigin to trans
TRANS   ADD R5, R5, #4
        AND R1, R0, R4
        STR R1, R3, #0
        ADD R3, R3, #1
        ADD R2, R2, #-1
        BRz BOUT
LEFT    ADD R0, R0, #0
        BRzp ZARO
        BRnzp ONE
ZARO    ADD R0, R0, R0
        BRnzp ALOOP
ONE     ADD R0, R0, R0
        ADD R0, R0, #1
ALOOP   ADD R5, R5, #-1
        BRz TRANS
        BRnzp LEFT
    ; output
BOUT    LEA R3, CHAR1
        ADD R2, R2, #4
        LD  R4, ASCNUM
        LD  R5, ASCAPH
OUTPUT  LDR R0, R3, #0
        ADD R1, R0, #-10
        BRzp APH
        ADD R0, R0, R4
        BRnzp STORCH
APH     ADD R0, R0, R5
STORCH  OUT
        ADD R3, R3, #1
        ADD R2, R2, #-1
        BRp OUTPUT
        HALT        
MARK    .FILL   x0
CHAR1   .FILL   x0
CHAR2   .FILL   x0
CHAR3   .FILL   x0
CHAR4   .FILL   x0
CHAR5   .FILL   x0
PDIV    .FILL   xF
ASCNUM  .FILL   x30
ASCAPH  .FILL   x37
NASCIID .FILL   xFFD0 ; -x30
NENTER  .FILL   xFFF6 ; -x0A
.END