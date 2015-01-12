#include "RegisterFile.h"

using namespace HivekMultithreadEmulator;

RegisterFile::RegisterFile() {
    ra[0] = register_pool.create_register("register_file:ra[0]", 5);
    ra[1] = register_pool.create_register("register_file:ra[1]", 5);

    rb[0] = register_pool.create_register("register_file:rb[0]", 5);
    rb[1] = register_pool.create_register("register_file:rb[1]", 5);

    rc[0] = register_pool.create_register("register_file:rc[0]", 5);
    rc[1] = register_pool.create_register("register_file:rc[1]", 5);

    vrc[0] = register_pool.create_register("register_file:vrc[0]", 32);
    vrc[1] = register_pool.create_register("register_file:vrc[1]", 32);

    wren[0] = register_pool.create_register("register_file:wren[0]", 1);
    wren[1] = register_pool.create_register("register_file:wren[1]", 1);

    thread_r[0] = register_pool.create_register("register_file:thread_r[0]", 5);
    thread_r[1] = register_pool.create_register("register_file:thread_r[1]", 5);

    thread_w[0] = register_pool.create_register("register_file:thread_w[0]", 5);
    thread_w[1] = register_pool.create_register("register_file:thread_w[1]", 5);

    for (int i = 0; i < 16; ++i) {
        registers[i][0] = 0;
    }
}

void RegisterFile::cycle() {
    // TODO
}

void RegisterFile::update() {
    register_pool.update();
}

void RegisterFile::write(int lane, u32 thread, u32 wren, u32 rc, u32 vrc) {
    this->thread_w[lane]->write(thread);
    this->wren[lane]->write(wren);
    this->rc[lane]->write(rc);
    this->vrc[lane]->write(vrc);

    thread = this->thread_w[lane]->read();
    wren   = this->wren[lane]->read();
    vrc    = this->vrc[lane]->read();
    rc     = this->rc[lane]->read();

    if (rc != 0 && wren) {
        registers[thread][rc] = vrc;
    }
}

u32 RegisterFile::read_ra(int lane, u32 thread, u32 ra) {
    this->ra[lane]->write(ra);
    this->thread_r[lane]->write(thread);

    return get_vra(lane);
}

u32 RegisterFile::read_rb(int lane, u32 thread, u32 rb) {
    this->rb[lane]->write(rb);
    this->thread_r[lane]->write(thread);

    return get_vrb(lane);
}

u32 RegisterFile::get_vra(int lane) {
    u32 thread = this->thread_r[lane]->read();
    u32 ra = this->ra[lane]->read();

    if (lane == 0) {
        return registers[thread][ra];
    } else {
        return 
    }
}

u32 RegisterFile::get_vrb(int lane) {
    u32 thread = this->thread_r[lane]->read();
    u32 rb = this->rb[lane]->read();

    return registers[thread][rb];
}

