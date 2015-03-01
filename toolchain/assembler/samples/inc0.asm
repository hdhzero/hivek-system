start:
        j main

.include "inc1.asm"

main:
        jal foo
        add r1, r1, r2

L0:
        j L0
        
