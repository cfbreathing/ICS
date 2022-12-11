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
        ;init
INEND   AND R0, R0, #0
        ADD R0, R0, #1
        AND R1, R1, #0
        LD  R2, PAGE
        ADD R2, R2, #1
        AND R3, R3, #0
        ADD R3, R3, #1
        LDI R4, PAGE    ; TARGET
        ADD R4, R4, R4
        LD  R6, Stack
        JSR     FLIP    ; flip
        LD  R2, PAGE
        JSR     ANS     ; output the answer
        HALT
        
        ; R2 is the pointer to the marked right side 
ANS     ADD R2, R2, #2
        LDR R1, R2, #0
        BRz     ANSOUT  ; ans have put out over
ANSPUT  LD  R3, ANUM
        LD  R4, ENDL
        ADD R4, R1, R4
        BRn     ONENUM  ; only one number
        ADD R0, R3, #1
        OUT 
        ADD R0, R4, R3
        OUT
        BRnzp   ANSNEXT
ONENUM  ADD R0, R1, R3
        OUT
ANSNEXT LD  R0, SPACE
        NOT R0, R0
        ADD R0, R0, #1
        OUT             ; output a space.
        BRnzp   ANS
ANSOUT  RET

        ; R0 is the chapter, R1 is flag of right, R2 is pointer,
        ; R3 is a counter for inserting the right chapter,
        ; R4 is the target, which is the whole chapter,
        ; FLIP is the recursion fuction
FLIP    ADD R6, R6, #-1
        STR R2, R6, #0
        ADD R6, R6, #-1
        STR R7, R6, #0 
        ; PUSH R2, R7 into the stack
        AND R7, R0, R4
        BRnp    OVER
        LDR R7, R2, #0
        BRn     FNEXT   ; picked page
        BRz     FOUT    ; no page
        AND R7, R7, R0
        BRz     FNEXT   ; the page haven't the chapter
        LDR R7, R2, #0
        ; PUSH the value of current page
        ADD R6, R6, #-1
        STR R7, R6, #0  
        LD  R5, MASK    
        ADD R7, R5, R7  ; there is an unpicked and suitable page, mark it
        STR R7, R2, #0
        ; PUSH R0, R2, R3 before the next recursion
        ADD R6, R6, #-1
        STR R0, R6, #0
        ADD R6, R6, #-1
        STR R2, R6, #0 
        ADD R6, R6, #-1
        STR R3, R6, #0
        ADD R0, R0, R0  ; find the next chapter
        ADD R3, R3, #1  ; add the counter
        LD  R2, PAGE    ; reset the pointer as the next flip is start from the frist page
        ADD R2, R2, #1
        JSR     FLIP
        ; POP the R0, R2, R3 after the recursion
        LDR R3, R6, #0
        ADD R6, R6, #1
        LDR R2, R6, #0
        ADD R6, R6, #1
        LDR R0, R6, #0
        ADD R6, R6, #1
        LDR R7, R6, #0
        ADD R6, R6, #1
        STR R7, R2, #0
        ADD R1, R1, #0  ; judge the way
        BRz     FNEXT
        ADD R2, R2, #1
        STR R3, R2, #0
        BRnzp   FOUT
FNEXT   ADD R2, R2, #2
        JSR     FLIP
        BRnzp   FOUT
OVER    ADD R1, R1, #1  ; the all chapter have been picked, mark the flag
        ; POP R0, R2, R3, R7 onto the stack
FOUT    LDR R7, R6, #0
        ADD R6, R6, #1
        LDR R2, R6, #0
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
        BRz     ENDGET
        LD  R2, SPACE
        ADD R3, R2, R0
        BRz     ENDGET
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
        
MASK    .FILL   x8000
Stack   .FILL   xFE00
Save1   .FILL   x0
Save2   .FILL   x0
Save3   .FILL   x0
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