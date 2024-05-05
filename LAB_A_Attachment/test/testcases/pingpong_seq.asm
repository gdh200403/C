.ORIG x3000
    AND R0, R0, #0 ; n
    ADD R0, R0, #1
    AND R1, R1, #0 ; f(n)
    ADD R1, R1, #3
    AND R2, R2, #0 ; direction
    ADD R2, R2, #2

; Main loop
LOOP:
    LD R3, xfb ;x3102 x3006
    ADD R7, R0, #0
    NOT R7, R7
    ADD R7, R7, #1
    ADD R7, R7, R3
    BRz END

    ; Calculate f(n+1)
    ADD R4, R1, R1 ; R4 = 2*f(n)
    ADD R4, R4, R2 ; R4 = 2*f(n) + direction
    LD      R5,UPPER_BOUND  ;4095
    AND     R4,R4,R5    ;mod 4096
    AND R1, R1, #0
    ADD R1, R1, R4 ; Update f(n)
    

    ; Update n and direction
    ADD R0, R0, #1

    ; Check if f(n) is divisible by 8 or contains the digit '8'
    AND R5, R1, #7 ; R5 = f(n) mod 8
    BRz CHANGE_DIRECTION

    LD R6, xee ; LD R6 from M[X3104] = -1000; x3015 this line
    ADD R5, R1, #0
;Check if f(n) contains the digit '8'. f(n) < 4096, so thousand digits cannot be 8, R5 %= 1000.
MINUS_1000:
    ADD R5, R5, R6 
    BRzp MINUS_1000
    NOT R6, R6
    ADD R6, R6, #1
    ADD R5, R5, R6
    LD R6, xe8 ; LD R6 from M[X3105] = -100; x3022 this line
    AND R7, R7, #0

;R7 = R5 / 100 + 1, so compared with 9
MINUS_100:
    ADD R7, R7, #1
    ADD R5, R5, R6
    BRzp MINUS_100
    NOT R6, R6
    ADD R6, R6, #1
    ADD R5, R5, R6
    ADD R7, R7, #-9
    BRz CHANGE_DIRECTION
    AND R7, R7, #0

;R7 = R5 / 10 + 1, so compared with 9
MINUS_10:
    ADD R7, R7, #1
    ADD R5, R5, #-10
    BRzp MINUS_10
    ADD R5, R5, #10
    ADD R7, R7, #-9
    BRz CHANGE_DIRECTION
    AND R7, R7, #0

;R7 = R5 + 1, so compared with 9
MINUS_1:
    ADD R7, R7, #1
    ADD R5, R5, #-1
    BRzp MINUS_1
    ADD R5, R5, #1
    ADD R7, R7, #-9
    BRz CHANGE_DIRECTION
    AND R7, R7, #0
    BRnzp LOOP

CHANGE_DIRECTION:
    NOT R2, R2
    ADD R2, R2, #1
    BRnzp LOOP


END:
    ST R1, xc9 ;ST R1 at M[x3103] x3039 this line
    TRAP x25
    
    
UPPER_BOUND .FILL   x0FFF
.END