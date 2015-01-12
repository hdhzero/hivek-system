#include "../include/DatapathSignals.h"
using namespace HivekMultithreadEmulator;

u32 DatapathSignals::get_instruction_in_lane(int lane) {
    // TODO
    return 0;
}

void DatapathSignals::generate_alu_res_for_lane(int lane) {
    u32 aluOp = ctrl->get_alu_op(lane);
    u32 op_a  = get_alu_first_operand(lane);
    u32 op_b  = get_alu_second_operand(lane);
    u32 res   = alu(aluOp, op_a, op_b);

    alu_res[lane]->write(res);
}

u32 DatapathSignals::get_alu_first_operand(int lane) {
    bool selector = ctrl->get_alu_pc_vra_sel(lane);

    if (selector) {
        return pcs[lane][5]->read();
    } else {
        return vra[lane]->read();
    }
}

u32 DatapathSignals::get_alu_second_operand(int lane) {
    bool selector = ctrl->get_alu_vrb_immediate_sel(lane);

    if (selector) {
        return immediate[lane][2]->read();
    } else {
        return vrb[lane]->read();
    }
}

void DatapathSignals::generate_sh_res_for_lane(int lane) {
    u32 sh_t = ctrl->get_sh_type(lane);
    u32 sh_a = ctrl->get_sh_add(lane);
    u32 sh_i = get_sh_amount(lane);
    u32 va   = vra[lane]->read();
    u32 vb   = vrb[lane]->read();
    u32 res  = barrel_shifter(sh_t, va, sh_i);
    
    if (sh_a) {
        res += vb;
    }

    sh_res[lane]->write(res);
}

void DatapathSignals::generate_alu_sh_res_for_lane(int lane) {
    bool selector = ctrl->get_alu_sh_sel(lane);

    if (selector) {
        alu_sh_res[lane]->write(sh_res[lane]->read());
    } else {
        alu_sh_res[lane]->write(alu_res[lane]->read());
    }
}

u32 DatapathSignals::get_sh_amount(int lane) {
    bool selector = ctrl->get_sh_amount_sel(lane);

    if (selector) {
        return vrb[lane]->read() & 0x01F;
    } else {
        return sh_immediate[lane][2]->read() & 0x01F;
    }
}

u32 DatapathSignals::alu(u32 op, u32 a, u32 b) {
    switch (op) {
        case ALU_ADD:
            return a + b;

        case ALU_SUB:
            return a - b;

        case ALU_AND:
            return a & b;

        case ALU_OR:
            return a | b;

        case ALU_NOR:
            return ~(a | b);

        case ALU_XOR:
            return a ^ b;
    }

    return 0;
}

u32 DatapathSignals::barrel_shifter(u32 shift_type, u32 a, u32 shift_ammount) {
    switch (shift_type) {
        case BARREL_SLL:
            return a << shift_ammount;

        case BARREL_SRL:
            return a >> shift_ammount;

        case BARREL_SRA:
            return ((i32) a) >> shift_ammount;
    }
}

void DatapathSignals::init(ControlSignals* ctrl) {
    this->ctrl = ctrl;

    vra[0] = rpool->create_register("vra[0]", 32);
    vra[1] = rpool->create_register("vra[1]", 32);

    vrb[0] = rpool->create_register("vrb[0]", 32);
    vrb[1] = rpool->create_register("vrb[1]", 32);

    immediate[0][0] = rpool->create_register("immediate[0][0]", 32);
    immediate[0][1] = rpool->create_register("immediate[0][1]", 32);
    immediate[0][2] = rpool->create_register("immediate[0][2]", 32);
    immediate[1][0] = rpool->create_register("immediate[1][0]", 32);
    immediate[1][1] = rpool->create_register("immediate[1][1]", 32);
    immediate[1][2] = rpool->create_register("immediate[1][2]", 32);

    alu_res[0] = rpool->create_register("alu_res[0]", 32);
    alu_res[1] = rpool->create_register("alu_res[1]", 32);

    sh_res[0] = rpool->create_register("sh_res[0]", 32);
    sh_res[1] = rpool->create_register("sh_res[1]", 32);

    alu_sh_res[0] = rpool->create_register("alu_sh_res[0]", 32);
    alu_sh_res[1] = rpool->create_register("alu_sh_res[1]", 32);

    mem_res[0] = rpool->create_register("mem_res[0]", 32);
    mem_res[1] = rpool->create_register("mem_res[1]", 32);
}
