        .ORIG x3000
        LEA R3, RESEND
        LD  R4, NENTER
GETCHAR GETC
        OUT
        AND R1, R1, #0
        ADD R1, R1, R0
        ADD R2, R1, R4
        BRz DONE
        STR R0, R3, #1
        ADD R3, R3, #1
        BRnzp GETCHAR
DONE    STR R0, R3, #1
        AND R1, R1, #0
        LD R5, LEFT
        LD R6, RIGHT
        STR R1, R3, #2
        STR R1, R5, #0
        STR R1, R6, #0
        LEA R2, RESEND
OP      LDR R1, R2, #1
        BRz NONE
        LD  R4, NENTER
        ADD R3, R1, R4
        BRz NONE
        ADD R2, R2, #1        
        LD  R4, NPLUS
        ADD R3, R1, R4
        BRz PUSH1
        LD  R4, NMIUS
        ADD R3, R1, R4
        BRz POP1        
        LD  R4, NLEFT
        ADD R3, R1, R4
        BRz PUSH2        
        LD  R4, NRIGHT
        ADD R3, R1, R4
        BRz POP2
NONE    HALT
PUSH1   AND R4, R4, #0
        LDR R0, R2, #1
        ADD R2, R2, #1
        JSR PUSH
        BRnzp OP
POP1    AND R4, R4, #0
        JSR POP
        OUT
        BRnzp OP
PUSH2   AND R4, R4, #0
        ADD R4, R4, #1
        LDR R0, R2, #1
        ADD R2, R2, #1
        JSR PUSH
        BRnzp OP
POP2    AND R4, R4, #0
        ADD R4, R4, #1
        JSR POP
        OUT
        BRnzp OP        

NENTER  .FILL   xFFF6 ; -x0A
NPLUS   .FILL   xFFD5 ; -x2B
NMIUS   .FILL   xFFD3 ; -x2D
NLEFT   .FILL   xFFA5 ; -5B
NRIGHT  .FILL   xFFA3 ; -5D
LEFT    .FILL   x4080
RIGHT   .FILL   x4081
RESEND  .FILL   x0
RES     .BLKW   x200
        ; R0 store the value needed to print
        ; R4 is the flag to decide left or right side is the operated side
        ; R5 is the left pointer, R6 is the right pointer.
POP     NOT R1, R6
        ADD R1, R1, #2
        ADD R1, R5, R1
        BRz fail        ; Branch if stack is empty
        ADD R1, R4, #0
        BRz POLEFT      ; to left
        ADD R6, R6, #-1
        LDR R0, R6, #0  ; get the value
        BRnzp success
POLEFT  ADD R5, R5, #1
        LDR R0, R5, #0  ; get the value
        BRnzp success        
    ;PUSH
PUSH    ADD R1, R4, #0
        BRz PULEFT
        STR R0, R6, #0 
        ADD R6, R6, #1 
        BRnzp success
PULEFT  STR R0, R5, #0 
        ADD R5, R5, #-1 
success RET
fail    LD  R0, FLAG
        RET
FLAG    .FILL   x5F ; _
.END