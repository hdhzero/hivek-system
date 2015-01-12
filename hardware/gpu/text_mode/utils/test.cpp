#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

int k = 0;
void plot_char(int ch, int video[480][640], int cx, int cy, vector<char>& font) {
    cx = cx * 8;
    cy = cy * 16;
    ch = ch * 8 * 16;
    
    for (int i = cy; i < cy + 16; ++i) {
        for (int j = cx; j < cx + 8; ++j) {
            video[i][j] = 0;

            if (font[ch] == '0') {
                video[i][j] = 1;
            }

            ++ch;
        }
    }
}

void init_char_buffer(int chars[40][80]) {
    int ch = 0;

    for (int i = 0; i < 40; ++i) {
        for (int j = 0; j < 80; ++j) {
    //        chars[i][j] = (ch % 128);
            chars[i][j] = '\0';
            ++ch;
        }
    }
}

void init_video(int video[480][640]) {
    for (int i = 0; i < 480; ++i) {
        for (int j = 0; j < 640; ++j) {
            video[i][j] = 0;
        }
    }
}

void read_font(vector<char>& font, string filename) {
    ifstream file;
    char ch;

    file.open(filename.c_str());

    while (file >> ch) {
        font.push_back(ch);
    }

    file.close();
}

void save_font(vector<char>& font) {
    ofstream file;

    file.open("dump_font.txt");

    for (int i = 0; i < font.size(); ++i) {
        file << font[i];
    }

    file.close();
}

void save_image(string filename, int video[480][640]) {
    ofstream file;

    file.open(filename.c_str());

    file << "P3\n640 480\n255\n";

    for (int i = 0; i < 480; ++i) {
        for (int j = 0; j < 640; ++j) {
            if (!video[i][j]) {
                file << "0 0 0\n";
            } else {
                file << "255 255 255\n";
            }
        }
    }

    file.close();
}

void write_string(int chars[40][80], int& r, int& c, string str) {
    char ch;

    for (int i = 0; i < str.size(); ++i) {
        chars[r][c] = str[i];

        if (c < 79) {
            ++c;
        } else {
            c = 0;

            if (r < 29) {
                ++r;
            } else {
                r = 0;
            }
        }
    }
}

int main() {
    int chars[40][80];
    int video[480][640];
    int row = 0;
    int column = 0;
    vector<char> font;
    string str;

    init_char_buffer(chars);
    init_video(video);
    read_font(font, "font2.txt");

    getline(cin, str);
    write_string(chars, row, column, str);

    getline(cin, str);
    write_string(chars, row, column, str);

    for (int i = 0; i < 30; ++i) {
        for (int j = 0; j < 80; ++j) {
            int ch = chars[i][j];
            plot_char(ch, video, j, i, font);
        }
    }
    
    save_image("img.ppm", video);
    save_font(font);

    return 0;
}
