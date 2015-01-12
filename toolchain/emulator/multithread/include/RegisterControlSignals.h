#ifndef HIVEK_MULTITHREAD_EMULATOR_REGISTER_CONTROL_SIGNALS_H
#define HIVEK_MULTITHREAD_EMULATOR_REGISTER_CONTROL_SIGNALS_H

namespace HivekMultithreadEmulator {
    class RegisterControlSignals {
        private:
            Register<u32>* r_dst_src[N_LANES];
            Register<u32>* r_wren[N_LANES][4];
            Register<u32>* ra;
            Register<u32>* rb[N_LANES][2];
            Register<u32>* rc[N_LANES][5];
    };
}

#endif
