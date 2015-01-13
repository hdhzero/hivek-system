#include "../include/Hivek.h"
using namespace HivekMultithreadEmulator;

void Hivek::reset() {
    // TODO
}

void Hivek::cycle() {
    // TODO
}

void Hivek::update() {
    // TODO
}

void Hivek::set_memory_hierarchy(MemoryHierarchy* ptr) {
    mem = ptr;
}

void Hivek::fetch() {
    // TODO
}

void Hivek::expand() {
    // TODO
}

void Hivek::decode() {
    // TODO
}

void Hivek::execute() {
    // TODO
}

void Hivek::writeback() {
    // TODO
}

Hivek::Hivek() {
    // TODO
    dpath.set_ctrl(&ctrl);
    dpath.set_rpool(&rpool);
    dpath.set_regfile(&regfile);
    dpath.set_mem(mem);

    dpath.init();
}
