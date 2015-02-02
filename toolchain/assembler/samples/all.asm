start:
    add r1, r2, r3
    sub r1, r2, r3

    and r1, r2, r3
    or r1, r2, r3
    xor r1, r2, r3
    nor r1, r2, r3

    eq p1, r1, r2
    lt r1, r2, r2
    ltu r1, r2, r3

    sll r1, r2, r3
    srl r1, r2, r3
    sra r1, r2, r3
    shadd r1, r2, r3 sll 7
    shadd r1, r2, r3 srl 15
    shadd r1, r2, r4 srl 3

    lw r1, r2, 7
    lh r1, r2, 7
    lb r1, r2, 7
    lhu r1, r2, 7
    lbu r1, r2, 7
    sw r1, r2, 7
    sh r1, r2, 7
    sb r1, r2, 7
   
    j 7
    jal 7
    b 7
    bn 7
    jr r1
    jalr r1 

    addi r1, r2, 7

    andi r1, r2, 7
    ori r1, r2, 7
    xori r1, r2, 7

    eqi p1, r1, 7
    lti r1, r2, 7
    ltiu r1, r2, 7

