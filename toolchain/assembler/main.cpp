#include "HivekAssembler.h"
#include <iostream>

int main(int argc, char** argv) {
    HivekAssembler assembler;

    std::cout << "64/24/16 missing\n";
    assembler.parse(std::string(argv[1]));
    assembler.generate_binimg();
    assembler.dump_bin(argv[2]);

    return 0;
}
