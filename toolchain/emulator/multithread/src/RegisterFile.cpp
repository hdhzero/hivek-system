#include "../include/RegisterFile.h"

using namespace HivekMultithreadEmulator;

void RegisterFile::set_rpool(RegisterPool* rpool) {
    this->rpool = rpool;
}

void RegisterFile::init() {
    ra[0][0] = rpool->create_register("regfile:ra[0][0]", 5);
    ra[1][0] = rpool->create_register("regfile:ra[1][0]", 5);

    ra[0][1] = rpool->create_register("regfile:ra[0][1]", 5);
    ra[1][1] = rpool->create_register("regfile:ra[1][1]", 5);

    rb[0][0] = rpool->create_register("regfile:rb[0][0]", 5);
    rb[1][0] = rpool->create_register("regfile:rb[1][0]", 5);

    rb[0][1] = rpool->create_register("regfile:rb[0][1]", 5);
    rb[1][1] = rpool->create_register("regfile:rb[1][1]", 5);

    rc[0] = rpool->create_register("regfile:rc[0]", 5);
    rc[1] = rpool->create_register("regfile:rc[1]", 5);

    vrc[0] = rpool->create_register("regfile:vrc[0]", 32);
    vrc[1] = rpool->create_register("regfile:vrc[1]", 32);

    wren[0] = rpool->create_register("regfile:wren[0]", 1);
    wren[1] = rpool->create_register("regfile:wren[1]", 1);

    thread_r[0][0] = rpool->create_register("regfile:thread_r[0][0]", 5);
    thread_r[1][0] = rpool->create_register("regfile:thread_r[1][0]", 5);

    thread_r[0][1] = rpool->create_register("regfile:thread_r[0][1]", 5);
    thread_r[1][1] = rpool->create_register("regfile:thread_r[1][1]", 5);

    thread_w[0] = rpool->create_register("regfile:thread_w[0]", 5);
    thread_w[1] = rpool->create_register("regfile:thread_w[1]", 5);

    for (int i = 0; i < 16; ++i) {
        registers[i][0] = 0;
    }
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

void RegisterFile::read_ra(int lane, u32 thread, u32 ra) {
    if (lane == 0) {
        this->ra[lane][1]->write(ra);
        this->thread_r[lane][1]->write(thread);
    } else {
        this->ra[lane][0]->write(ra);
        this->ra[lane][1]->write(this->ra[lane][0]->read());

        this->thread_r[lane][0]->write(thread);
        this->thread_r[lane][1]->write(thread);
    }
}

void RegisterFile::read_rb(int lane, u32 thread, u32 rb) {
    if (lane == 0) {
        this->rb[lane][1]->write(rb);
        this->thread_r[lane][1]->write(thread);
    } else {
        this->rb[lane][0]->write(rb);
        this->rb[lane][1]->write(this->rb[lane][0]->read());

        this->thread_r[lane][0]->write(thread);
        this->thread_r[lane][1]->write(thread);
    }
}

u32 RegisterFile::get_vra(int lane) {
    u32 thread = this->thread_r[lane][1]->read();
    u32 ra = this->ra[lane][1]->read();

    return registers[thread][ra];
}

u32 RegisterFile::get_vrb(int lane) {
    u32 thread = this->thread_r[lane][1]->read();
    u32 rb = this->rb[lane][1]->read();

    return registers[thread][rb];
}

