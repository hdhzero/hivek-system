    .text
    .global start
    .func start
start:
        add r1, r2, r3
        sub r4, r5, r6
        and r7, r8, r9
        or r10, r11, r12
        nor r13, r14, r15
        xor r16, r17, r18
        eq  p2, r19, r20
        lt r21, r22, r23
        ltu r24, r25, r26
        sllv r1, r2, r3
        srlv r1, r2, r3
        srav r1, r2, r3
        shadd r2, r3, r3 SLL 7
        shadd r2, r3, r3 SRL 31
        shadd r4, r4, r1 SRA 17

        lw r0, r1, 0x123
        lh r0, r2, 0b0101001
        lb r0, r3, 12315
        lhu r0, r2, 0b0101001
        lbu r0, r3, 12315

        sw r0, r1, 0x123
        sh r0, r2, 0b0101001
        sb r0, r3, 12315

        addi r1, r2, 10
        addl r1, r2, 0x1234abcd

        andi r1, r2, 11
        andl r1, r2, 0x12345678

        ori r1, r2, 11
        orl r1, r2, 0x12345678

        xori r1, r2, 11
        xorl r1, r2, 0x12345678

        eqi p1, r0, 0x234
        eql p1, r0, 0x12341234

        lti r1, r2, 0x123
        ltiu r1, r2, 0x1234CDCD

        lt r1, r2, r1
        ltu r1, r2, r1 ;;

        lti r1, r2, 17
        ltiu r1, r2, 17

        add.m r1, r2, r5
        add.n r1, r2, r5
        addi r1, r2, 124
        addl r1, r2, 0x12340000

        addi r1, r2, %hi04(start)
        addi r1, r2, %lo04(start)

        and p1, r1, r2
   (p1) sw r0, r0, 0

vector:
    .h16 0x123, 0x124, 0x45

msg:
    .asciiz "Hello      world!"

buffer:
    .r8 255

img:
    .incbin "img.bin"

