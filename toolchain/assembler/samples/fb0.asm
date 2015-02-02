start:
        add r0, r0, r0
        add r0, r0, r0
        add r0, r0, r0
        add r0, r0, r0
        add r0, r0, r0
        add r0, r0, r0
        add r0, r0, r0
        add r1, r0, r0
        addi r2, r0, 15

L0:
        addi r1, r1, 1
        sw r2, r1, 0
        j L0
        
