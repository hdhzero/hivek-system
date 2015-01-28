#include "../include/GPU.h"
#include <iostream>
#include <fstream>
using namespace HivekMultithreadEmulator;
using namespace std;

void GPU::plot_char(int ch, int r, int c, int fg, int bg) {
    c = c * 8;
    r = r * 16;
    ch = ch * 8 * 16;

    for (int i = r; i < r + 16; ++i) {
        for (int j = c; j < c + 8; ++j) {
            if (font_rom[ch] == '0') {
                pixels[i * 640 + j] = bg;
            } else {
                pixels[i * 640 + j] = fg;
            }

            ++ch;
        }
    }
}

void GPU::plot_char_matrix() {
    int ch;
    int fg;
    int bg;

    for (int i = 0; i < 30; ++i) {
        for (int j = 0; j < 80; ++j) {
            ch = char_matrix[i][j] & 0x0FF;
            fg = (char_matrix[i][j] & 0xF000) >> 12;
            bg = (char_matrix[i][j] & 0x0F00) >> 8;
            plot_char(ch, i, j, fg, bg);
        }
    }
}

void GPU::read_font(std::string filename) {
    std::ifstream file;
    char ch;

    file.open(filename.c_str());

    while (file >> ch) {
        font_rom.push_back(ch);
    }

    file.close();
}

GPU::GPU() {
    pixels = new u16[640*480];
    mode = GPU_TEXT_MODE;
    read_font("font_rom.txt");

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

GPU::~GPU() {
    delete[] pixels;
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void GPU::write_register(u32 addr, u32 data) {
    switch (addr) {
        case 0:
            mode = data; break;

        case 4:
            int r  = (data >> 16 + 7) & 0x1F;
            int c  = (data >> 16) & 0x07F;
            int ch = data & 0x0FFFF;
            write_char(ch, r, c);
            break;
    }
}

void GPU::write_pixel(u32 addr, u16 pixel) {
    if (addr < 640 * 480) {
        pixels[addr] = pixel;
    }
}

void GPU::write_char(int ch, int r, int c) {
    if (r >= 0 && r < 30 && c >= 0 && c < 80) {
        char_matrix[r][c] = ch;
    }
}

void GPU::draw() {
    if (mode == GPU_TEXT_MODE) {
        plot_char_matrix();
    }

    SDL_UpdateTexture(texture, NULL, pixels, 640 * sizeof(u16));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

