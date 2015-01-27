#ifndef HIVEK_MULTITHREAD_EMULATOR_GPU_H
#define HIVEK_MULTITHREAD_EMULATOR_GPU_H

#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include "Defines.h"

namespace HivekMultithreadEmulator {
    class GPU {
        public:
            GPU(); 
            ~GPU();
            void write_register(u32 addr, u32 data);
            void write_pixel(u32 addr, u16 pixel);
            void write_char(int ch, int r, int c);
            void draw();

        private:
            void plot_char(int ch, int r, int c, int fg, int bg);
            void plot_char_matrix();
            void read_font(std::string filename);

        private:
            int mode;
            int char_matrix[30][80];
            std::vector<char> font_rom;

        private:
            SDL_Window* window;
            SDL_Renderer* renderer;
            SDL_Texture* texture;
            uint16_t* pixels;

    };
}

#endif
