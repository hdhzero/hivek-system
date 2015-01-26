#include "../include/Framebuffer.h"
#include <iostream>
using namespace std;
using namespace HivekMultithreadEmulator;

FrameBuffer::FrameBuffer() {
    pixels = new u16[640*480];

    for (int i = 0; i < 640 * 480; ++i) {
        pixels[i] = 0x0000;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "Error initing SDL\n";
        exit(0);
    }

    window = SDL_CreateWindow("Hivek Emulator", 0, 0, 640, 480, SDL_WINDOW_SHOWN);

    if (window == NULL) {
        cout << "Error creating window in SDL\n";
        exit(0);
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    if (renderer == NULL) {
        cout << "Error creating renderer\n";
        exit(0);
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB4444, SDL_TEXTUREACCESS_STREAMING, 640, 480);

    if (texture == NULL) {
        cout << "Error creating texture\n";
        exit(0);
    }
}

FrameBuffer::~FrameBuffer() {
    delete[] pixels;
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void FrameBuffer::write_pixel(u32 addr, u16 pixel) {
    if (addr < 640 * 480) {
        pixels[addr] = pixel;
    }
}

void FrameBuffer::draw() {
    SDL_UpdateTexture(texture, NULL, pixels, 640 * sizeof(uint16_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

