start:
        add r0, r0, r0
        addi.l r1, r2, 0xF0F0F0F0
        add r0, r0, r7
        lw.l r7, r8, 0xFFFF0000
        addi r1, r1, 7
