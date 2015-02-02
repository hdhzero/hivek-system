
start:
        add r1, r2, r3
        j 0x0F0F0
        jal 0x0F0F0
   (p1) b 15
  (!p3) bn 31

vec:
    .h16 0xFFFF, 0x0000, 0x7777, 0x0000
    .d16 255, 9, 7
    .d32 32
    .h32 0xffffffff
