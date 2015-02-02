start:
        addi r1, r1, 1
        lti r2, r1, 10
        eq p1, r2, r0
  (!p1) b start
        b loop
        bn loop
loop:
        j loop

vec:
    .h32 0x77
