#include "DatapathSignals.h"
using namespace HivekMultithreadEmulator;

void DatapathSignals::generate_alu_res_for_lane(int lane) {
    u32 aluOp = ctrl->get_alu_op(lane);
    u32 op_a  = get_alu_first_operand(lane);
    u32 op_b  = get_alu_second_operand(lane);
    u32 res   = alu(aluOp, op_a, op_b);

    alu_res[lane]->write(res);
}

u32 DatapathSignals::get_alu_first_operand(int lane) {
    bool selector = ctrl->get_pc_vra_sel(lane);

    if (selector) {
        return pcs[lane][5]->read();
    } else {
        return vra[lane]->read();
    }
}

u32 DatapathSignals::get_alu_second_operand(int lane) {
    bool selector = ctrl->get_vrb_immediate_sel(lane);

    if (selector) {
        return immediate[lane][2]->read();
    } else {
        return vrb[lane][1]->read();
    }
}

void DatapathSignals::generate_sh_res_for_lane(int lane) {
    u32 sh_t = ctrl->get_sh_type(lane);
    u32 sh_i = get_sh_amount(lane);
    u32 va   = vra[lane]->read();
    u32 vb   = vrb[lane]->read();
    u32 res  = barrel_shifter(sh_t, va, sh_i);
    
    if (sh_add[lane][1]->read()) {
        res += vb;
    }

    sh_res[lane]->write(res);
}

void DatapathSignals::generate_alu_sh_res_for_lane(int lane) {
    bool selector = ctrl->get_alu_sh_src(lane);

    if (selector) {
        alu_sh_res[lane]->write(sh_res[lane]->read());
    } else {
        alu_sh_res[lane]->write(alu_res[lane]->read());
    }
}

u32 DatapathSignals::get_sh_amount(int lane) {
    bool selector = ctrl->get_sh_amnt_src(lane);

    if (selector) {
        return vrb[lane]->read() & 0x01F;
    } else {
        return sh_immediate[lane][2]->read() & 0x01F;
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
