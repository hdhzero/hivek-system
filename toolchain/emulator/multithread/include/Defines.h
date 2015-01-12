#ifndef HIVEK_MULTITHREAD_EMULATOR_DEFINES_H
#define HIVEK_MULTITHREAD_EMULATOR_DEFINES_H

namespace HivekMultithreadEmulator {
    typedef unsigned long long u64;
    typedef long long i64;
    typedef unsigned int u32;
    typedef int i32;
    typedef unsigned short u16;
    typedef short i16;
    typedef unsigned char u8;
    typedef char i8;

    #define TYPE_I_MASK  0x070000000
    #define TYPE_II_MASK 0x078000000
    #define TYPE_III_MASK 0x07C000000
    #define TYPE_IV_MASK 0x07E000000
    #define TYPE_V_MASK  0x000600000

    #define N_LANES 2

    #define SZ_16 0
    #define SZ_24 1
    #define SZ_32 2
    #define SZ_64 3

    enum OpcodeOp {
        OP_ADD = 0,
        OP_SUB = 1,
        OP_AND = 2,
        OP_OR  = 3,
        OP_NOR = 4,
        OP_XOR = 5,
        OP_EQ = 6,
        OP_LT = 7,
        OP_LTU = 8,
        OP_JR = 9,
        OP_JALR = 10,
        OP_SLL = 11,
        OP_SRL = 12,
        OP_SRA = 13,
        OP_SHADD = 0,

        OP_ADDI = 0,
        OP_ANDI = 1,
        OP_ORI = 2,
        OP_XORI = 3,
        OP_EQI = 4,
        OP_LTI = 5,
        OP_LTIU = 6,

        OP_LW = 7,
        OP_LH = 8,
        OP_LB = 9,
        OP_LHU = 10,
        OP_LBU = 11,
        OP_SW = 12,
        OP_SH = 13,
        OP_SB = 14,

        OP_B = 0,
        OP_BN = 0,
        OP_J = 0,
        OP_JAL = 0
    };

    enum RtkKind {
        RTK_RT_RT = 0,
        RTK_RT_NRT = 1,
        RTK_RT_NOP = 2,
        RTK_NRT_NRT = 3,
        RTK_NRT_NOP = 4,
        RTK_NOP_NOP = 5
    };

    enum InstructionKind {
        IK_LAM = 0,
        IK_J = 1,
        IK_JR = 2,
        IK_B = 3,

        IK_LAM_LAM = 0,
        IK_LAM_J = 1,
        IK_LAM_JR = 2,
        IK_LAM_B = 3,

        IK_JR_J = 9,
        IK_JR_JR = 10,
        IK_JR_B = 11,

        IK_B_J = 13,
        IK_B_JR = 14,
        IKB_B = 15
        
    };

    enum ALUOp {
        ALU_ADD = 0,
        ALU_SUB = 1,

        ALU_AND = 2,
        ALU_OR = 3,
        ALU_NOR = 4,
        ALU_XOR = 5
    };

    enum SHType {
        BARREL_SLL,
        BARREL_SRL,
        BARREL_SRA
    };

    struct ControlTable {
        u32 alu_op;
        u32 alu_src;
        u32 alu_sh_src;
        u32 alu_sh_mem_src;

        u32 sh_type;
        u32 sh_amnt_src;
        u32 sh_add;

        u32 r_dst_src;
        u32 r_wren;
        u32 p_wren;
        u32 m_wren;
        u32 m_size;

        u32 instruction_kind;
    };

    static ControlTable control_table[] = {
        { ALU_ADD, 1, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP

        /* Type I */
        { ALU_ADD, 1, 0, 0, BARREL_SLL, 0, 0, 1, 1, 0, 0, 0, IK_LAM }, // addi 
        { ALU_AND, 1, 0, 0, BARREL_SLL, 0, 0, 1, 1, 0, 0, 0, IK_LAM }, // and 
        { ALU_OR,  1, 0, 0, BARREL_SLL, 0, 0, 1, 1, 0, 0, 0, IK_LAM }, // or 
        { ALU_XOR, 1, 0, 0, BARREL_SLL, 0, 0, 1, 1, 0, 0, 0, IK_LAM }, // xor 
        { ALU_SUB, 1, 0, 0, BARREL_SLL, 0, 0, 1, 0, 1, 0, 0, IK_LAM }, // eq 
        { ALU_SUB, 1, 0, 0, BARREL_SLL, 0, 0, 1, 1, 0, 0, 0, IK_LAM }, // lt 
        { ALU_SUB, 1, 0, 0, BARREL_SLL, 0, 0, 1, 1, 0, 0, 0, IK_LAM }, // ltu 
        { ALU_ADD, 1, 0, 1, BARREL_SLL, 0, 0, 1, 1, 0, 0, 0, IK_LAM }, // lw 
        { ALU_ADD, 1, 0, 1, BARREL_SLL, 0, 0, 1, 1, 0, 0, 0, IK_LAM }, // lh 
        { ALU_ADD, 1, 0, 1, BARREL_SLL, 0, 0, 1, 1, 0, 0, 0, IK_LAM }, // lb 
        { ALU_ADD, 1, 0, 1, BARREL_SLL, 0, 0, 1, 1, 0, 0, 0, IK_LAM }, // lhu 
        { ALU_ADD, 1, 0, 1, BARREL_SLL, 0, 0, 1, 1, 0, 0, 0, IK_LAM }, // lbu
        { ALU_ADD, 1, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 1, 4, IK_LAM }, // sw 
        { ALU_ADD, 1, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 1, 2, IK_LAM }, // sh 
        { ALU_ADD, 1, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 1, 1, IK_LAM }, // sb 
        { ALU_ADD, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP

        /* Type II */
        { ALU_ADD, 3, 0, 0, BARREL_SLL, 0, 0, 3, 0, 0, 0, 0, IK_J }, // NOP

        /* Type III */
        { ALU_ADD, 0, 0, 0, BARREL_SLL, 0, 0, 0, 1, 0, 0, 0, IK_LAM }, // add
        { ALU_SUB, 0, 0, 0, BARREL_SLL, 0, 0, 0, 1, 0, 0, 0, IK_LAM }, // sub
        { ALU_AND, 0, 0, 0, BARREL_SLL, 0, 0, 0, 1, 0, 0, 0, IK_LAM }, // and
        { ALU_OR,  0, 0, 0, BARREL_SLL, 0, 0, 0, 1, 0, 0, 0, IK_LAM }, // or
        { ALU_NOR, 0, 0, 0, BARREL_SLL, 0, 0, 0, 1, 0, 0, 0, IK_LAM }, // nor
        { ALU_XOR, 0, 0, 0, BARREL_SLL, 0, 0, 0, 1, 0, 0, 0, IK_LAM }, // xor
        { ALU_SUB, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 1, 0, 0, IK_LAM }, // eq
        { ALU_SUB, 0, 0, 0, BARREL_SLL, 0, 0, 0, 1, 0, 0, 0, IK_LAM }, // lt
        { ALU_SUB, 0, 0, 0, BARREL_SLL, 0, 0, 0, 1, 0, 0, 0, IK_LAM }, // ltu
        { ALU_ADD, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, IK_JR }, // jr
        { ALU_ADD, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, IK_JR }, // jalr
        { ALU_ADD, 0, 1, 0, BARREL_SLL, 1, 0, 0, 1, 0, 0, 0, IK_LAM }, // sll
        { ALU_ADD, 0, 1, 0, BARREL_SRL, 1, 0, 0, 1, 0, 0, 0, IK_LAM }, // srl
        { ALU_ADD, 0, 1, 0, BARREL_SRA, 1, 0, 0, 1, 0, 0, 0, IK_LAM }, // sra
        { ALU_ADD, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP
        { ALU_ADD, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP
        { ALU_ADD, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP
        { ALU_ADD, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP
        { ALU_ADD, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP
        { ALU_ADD, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP
        { ALU_ADD, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP
        { ALU_ADD, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP
        { ALU_ADD, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP
        { ALU_ADD, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP
        { ALU_ADD, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP
        { ALU_ADD, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP
        { ALU_ADD, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP
        { ALU_ADD, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP
        { ALU_ADD, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP
        { ALU_ADD, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP
        { ALU_ADD, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP
        { ALU_ADD, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP

        // shadd
        { ALU_ADD, 0, 1, 0, BARREL_SLL, 0, 1, 0, 1, 0, 0, 0, IK_LAM }, // NOP
        { ALU_ADD, 0, 1, 0, BARREL_SRL, 0, 1, 0, 1, 0, 0, 0, IK_LAM }, // NOP
        { ALU_ADD, 0, 1, 0, BARREL_SRA, 0, 1, 0, 1, 0, 0, 0, IK_LAM }, // NOP

        /* Type IV */
        { ALU_ADD, 3, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, IK_B } // b bn
    };
}

#endif
