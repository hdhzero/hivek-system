start:
        add r0, r0, r0 
        add r0, r0, r0
        add r0, r0, r0
        add r0, r0, r0
        addi r1, r0, 10

L0:
        eq p1, r1, r0
  (!p1) addi r1, r1, -1
   (p1) b L1 
        j L0 ;;

L1:
        addi r2, r0, 7
        sub r1, r1, r2

L2:
        j L2
