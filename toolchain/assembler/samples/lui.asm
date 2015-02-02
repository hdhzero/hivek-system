start:
        lui r7, %hi16(0xFFFF0000)
        lli r7, %lo16(0x0000FFFF)

        lui r1, %hi16(1234567)
        lli r1, %lo16(1234567)

        b start
