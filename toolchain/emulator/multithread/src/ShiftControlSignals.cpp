#include "ShiftControlSignals.h"
using namespace HivekMultithreadEmulator;

void ShiftControlSignals::generate_controls_for_lane(int lane, ControlTable* ct) {
    sh_type[lane][0]->write(ct->sh_type);
    sh_type[lane][1]->write(sh_type[lane][0]->read());

    sh_amnt_src[lane][0]->write(ct->sh_amnt_src);
    sh_amnt_src[lane][1]->write(sh_amnt_src[lane][0]->read());

    sh_add[lane][0]->write(ct->sh_add);
    sh_add[lane][1]->write(sh_add[lane][0]->read());

    sh_immd[lane][0]->write(get_sh_immd(inst));
    sh_immd[lane][1]->write(sh_immd[lane][0]->read());
    sh_immd[lane][2]->write(sh_immd[lane][1]->read());
}
