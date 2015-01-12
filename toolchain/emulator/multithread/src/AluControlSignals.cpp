#include "AluControlSignals.h"
using namespace HivekMultithreadEmulator;

void AluControlSignals::generate_lane(int lane, ControlTable* ct) {
    alu_op[lane][0]->write(ct->alu_op);
    alu_op[lane][1]->write(alu_op[lane][0]->read());

    alu_src[lane][0]->write(ct->alu_src);
    alu_src[lane][1]->write(alu_src[lane][0]->read());

    alu_sh_src[lane][0]->write(ct->alu_sh_src);
    alu_sh_src[lane][1]->write(alu_sh_src[lane][0]->read());
    alu_sh_src[lane][2]->write(alu_sh_src[lane][1]->read());

    alu_sh_mem_src[lane][0]->write(ct->alu_sh_mem_src);
    alu_sh_mem_src[lane][1]->write(alu_sh_mem_src[lane][0]->read());
    alu_sh_mem_src[lane][2]->write(alu_sh_mem_src[lane][1]->read());
    alu_sh_mem_src[lane][3]->write(alu_sh_mem_src[lane][2]->read());
}

