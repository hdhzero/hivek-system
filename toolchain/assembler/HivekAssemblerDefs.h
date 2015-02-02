#ifndef HIVEK_DEFS_H
#define HIVEK_DEFS_H

#define TYPE_I_MASK  0x070000000
#define TYPE_II_MASK 0x078000000
#define TYPE_III_MASK 0x07C000000
#define TYPE_IV_MASK 0x07FE00000
#define TYPE_V_MASK  0x07E000000
#define TYPE_VI_MASK 0x07FFC0000

// Enumeration to map string <--> integer
enum Opcode {
    ADD, SUB, AND, OR, XOR, NOR, SLL, SRL, SRA, SHADD,
    ADDI, ANDI, XORI, ORI,
    EQ, EQI, LT, LTU, LTI, LTIU,
    LW, LH, LB, LHU, LBU, SW, SH, SB,
    B, BN, J, JAL, JR, JALR,
    LUI, LLI,

    ADDM, SUBM, ANDM, ORM, XORM, NORM, SLLM, SRLM, SRAM, 
    EQM, LTM, LTUM,
    JRM, JALRM,

    ADDI_L, ANDI_L, ORI_L, XORI_L, EQI_L, LTI_L, LTIU_L, 
    LW_L, LH_L, LB_L, LHU_L, LBU_L, SW_L, SH_L, SB_L
    
};

// Enumeration that stores the values from opcodes
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

    OP_LUI = 1,
    OP_LLI = 0,

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

// Maps string <--> integer
enum DirectiveType {
    ASCII, ASCIIZ, 
    H8, H16, H32,
    D8, D16, D32,
    U8, U16, U32,
    R8, R16, R32,
    C8,
    INCBIN
};

class Code {
    public:
        int kind;
        int index;
};

class Instruction {
    public:
        bool parallel;
        int size;
        int address;
        int type;
        int opcode;
        int rc;
        int ra;
        int rb;
        int shadd_type;
        int shadd_ammount;
        int predicate_register;
        int predicate_value;
        std::string immediate;

    public:
        void print() {
            std::cout << "@" << rc << ' ' << ra << ' ' << rb << '\n';
        }
};


class Label {
    public:
        int address;
        std::string label;

    public:
        void print() {
            std::cout << label << '\n';
        }
};


class Directive {
    public:
        int address;
        int kind;
        std::vector<std::string> data;

    public:
        void print() {
            for (int i = 0; i < data.size(); ++i) {
                std::cout << data[i] << ' ';
            }

            std::cout << '\n';
        }
};

#endif
