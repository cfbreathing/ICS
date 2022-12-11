        ; made by luowenjie
        .ORIG   x3000
        JSR     GET
        ST  R1, NUM
        LD  R3, PAGE
        JSR     TRANS
        ADD R3, R3, #1
        LD  R2, NUM     ; conuter of input
        
INPUT   JSR     GET
        JSR     TRANS
        JSR     GET
        JSR     TRANS
        ADD R3, R3, #2
        ADD R2, R2, #-1
        BRz     INEND
        BRnzp   INPUT  
        
INEND   AND R0, R0, #0
        ADD R0, R0, #1
        AND R1, R1, #0
        LD  R2, PAGE
        ADD R2, R2, #1
        AND R3, R3, #0
        ADD R3, R3, #1
        AND R4, R4, #0
        LDI R5, PAGE    ; TARGET
        ADD R5, R5, R5
        LD  R6, Stack
        JSR     FLIP
        LD  R2, PAGE    ; flip
        JSR     ANS     ; output the answer
        HALT
        
        ; R2 is the pointer to the marked right side
ANS     ADD R2, R2, #2
        LDR R0, R2, #0
        BRz     ANSOUT  ; ans have put out over
        AND R1, R1, #0
        LD  R3, ONESIX
ANSLOOP ADD R1, R1, #1
        ADD R0, R0, R0
        BRn     ANSPUT
        BRnzp   ANSLOOP
ANSPUT  NOT R1, R1
        ADD R1, R1, #1
        ADD R1, R1, R3
        LD  R3, ENDL
        ADD R4, R1, R3
        BRn     ONENUM  ; only one number
        LD  R3, ANUM
        ADD R0, R3, #1
        OUT
        ADD R0, R3, R4
        OUT
        BRnzp   ANSNEXT
ONENUM  LD  R3, ANUM
        ADD R0, R1, R3
        OUT
ANSNEXT LD  R0, SPACE
        NOT R0, R0
        ADD R0, R0, #1
        OUT
        BRnzp   ANS
ANSOUT  RET
        ; R0 is the chapter, R1 is flag of right, R2 is pointer,
        ; R3 is a counter to figure out the 
        ; R4 is a vector to count which page have been picked
        ; R5 is the target, which is the whole chapter,
        ; FLIP is the main recursion, counting the chapters        
FLIP    ADD R6, R6, #-1
        STR R0, R6, #0
        ADD R6, R6, #-1
        STR R2, R6, #0
        ADD R6, R6, #-1
        STR R3, R6, #0
        ADD R6, R6, #-1
        STR R4, R6, #0
        ADD R6, R6, #-1
        STR R7, R6, #0
        
        AND R7, R0, R5
        BRnp    FILEND
        LD  R2, PAGE
        ADD R2, R2, #1
        AND R3, R3, #0
        ADD R3, R3, #1
        BRnzp   FLOW
        
LOOP    ADD R2, R2, #2
        ADD R3, R3, R3
FLOW    AND R7, R3, R5
        BRnp    LOOPOUT
        LDR R7, R2, #0
        AND R7, R0, R7
        BRz     LOOP
        AND R7, R3, R4
        BRnp    LOOP
        
        ADD R6, R6, #-1
        STR R4, R6, #0        
        ADD R4, R3, R4
        ADD R6, R6, #-1
        STR R0, R6, #0
        ADD R0, R0, R0
        JSR     FLIP
        LDR R0, R6, #0
        ADD R6, R6, #1
        LDR R4, R6, #0
        ADD R6, R6, #1    
        
        ADD R1, R1, #0
        BRz     LOOP
        ADD R2, R2, #1
        STR R0, R2, #0
        BRnzp   LOOPOUT
FILEND  ADD R1, R1, #1
LOOPOUT LDR R7, R6, #0
        ADD R6, R6, #1
        LDR R4, R6, #0
        ADD R6, R6, #1
        LDR R3, R6, #0
        ADD R6, R6, #1
        LDR R2, R6, #0
        ADD R6, R6, #1
        LDR R0, R6, #0
        ADD R6, R6, #1
        RET       

    ; store the num in the R1 into the adderss in R3 as binary
TRANS   ST  R0, Save1
        ST  R2, Save2
        LEA R0, ZERO
        ADD R0, R1, R0
        LDR R0, R0, #0
        LDR R2, R3, #0
        NOT R0, R0
        NOT R2, R2
        AND R0, R2, R0
        NOT R0, R0
        STR R0, R3, #0
        LD  R0, Save1
        LD  R2, Save2
        RET
        
    ; get a num string into R1 and echo
GET     ST  R0, Save1 ; 
        ST  R2, Save2 ; store the ending(space or endl)
        ST  R3, Save3
        AND R1, R1, #0
GETLOOP GETC
        OUT
        LD  R2, ENDL
        ADD R3, R2, R0
        BRz ENDGET
        LD  R2, SPACE
        ADD R3, R2, R0
        BRz ENDGET
        LD  R2, NNUM
        ADD R0, R0, R2
        ADD R1, R1, R1
        ADD R3, R1, R1
        ADD R3, R3, R3
        ADD R1, R1, R3
        ADD R1, R1, R0
        BRnzp   GETLOOP
ENDGET  LD  R0, Save1
        LD  R2, Save2
        LD  R3, Save3
        RET
        
Stack   .FILL   xFE00
Save1   .FILL   x0
Save2   .FILL   x0
Save3   .FILL   x0
Save4   .FILL   x0
ONESIX  .FILL   #16
ENDL    .FILL   #-10
SPACE   .FILL   #-32
NNUM    .FILL   #-48
ANUM    .FILL   #48
NUM     .FILL   x0
ZERO    .FILL   x0
        .FILL   x1
        .FILL   x2
        .FILL   x4
        .FILL   x8
        .FILL   x10
        .FILL   x20
        .FILL   x40
        .FILL   x80
        .FILL   x100
        .FILL   x200
        .FILL   x400
        .FILL   x800
        .FILL   x1000
        .FILL   x2000
        .FILL   x4000
        .FILL   x8000
PAGE    .FILL   x5000
    .END



        .ORIG   x5000
TARGET  .FILL   x0
PAGE1   .FILL   x0
        .FILL   x0
PAGE2   .FILL   x0
        .FILL   x0
PAGE3   .FILL   x0
        .FILL   x0
PAGE4   .FILL   x0
        .FILL   x0
PAGE5   .FILL   x0
        .FILL   x0
PAGE6   .FILL   x0
        .FILL   x0
PAGE7   .FILL   x0
        .FILL   x0
PAGE8   .FILL   x0
        .FILL   x0
PAGE9   .FILL   x0
        .FILL   x0
PAGE10  .FILL   x0
        .FILL   x0
PAGE11  .FILL   x0
        .FILL   x0
PAGE12  .FILL   x0
        .FILL   x0
PAGE13  .FILL   x0
        .FILL   x0
PAGE14  .FILL   x0
        .FILL   x0
PAGE15  .FILL   x0
        .FILL   x0
    .END