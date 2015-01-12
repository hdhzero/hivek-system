#ifndef HIVEK_MULTITHREAD_EMULATOR_PREDICATE_CONTROL_SIGNALS_H
#define HIVEK_MULTITHREAD_EMULATOR_PREDICATE_CONTROL_SIGNALS_H

namespace HivekMultithreadEmulator {
    class PredicateControlSignals {
        private:
            Register<u32>* predicate_value[N_LANES][3];
            Register<u32>* predicate_rvalue[N_LANES][2];
            Register<u32>* predicate_register[N_LANES][2];
    };
}

#endif
