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
    #define TYPE_IV_MASK 0x07FE00000
    #define TYPE_V_MASK  0x07E000000

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
        IK_B_B = 15
    };

    enum ALUOp {
        ALU_ADD = 0,
        ALU_SUB = 1,

        ALU_AND = 2,
        ALU_OR = 3,
        ALU_NOR = 4,
        ALU_XOR = 5,

        ALU_LT = 6,
        ALU_LTU = 7,
        ALU_EQ = 8,

        ALU_LUI = 9,
        ALU_LLI = 10
    };

    enum SHType {
        BARREL_SLL,
        BARREL_SRL,
        BARREL_SRA
    };

    enum GPUModes {
        GPU_TEXT_MODE = 0,
        GPU_VIDEO_MODE = 1
    };

    struct RTNextPCSels {
        u32 pc_alu_sel;
        u32 alu_sel;
        u32 zero_sz_sel;
        u32 sz_sel;
    };

    struct ControlTable {
        u32 alu_op;
        u32 alu_pc_vra_sel;
        u32 alu_vrb_immediate_sel;
        u32 alu_sh_sel;
        u32 alu_sh_mem_sel;
        u32 alu_sh_mem_jump_sel;

        u32 sh_type;
        u32 sh_amount_sel;
        u32 sh_add;

        u32 r_dst_sel;
        u32 rb_31_sel;
        u32 r_wren;

        u32 p_wren;

        u32 m_wren;
        u32 m_size;

        u32 instruction_kind;
    };

    static ControlTable control_table[] = {
        { ALU_ADD, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP 0

        /* Type I */
        { ALU_ADD, 0, 1, 0, 0, 0, BARREL_SLL, 0, 0, 1, 0, 1, 0, 0, 0, IK_LAM }, // addi 1
        { ALU_AND, 0, 1, 0, 0, 0, BARREL_SLL, 0, 0, 1, 0, 1, 0, 0, 0, IK_LAM }, // and  2
        { ALU_OR,  0, 1, 0, 0, 0, BARREL_SLL, 0, 0, 1, 0, 1, 0, 0, 0, IK_LAM }, // or   3
        { ALU_XOR, 0, 1, 0, 0, 0, BARREL_SLL, 0, 0, 1, 0, 1, 0, 0, 0, IK_LAM }, // xor  4
        { ALU_EQ,  0, 1, 0, 0, 0, BARREL_SLL, 0, 0, 1, 0, 0, 1, 0, 0, IK_LAM }, // eq   5
        { ALU_LT,  0, 1, 0, 0, 0, BARREL_SLL, 0, 0, 1, 0, 1, 0, 0, 0, IK_LAM }, // lt   6
        { ALU_LTU, 0, 1, 0, 0, 0, BARREL_SLL, 0, 0, 1, 0, 1, 0, 0, 0, IK_LAM }, // ltu  7
        { ALU_ADD, 0, 1, 0, 1, 0, BARREL_SLL, 0, 0, 1, 0, 1, 0, 0, 3, IK_LAM }, // lw   8
        { ALU_ADD, 0, 1, 0, 1, 0, BARREL_SLL, 0, 0, 1, 0, 1, 0, 0, 2, IK_LAM }, // lh   9
        { ALU_ADD, 0, 1, 0, 1, 0, BARREL_SLL, 0, 0, 1, 0, 1, 0, 0, 1, IK_LAM }, // lb   10
        { ALU_ADD, 0, 1, 0, 1, 0, BARREL_SLL, 0, 0, 1, 0, 1, 0, 0, 2, IK_LAM }, // lhu  11
        { ALU_ADD, 0, 1, 0, 1, 0, BARREL_SLL, 0, 0, 1, 0, 1, 0, 0, 1, IK_LAM }, // lbu  12
        { ALU_ADD, 0, 1, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 1, 3, IK_LAM }, // sw   13
        { ALU_ADD, 0, 1, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 1, 2, IK_LAM }, // sh   14
        { ALU_ADD, 0, 1, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 1, 1, IK_LAM }, // sb   15
        { ALU_ADD, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP  16

        /* Type II */
        { ALU_ADD, 1, 1, 0, 0, 0, BARREL_SLL, 0, 0, 1, 1, 0, 0, 0, 0, IK_J }, // J 17
        { ALU_ADD, 1, 1, 0, 0, 1, BARREL_SLL, 0, 0, 1, 1, 1, 0, 0, 0, IK_J }, // Jal 18

        /* Type III */
        { ALU_ADD, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 1, 0, 0, 0, IK_LAM }, // add 19
        { ALU_SUB, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 1, 0, 0, 0, IK_LAM }, // sub 20
        { ALU_AND, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 1, 0, 0, 0, IK_LAM }, // and 21
        { ALU_OR,  0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 1, 0, 0, 0, IK_LAM }, // or  22
        { ALU_NOR, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 1, 0, 0, 0, IK_LAM }, // nor 23
        { ALU_XOR, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 1, 0, 0, 0, IK_LAM }, // xor 24
        { ALU_EQ,  0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 1, 0, 0, IK_LAM }, // eq  25
        { ALU_LT,  0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 1, 0, 0, 0, IK_LAM }, // lt  26
        { ALU_LTU, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 1, 0, 0, 0, IK_LAM }, // ltu 27
        { ALU_ADD, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, 0, IK_JR }, // jr   28
        { ALU_ADD, 0, 0, 0, 0, 1, BARREL_SLL, 0, 0, 0, 0, 1, 0, 0, 0, IK_JR }, // jalr 29
        { ALU_ADD, 0, 0, 1, 0, 0, BARREL_SLL, 1, 0, 0, 0, 1, 0, 0, 0, IK_LAM }, // sll 30
        { ALU_ADD, 0, 0, 1, 0, 0, BARREL_SRL, 1, 0, 0, 0, 1, 0, 0, 0, IK_LAM }, // srl 31
        { ALU_ADD, 0, 0, 1, 0, 0, BARREL_SRA, 1, 0, 0, 0, 1, 0, 0, 0, IK_LAM }, // sra 32
        { ALU_ADD, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP 33
        { ALU_ADD, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP 34
        { ALU_ADD, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP 35
        { ALU_ADD, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP 36
        { ALU_ADD, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP 37
        { ALU_ADD, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP 38
        { ALU_ADD, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP 39
        { ALU_ADD, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP 40
        { ALU_ADD, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP 41
        { ALU_ADD, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP 42
        { ALU_ADD, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP 43
        { ALU_ADD, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP 44
        { ALU_ADD, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP 45
        { ALU_ADD, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP 46
        { ALU_ADD, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP 47
        { ALU_ADD, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP 48
        { ALU_ADD, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP 49
        { ALU_ADD, 0, 0, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, 0, IK_LAM }, // NOP 50

        // shadd
        { ALU_ADD, 0, 0, 1, 0, 0, BARREL_SLL, 0, 1, 0, 0, 1, 0, 0, 0, IK_LAM }, // shadd sll 51
        { ALU_ADD, 0, 0, 1, 0, 0, BARREL_SRL, 0, 1, 0, 0, 1, 0, 0, 0, IK_LAM }, // shadd srl 52
        { ALU_ADD, 0, 0, 1, 0, 0, BARREL_SRA, 0, 1, 0, 0, 1, 0, 0, 0, IK_LAM }, // shadd sra 53

        /* Type IV */
        { ALU_ADD, 1, 1, 0, 0, 0, BARREL_SLL, 0, 0, 0, 0, 0, 0, 0, 0, IK_B },// b bn 54

        /* Type V */
        { ALU_LUI, 0, 1, 0, 0, 0, BARREL_SLL, 0, 0, 1, 0, 1, 0, 0, 0, IK_LAM }, // lui
        { ALU_LLI, 0, 1, 0, 0, 0, BARREL_SLL, 0, 0, 1, 0, 1, 0, 0, 0, IK_LAM }  // lli
    };
}

#endif
